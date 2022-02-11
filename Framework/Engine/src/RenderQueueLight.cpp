#include "EnginePCH.h"
#include "RenderQueueLight.h"
#include "DepthStencil.h"
#include "PrimitiveVI.h"
#include "VIBuffer.h"
#include "CompiledShaderDesc.h"
#include "LightManager.h"
#include "RenderTarget.h"
#include "DeferredRenderTarget.h"

RenderQueueLight::~RenderQueueLight()
{
    for (uint i = 0; i < 6; ++i)
    {
        SafeDelete(m_lightDepth[i]);
    }

    SafeDelete(m_normalizedQuad);

    SafeDelete(m_shaderLightDepthWrite);
    SafeDelete(m_shaderLighting);
    SafeDelete(m_shaderLightBlending);
}

HRESULT RenderQueueLight::Initialize(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferManager)
{
    HRESULT hr = RenderQueueBase::Initialize(graphicSystem, cBufferManager, instanceBufferManager);

    if (FAILED(hr))
        return hr;

    if (FAILED(hr = SetupQuad()))
        return hr;
    if (FAILED(hr = SetupShaders()))
        return hr;
    if (FAILED(hr = SetupDepthes()))
        return hr;

    return S_OK;
}

bool RenderQueueLight::IsValidInput(const RenderRequest& input) const
{
    if (!input.shadow.draw)
        return false;

    if (input.shadow.cutoffEnable && input.shadow.cutoffTexture == nullptr)
        return false;

	return true;
}

bool RenderQueueLight::AddInput(const RenderRequest& input)
{
    if (!IsValidInput(input))
        return false;

    if (input.op.boneOp == nullptr)
    {
        if (input.shadow.cutoffEnable == false)
            m_instanceRequets[input.essential.mesh].push_back(input);
        else
            m_instanceCutoffRequets[input.shadow.cutoffAlpha][input.essential.mesh].push_back(input);
    }
    else
    {
        if (input.shadow.cutoffEnable == false)
            m_skinnedRequests.push_back(input);
        else
            m_skinnedCutoffRequests.push_back(input);
    }

    return true;
}

void RenderQueueLight::Render(ICamera* camera)
{
    LightManager* lightManager = m_graphicSystem->lightManager;

    for (uint nLightType = 0; nLightType < (uint)LightType::Max; ++nLightType)
    {
        LightType lightType = (LightType)nLightType;
        uint lightCount = lightManager->GetLightCount(lightType);

        for (uint i = 0; i < lightCount; ++i)
        {
            ILight* light = lightManager->GetLight(lightType, i);

            if (!IsValidLight(camera, light))
                continue;
            
            LightDesc lightDesc = light->GetLightDesc(camera);

            Render_DepthOfLight(camera, light, lightDesc);
            Render_LightAccumulate(camera, light, lightDesc);
        }
    }

    Render_LightBlend(camera);
}

void RenderQueueLight::RenderOnce(ICamera* camera, const RenderRequest& request)
{
    LightManager* lightManager = m_graphicSystem->lightManager;

    for (uint nLightType = 0; nLightType < (uint)LightType::Max; ++nLightType)
    {
        LightType lightType = (LightType)nLightType;
        uint lightCount = lightManager->GetLightCount(lightType);

        for (uint i = 0; i < lightCount; ++i)
        {
            ILight* light = lightManager->GetLight(lightType, i);

            if (!IsValidLight(camera, light))
                continue;

            LightDesc lightDesc = light->GetLightDesc(camera);
        }
    }
}

void RenderQueueLight::Clear()
{
    if (!m_instanceRequets.empty())
        m_instanceRequets.clear();

    if (!m_instanceCutoffRequets.empty())
        m_instanceCutoffRequets.clear();

    if (!m_skinnedRequests.empty())
        m_skinnedRequests.clear();

    if (!m_skinnedCutoffRequests.empty())
        m_skinnedCutoffRequests.clear();
}

bool RenderQueueLight::IsValidShadowRequest(const RenderRequest& request, const BoundingHolder& boundingHolder) const
{
    if (!request.shadow.draw)
        return false;

    if (request.op.boundsOp && !boundingHolder.Intersects(request.op.boundsOp->GetBounds()))
        return false;

    return true;
}

void RenderQueueLight::ApplyCBufferForLight(const RenderRequest& request, const LightDesc& lightDesc, uint projectionIndex)
{
    uint i = projectionIndex;

    m_CBufferManager->ApplyWorldMatrixBuffer(request.essential.worldMatrix);
    m_CBufferManager->ApplyBoneMatricesUsageBuffer(request.op.boneOp != nullptr);
    if (request.op.boneOp != nullptr)
    {
        request.op.boneOp->OnSetBoneMatricesCBuffer(request.essential.subMeshIndex, m_CBufferManager->GetBoneMatricesBufferData());
        m_CBufferManager->ApplyBoneMatrices();
    }
    m_CBufferManager->ApplyCameraBuffer(lightDesc.Position, lightDesc.Direction, lightDesc.ViewMatrix[i], lightDesc.ProjectionMatrix[i], lightDesc.Near, lightDesc.Far);
}

bool RenderQueueLight::IsValidLight(ICamera* camera, ILight* light) const
{
    if (!light->IsWorking())
        return false;

    if (!light->ContainsInCamera(camera))
        return false;

    return true;
}

void RenderQueueLight::Render_DepthOfLight(ICamera* camera, ILight* light, const LightDesc& lightDesc)
{
    if (!light->IsDrawShadow())
        return;

    IGraphicSystem* iGraphicSystem = m_graphicSystem;

    BoundingHolder lightBounds[6];
    uint projectionCount = light->GetProjectionCount();
    light->GetBoundingHolders(camera, lightBounds);

    iGraphicSystem->SetViewport(m_lightDepthMapWidth, m_lightDepthMapHeight);
    {
        for (uint i = 0; i < 6; ++i)
            m_lightDepth[i]->Clear(m_graphicSystem->deviceContext);

        for (uint i = 0; i < projectionCount; ++i)
        {
            m_graphicSystem->SetRenderTargetsWithDepthStencil(0, nullptr, m_lightDepth[i]->dsv.Get());

            Render_DepthOfLight_Instance_NonCutoff(lightDesc, lightBounds, i);
            Render_DepthOfLight_Instance_Cutoff(lightDesc, lightBounds, i);
            Render_DepthOfLight_Skinned_NonCutoff(lightDesc, lightBounds, i);
            Render_DepthOfLight_Skinned_Cutoff(lightDesc, lightBounds, i);
        }
    }
    iGraphicSystem->RollbackViewport();

    m_graphicSystem->RollbackRenderTarget();
}

void RenderQueueLight::Render_DepthOfLight_Instance_NonCutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    m_shaderLightDepthWrite->SetInputLayout(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_INSTANCE, LIGHT_DEPTH_SHADER_PASS_NONCUTOFF);
    m_shaderLightDepthWrite->ApplyPass(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_INSTANCE, LIGHT_DEPTH_SHADER_PASS_NONCUTOFF);

    Render_DepthOfLight_Instance(lightDesc, boundings, projectionIndex, false, m_instanceRequets);
}

void RenderQueueLight::Render_DepthOfLight_Instance_Cutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    m_shaderLightDepthWrite->SetInputLayout(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_INSTANCE, LIGHT_DEPTH_SHADER_PASS_CUTOFF);
    m_shaderLightDepthWrite->ApplyPass(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_INSTANCE, LIGHT_DEPTH_SHADER_PASS_CUTOFF);

    for (auto& pairByCutoffAlpha : m_instanceCutoffRequets)
    {
        const InstanceRequets& requests = pairByCutoffAlpha.second;
        Render_DepthOfLight_Instance(lightDesc, boundings, projectionIndex, true, requests);
    }
}

void RenderQueueLight::Render_DepthOfLight_Instance(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex, bool cutoff, const InstanceRequets& requests)
{
    uint i = projectionIndex;

    for (auto& pairByMesh : requests)
    {
        const vector<RenderRequest>& requests = pairByMesh.second;
        size_t instanceRequestCount = requests.size();
        const RenderRequest& front = requests.front();

        size_t drawCount = 0;
        m_instanceBufferManager->BeginSetDatas(instanceRequestCount);
        {
            for (auto& request : requests)
            {
                if (!IsValidShadowRequest(request, boundings[i]))
                    continue;

                InstanceData data;
                data.right = request.essential.worldMatrix.row[0];
                data.up = request.essential.worldMatrix.row[1];
                data.forward = request.essential.worldMatrix.row[2];
                data.position = request.essential.worldMatrix.row[3];

                m_instanceBufferManager->SetData(uint(drawCount), &data);
                ++drawCount;
            }
        }
        m_instanceBufferManager->EndSetDatas();

        if (drawCount == 0)
            continue;

        m_CBufferManager->BeginApply(m_shaderLightDepthWrite->GetEffect());
        {
            ApplyCBufferForLight(front, lightDesc, i);

            if (cutoff)
            {
                m_shaderLightDepthWrite->SetTexture("_ShadowCutoffTexture", front.shadow.cutoffTexture->GetSRV().Get());
                m_shaderLightDepthWrite->SetFloat("_ShadowCutoffAlpha", front.shadow.cutoffAlpha);
            }

            if (FAILED(front.essential.mesh->ApplyVertexAndInstanceBuffer(m_graphicSystem->deviceContext, m_instanceBufferManager->GetBuffer()))) continue;
            if (FAILED(front.essential.mesh->ApplyIndexBuffer(m_graphicSystem->deviceContext))) continue;
            if (FAILED(front.essential.mesh->DrawInstanceSubMesh(m_graphicSystem->deviceContext, front.essential.subMeshIndex, uint(drawCount)))) continue;
        }
        m_CBufferManager->EndApply();
    }
}

void RenderQueueLight::Render_DepthOfLight_Skinned_NonCutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    m_shaderLightDepthWrite->SetInputLayout(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_SKINNED, LIGHT_DEPTH_SHADER_PASS_NONCUTOFF);
    m_shaderLightDepthWrite->ApplyPass(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_SKINNED, LIGHT_DEPTH_SHADER_PASS_NONCUTOFF);

    Render_DepthOfLight_Skinned(lightDesc, boundings, projectionIndex, false);
}

void RenderQueueLight::Render_DepthOfLight_Skinned_Cutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    m_shaderLightDepthWrite->SetInputLayout(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_SKINNED, LIGHT_DEPTH_SHADER_PASS_CUTOFF);
    m_shaderLightDepthWrite->ApplyPass(m_graphicSystem->deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE_SKINNED, LIGHT_DEPTH_SHADER_PASS_CUTOFF);

    Render_DepthOfLight_Skinned(lightDesc, boundings, projectionIndex, true);
}

void RenderQueueLight::Render_DepthOfLight_Skinned(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex, bool cutoff)
{
    uint i = projectionIndex;

    vector<RenderRequest>* pRequests = nullptr;
    if (cutoff == false)            pRequests = &m_skinnedRequests;
    else                            pRequests = &m_skinnedCutoffRequests;
    const vector<RenderRequest>& requests = *pRequests;

    for (auto& request : requests)
    {
        if (!IsValidShadowRequest(request, boundings[i]))
            continue;

        m_CBufferManager->BeginApply(m_shaderLightDepthWrite->GetEffect());
        {
            ApplyCBufferForLight(request, lightDesc, i);

            if (cutoff)
            {
                m_shaderLightDepthWrite->SetTexture("_ShadowCutoffTexture", request.shadow.cutoffTexture->GetSRV().Get());
                m_shaderLightDepthWrite->SetFloat("_ShadowCutoffAlpha", request.shadow.cutoffAlpha);
            }

            if (FAILED(request.essential.mesh->ApplyVertexBuffer(m_graphicSystem->deviceContext))) continue;
            if (FAILED(request.essential.mesh->ApplyIndexBuffer(m_graphicSystem->deviceContext))) continue;
            if (FAILED(request.essential.mesh->DrawSubMesh(m_graphicSystem->deviceContext, request.essential.subMeshIndex))) continue;
        }
        m_CBufferManager->EndApply();
    }
}

void RenderQueueLight::Render_LightAccumulate(ICamera* camera, ILight* light, LightDesc lightDesc)
{
    uint passIndex = (uint)lightDesc.Type;

    DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();

    // Inputs
    RenderTarget* normal = drt->normal;
    RenderTarget* worldPosition = drt->worldPosition;
    RenderTarget* depthLightOcclusionShadow = drt->depthLightOcclusionShadow;
    RenderTarget* specularPower = drt->specularPower;

    // Outputs
    RenderTarget* lightRenderTarget = drt->light;
    RenderTarget* specularRenderTarget = drt->specular;

    ID3D11RenderTargetView* arrRTV[8] = {};
    arrRTV[0] = lightRenderTarget->rtv.Get();
    arrRTV[1] = specularRenderTarget->rtv.Get();
    m_graphicSystem->SetRenderTargetsWithDepthStencil(2, arrRTV, nullptr);
    {
        m_CBufferManager->BeginApply(m_shaderLighting->GetEffect());
        {
            m_CBufferManager->ApplyCameraBuffer(camera->GetPosition(), camera->GetDirection(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetNear(), camera->GetFar());
        
            ID3D11ShaderResourceView* depthMapArray[6] = {};
            depthMapArray[0] = m_lightDepth[0]->srv.Get();
            depthMapArray[1] = m_lightDepth[1]->srv.Get();
            depthMapArray[2] = m_lightDepth[2]->srv.Get();
            depthMapArray[3] = m_lightDepth[3]->srv.Get();
            depthMapArray[4] = m_lightDepth[4]->srv.Get();
            depthMapArray[5] = m_lightDepth[5]->srv.Get();
            m_shaderLighting->SetTextureArray("_LightDepthMap", depthMapArray, light->GetProjectionCount());

            lightDesc.ViewMatrix[0].Transpose();
            lightDesc.ViewMatrix[1].Transpose();
            lightDesc.ViewMatrix[2].Transpose();
            lightDesc.ViewMatrix[3].Transpose();
            lightDesc.ViewMatrix[4].Transpose();
            lightDesc.ViewMatrix[5].Transpose();
            lightDesc.ProjectionMatrix[0].Transpose();
            lightDesc.ProjectionMatrix[1].Transpose();
            lightDesc.ProjectionMatrix[2].Transpose();
            lightDesc.ProjectionMatrix[3].Transpose();
            lightDesc.ProjectionMatrix[4].Transpose();
            lightDesc.ProjectionMatrix[5].Transpose();

            m_shaderLighting->SetRawValue("_LightDesc", &lightDesc, sizeof(LightDesc));
            m_shaderLighting->SetTexture("_Normal", normal->srv);
            m_shaderLighting->SetTexture("_WorldPosition", worldPosition->srv);
            m_shaderLighting->SetTexture("_DepthLightOcclusionShadow", depthLightOcclusionShadow->srv);
            m_shaderLighting->SetTexture("_SpecularPower", specularPower->srv);

            m_shaderLighting->SetInputLayout(m_graphicSystem->deviceContext, 0, passIndex);
            m_shaderLighting->ApplyPass(m_graphicSystem->deviceContext, 0, passIndex);
            m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
            m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
            m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
        }
        m_CBufferManager->EndApply();
    }
    m_graphicSystem->RollbackRenderTarget();
}

void RenderQueueLight::Render_LightBlend(ICamera* camera)
{
    DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();

    // Inputs
    RenderTarget* diffuse = drt->diffuse;
    RenderTarget* depthLightOcclusionShadow = drt->depthLightOcclusionShadow;
    RenderTarget* light = drt->light;
    RenderTarget* specular = drt->specular;

    // Outputs
    RenderTarget* lightBlend = drt->lightBlend;

    ID3D11RenderTargetView* arrRTV[8] = {};
    arrRTV[0] = lightBlend->rtv.Get();
    m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);
    {
        m_shaderLightBlending->SetColor("_Ambient", m_graphicSystem->lightManager->ambient);
        m_shaderLightBlending->SetTexture("_Diffuse", diffuse->srv);
        m_shaderLightBlending->SetTexture("_DepthLightOcclusionShadow", depthLightOcclusionShadow->srv);
        m_shaderLightBlending->SetTexture("_Light", light->srv);
        m_shaderLightBlending->SetTexture("_Specular", specular->srv);

        m_shaderLightBlending->SetInputLayout(m_graphicSystem->deviceContext, 0, 0);
        m_shaderLightBlending->ApplyPass(m_graphicSystem->deviceContext, 0, 0);
        m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
        m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
        m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
    }
    m_graphicSystem->RollbackRenderTarget();
}

HRESULT RenderQueueLight::SetupQuad()
{
    VI* vi = PrimitiveVI::CreateNormalizedQuad();
    HRESULT hr = VIBuffer::CreateVIBufferNocopy(
        m_graphicSystem->device, m_graphicSystem->deviceContext,
        &vi,
        D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
        D3D11_USAGE_IMMUTABLE, 0, 0,
        &m_normalizedQuad);
    if (FAILED(hr))
        RETURN_E_FAIL_ERROR_MESSAGE("RenderQueueLight::Initialize::Failed to create VIBuffer(Normalized Quad)");

    return S_OK;
}

HRESULT RenderQueueLight::SetupShaders()
{
    m_shaderLightDepthWrite = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredLightDepthWrite.fx"));
    if (!m_shaderLightDepthWrite)
        RETURN_E_FAIL_ERROR_MESSAGE("RenderQueueLight::Initialize::Failed to load ../Shader/DeferredLightDepthWrite.fx");

    m_shaderLighting = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredLighting.fx"));
    if (!m_shaderLighting)
        RETURN_E_FAIL_ERROR_MESSAGE("RenderQueueLight::Initialize::Failed to load ../Shader/DeferredLighting.fx");

    m_shaderLightBlending = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredLightBlending.fx"));
    if (!m_shaderLightBlending)
        RETURN_E_FAIL_ERROR_MESSAGE("RenderQueueLight::Initialize::Failed to load ../Shader/DeferredLightBlending.fx");

    return S_OK;
}

HRESULT RenderQueueLight::SetupDepthes()
{
    HRESULT hr = S_OK;
    for (uint i = 0; i < 6; ++i)
    {
        hr = DepthStencil::Create(
            m_graphicSystem->device, 
            m_lightDepthMapWidth, m_lightDepthMapHeight, 
            false, DepthStencil::Type::SRV_DEPTH, 
            &m_lightDepth[i]);

        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}
