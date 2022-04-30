#include "EnginePCH.h"
#include "RenderQueueLight.h"
#include "DepthStencil.h"
#include "PrimitiveVI.h"
#include "VIBuffer.h"
#include "CompiledShaderDesc.h"
#include "LightManager.h"
#include "RenderTarget.h"
#include "DeferredRenderTarget.h"
#include "PostProcessing.h"

RenderQueueLight::~RenderQueueLight()
{
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

    return S_OK;
}

bool RenderQueueLight::IsValidInput(const RenderRequest& input) const
{
    if (input.shadow.shadowPass)
        return true;

    if (!input.shadow.draw)
        return false;

	return true;
}

bool RenderQueueLight::AddInput(const RenderRequest& input)
{
    if (!IsValidInput(input))
        return false;

    if (input.shadow.shadowPass && !input.essential.instance)
    {
        // 재질에 따라 나눕니다.
        Map_RenderRequestsByMesh& meshMap = m_shadowPassRequests[input.essential.material];

        // 메쉬에 따라 나눕니다.
        Vector_RenderRequests& requests = meshMap[input.essential.mesh];

        requests.push_back(input);

        return true;
    }
    else if (input.shadow.shadowPass && input.essential.instance && !input.op.boneOp)
    {
        // 재질에 따라 나눕니다.
        IMaterial* material = input.essential.material;
        IMap_RenderRequestsByMesh& meshMap = m_shadowPassInstanceRequests[material];

        // 메쉬에 따라 나눕니다.
        IMesh* mesh = input.essential.mesh;
        IMap_RenderRequestsBySubMeshIndex& subMeshIndexMap = meshMap[mesh];

        // 서브메쉬 인덱스에 따라 나눕니다.
        uint subMeshIndex = input.essential.subMeshIndex;
        Vector_RenderRequests& requests = subMeshIndexMap[subMeshIndex];

        requests.push_back(input);

        return true;
    }
    else if (!input.op.boneOp)
    {
        // 메쉬에 따라 나눕니다.
        Vector_RenderRequests& requests = m_staticRequets[input.essential.mesh];

        requests.push_back(input);

        return true;
    }
    else if (input.op.boneOp)
    {
        // 메쉬에 따라 나눕니다.
        Vector_RenderRequests& requests = m_skinnedRequests[input.essential.mesh];

        requests.push_back(input);

        return true;
    }

    return false;
}

void RenderQueueLight::RenderDepthes(ICamera* camera)
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
        }
    }
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

            Render_LightAccumulate(camera, light, lightDesc, light->GetVolumetricDesc());
        }
    }

    Render_LightBlend(camera);
}

void RenderQueueLight::PostProcessing(ICamera* camera)
{
    BlurDesc blurDesc;
    blurDesc.NumSamples = 8;
    blurDesc.PixelDistance = 25;
    blurDesc.Type = BlurType::Default;
    m_graphicSystem->postProcessing->Blur(blurDesc, camera->GetDeferredRenderTarget()->volumetric, camera->GetDeferredRenderTarget()->bridgeHalf, camera->GetDeferredRenderTarget()->volumetric);

    Render_Volumetric(camera);
}

void RenderQueueLight::Clear()
{
    if (!m_staticRequets.empty())
        m_staticRequets.clear();

    if (!m_skinnedRequests.empty())
        m_skinnedRequests.clear();

    if (!m_shadowPassRequests.empty())
        m_shadowPassRequests.clear();

    if (!m_shadowPassInstanceRequests.empty())
        m_shadowPassInstanceRequests.clear();
}

bool RenderQueueLight::IsValidShadowRequest(ICamera* camera, const RenderRequest& request, const BoundingHolder& boundingHolder) const
{
    if (!request.shadow.draw)
        return false;

    if ((camera->GetAllowedLayers() & (1 << request.essential.layerIndex)) == 0)
        return false;

    if (request.essential.cull && !CullOp(request, boundingHolder))
        return false;

    return true;
}

void RenderQueueLight::ApplyCBufferForLight(Com<ID3D11DeviceContext> deviceContext, const RenderRequest& request, const LightDesc& lightDesc, uint projectionIndex)
{
    ApplyWorldMatrix(deviceContext, request.essential.worldMatrix);
    ApplyBoneMatrices(deviceContext, request.op.boneOp, request.essential.subMeshIndex);
    ApplyCameraBuffer(deviceContext, lightDesc, projectionIndex);
}

bool RenderQueueLight::CullOp(const RenderRequest& request, const BoundingHolder& boundingHolder) const
{
    if (request.op.boundsOp && !boundingHolder.Intersects(request.op.boundsOp->GetBounds()))
        return false;

    return true;
}

void RenderQueueLight::ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, ICamera* camera, IMaterial* material, uint techniqueIndex, uint passIndex, IMaterial** inout_prevMaterial)
{
    HRESULT hr = S_OK;

    if (!inout_prevMaterial)
        return;

    // 이미 적용된 재질이라면 적용하지 않습니다.
    if (*inout_prevMaterial == material)
        return;

    *inout_prevMaterial = material;

    material->ApplyMaterial(deviceContext, camera);

    if (FAILED(hr = material->SetInputLayout(deviceContext, techniqueIndex, passIndex)))
        return;

    if (FAILED(hr = material->ApplyPass(deviceContext, techniqueIndex, passIndex)))
        return;
}

void RenderQueueLight::ApplyMesh(Com<ID3D11DeviceContext> deviceContext, IMesh* mesh, IMesh** inout_prevMesh)
{
    HRESULT hr = S_OK;

    if (!inout_prevMesh)
        return;

    // 이미 적용된 메시라면 적용하지 않습니다.
    if (*inout_prevMesh == mesh)
        return;

    *inout_prevMesh = mesh;

    if (FAILED(hr = mesh->ApplyVertexBuffer(deviceContext)))
        return;

    if (FAILED(hr = mesh->ApplyIndexBuffer(deviceContext)))
        return;
}

void RenderQueueLight::ApplyCameraBuffer(Com<ID3D11DeviceContext> deviceContext, const LightDesc& lightDesc, uint projectionIndex)
{
    uint& i = projectionIndex;
    m_CBufferManager->ApplyCameraBuffer(deviceContext, lightDesc.Position, lightDesc.Direction, lightDesc.ViewMatrix[i], lightDesc.ProjectionMatrix[i], uint2(lightDesc.DepthSize, lightDesc.DepthSize), lightDesc.Near, lightDesc.Far);
}

void RenderQueueLight::ApplyBoneMatrices(Com<ID3D11DeviceContext> deviceContext, IRendererBoneOp* boneOp, uint subMeshIndex)
{
    m_CBufferManager->ApplyBoneMatricesUsageBuffer(deviceContext, boneOp != nullptr);

    if (!boneOp)
        return;

    boneOp->OnSetBoneMatricesCBuffer(subMeshIndex, m_CBufferManager->GetBoneMatricesBufferData());
    m_CBufferManager->ApplyBoneMatrices(deviceContext);
}

void RenderQueueLight::ApplyWorldMatrix(Com<ID3D11DeviceContext> deviceContext, const M4& worldMatrix)
{
    m_CBufferManager->ApplyWorldMatrixBuffer(deviceContext, worldMatrix);
}

void RenderQueueLight::IApplyMesh(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceDataBuffer, IMesh* mesh, IMesh** inout_prevMesh)
{
    HRESULT hr = S_OK;

    if (!inout_prevMesh)
        return;

    // 이미 적용된 메시라면 적용하지 않습니다.
    if (*inout_prevMesh == mesh)
        return;

    *inout_prevMesh = mesh;

    if (FAILED(hr = mesh->ApplyVertexAndInstanceBuffer(deviceContext, instanceDataBuffer)))
        return;

    if (FAILED(hr = mesh->ApplyIndexBuffer(deviceContext)))
        return;
}

void RenderQueueLight::IApplyBoneMatricesUsage(Com<ID3D11DeviceContext> deviceContext)
{
    m_CBufferManager->ApplyBoneMatricesUsageBuffer(deviceContext, false);
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

    Com<ID3D11Device> device = m_graphicSystem->device;
    Com<ID3D11DeviceContext> deviceContext = m_graphicSystem->deviceContext;

    IGraphicSystem* iGraphicSystem = m_graphicSystem;
    uint2 prevViewport = iGraphicSystem->GetViewport(deviceContext);

    BoundingHolder lightBounds[6];
    uint projectionCount = light->GetProjectionCount();
    uint depthSize = light->GetDepthSize();
    DepthStencil* lightDepthes[6] = {};
    light->GetBoundingHolders(camera, lightBounds);
    light->GetDepthes(lightDepthes);

    iGraphicSystem->SetViewport(deviceContext, depthSize, depthSize);
    {
        light->ClearDepthes(deviceContext);

        for (uint i = 0; i < projectionCount; ++i)
        {
            m_graphicSystem->SetRenderTargetsWithDepthStencil(deviceContext, 0, nullptr, lightDepthes[i]->dsv.Get());

            Render_DepthOfLight_Static(camera, lightDesc, lightBounds, i);
            Render_DepthOfLight_Skinned(camera, lightDesc, lightBounds, i);
            Render_DepthOfLight_ShadowPass(camera, lightDesc, lightBounds, i);
            Render_DepthOfLight_ShadowPassInstance(camera, lightDesc, lightBounds, i);
        }
    }
    iGraphicSystem->SetViewport(deviceContext, prevViewport.x, prevViewport.y);

    m_graphicSystem->RollbackRenderTarget(deviceContext);
}

void RenderQueueLight::Render_DepthOfLight_Static(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    Com<ID3D11Device> device = m_graphicSystem->device;
    Com<ID3D11DeviceContext> deviceContext = m_graphicSystem->deviceContext;

    m_shaderLightDepthWrite->SetInputLayout(deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE, LIGHT_DEPTH_SHADER_PASS_STATIC);
    m_shaderLightDepthWrite->ApplyPass(deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE, LIGHT_DEPTH_SHADER_PASS_STATIC);

    uint i = projectionIndex;

    for (auto& pairByMesh : m_staticRequets)
    {
        const Vector_RenderRequests& requests = pairByMesh.second;
        uint instanceRequestCount = uint(requests.size());
        const RenderRequest& front = requests.front();

        uint drawCount = 0;
        m_instanceBufferManager->BeginSetDatas(deviceContext, instanceRequestCount);
        {
            for (auto& request : requests)
            {
                if (!IsValidShadowRequest(camera, request, boundings[i]))
                    continue;

                InstanceData data;
                data.right = request.essential.worldMatrix.row[0];
                data.up = request.essential.worldMatrix.row[1];
                data.forward = request.essential.worldMatrix.row[2];
                data.position = request.essential.worldMatrix.row[3];

                m_instanceBufferManager->SetData(drawCount, &data);
                ++drawCount;
            }
        }
        m_instanceBufferManager->EndSetDatas(deviceContext);

        if (drawCount == 0)
            continue;

        m_CBufferManager->BeginApply(m_shaderLightDepthWrite->GetEffect());
        {
            ApplyCBufferForLight(deviceContext, front, lightDesc, i);

            if (FAILED(front.essential.mesh->ApplyVertexAndInstanceBuffer(deviceContext, m_instanceBufferManager->GetBuffer()))) continue;
            if (FAILED(front.essential.mesh->ApplyIndexBuffer(deviceContext))) continue;

            if (front.customPrimitiveCount.usePrimitiveCount)
            {
                if (FAILED(front.essential.mesh->DrawInstanceSubMesh(deviceContext, front.essential.subMeshIndex, uint(drawCount), front.customPrimitiveCount.primitiveCount))) continue;
            }
            else
            {
                if (FAILED(front.essential.mesh->DrawInstanceSubMesh(deviceContext, front.essential.subMeshIndex, uint(drawCount)))) continue;
            }
        }
        m_CBufferManager->EndApply();
    }
}

void RenderQueueLight::Render_DepthOfLight_Skinned(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    Com<ID3D11Device> device = m_graphicSystem->device;
    Com<ID3D11DeviceContext> deviceContext = m_graphicSystem->deviceContext;

    m_shaderLightDepthWrite->SetInputLayout(deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE, LIGHT_DEPTH_SHADER_PASS_SKINNED);
    m_shaderLightDepthWrite->ApplyPass(deviceContext, LIGHT_DEPTH_SHADER_TECHNIQUE, LIGHT_DEPTH_SHADER_PASS_SKINNED);

    uint i = projectionIndex;

    for (auto& pairByMesh : m_skinnedRequests)
    {
        const Vector_RenderRequests& requests = pairByMesh.second;
        uint instanceRequestCount = uint(requests.size());

        for (auto& request : requests)
        {
            if (!IsValidShadowRequest(camera, request, boundings[i]))
                continue;

            m_CBufferManager->BeginApply(m_shaderLightDepthWrite->GetEffect());
            {
                ApplyCBufferForLight(deviceContext, request, lightDesc, i);

                if (FAILED(request.essential.mesh->ApplyVertexBuffer(deviceContext))) continue;
                if (FAILED(request.essential.mesh->ApplyIndexBuffer(deviceContext))) continue;

                if (request.customPrimitiveCount.usePrimitiveCount)
                {
                    if (FAILED(request.essential.mesh->DrawSubMesh(deviceContext, request.essential.subMeshIndex, request.customPrimitiveCount.primitiveCount))) continue;
                }
                else
                {
                    if (FAILED(request.essential.mesh->DrawSubMesh(deviceContext, request.essential.subMeshIndex))) continue;
                }
            }
            m_CBufferManager->EndApply();
        }
    }
}

void RenderQueueLight::Render_DepthOfLight_ShadowPass(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    Com<ID3D11Device> device = m_graphicSystem->device;
    Com<ID3D11DeviceContext> deviceContext = m_graphicSystem->deviceContext;

    IMaterial* prevMaterial = nullptr;
    IMesh* prevMesh = nullptr;

    for (auto& pairByMaterial : m_shadowPassRequests)
    {
        Map_RenderRequestsByMesh& mapByMesh = pairByMaterial.second;
        IMaterial* material = pairByMaterial.first;

        Com<ID3DX11Effect> effect;
        material->GetEffectDesc(effect);
        m_CBufferManager->BeginApply(effect);
        {
            ApplyCameraBuffer(deviceContext, lightDesc, projectionIndex);

            for (auto& pairByMesh : mapByMesh)
            {
                Vector_RenderRequests& requests = pairByMesh.second;
                IMesh* mesh = pairByMesh.first;

                for (auto& request : requests)
                {
                    if ((camera->GetAllowedLayers() & (1 << request.essential.layerIndex)) == 0)
                        continue;

                    if (!CullOp(request, boundings[projectionIndex]))
                        continue;

                    ApplyMaterial(deviceContext, camera, material, request.essential.techniqueIndex, request.essential.passIndex, &prevMaterial);
                    ApplyMesh(deviceContext, mesh, &prevMesh);
                    ApplyBoneMatrices(deviceContext, request.op.boneOp, request.essential.subMeshIndex);
                    ApplyWorldMatrix(deviceContext, request.essential.worldMatrix);

                    if (request.customPrimitiveCount.usePrimitiveCount)
                    {
                        if (FAILED(mesh->DrawSubMesh(deviceContext, request.essential.subMeshIndex, request.customPrimitiveCount.primitiveCount))) continue;
                    }
                    else
                    {
                        if (FAILED(mesh->DrawSubMesh(deviceContext, request.essential.subMeshIndex))) continue;
                    }
                }
            }
        }
        m_CBufferManager->EndApply();
    }
}

void RenderQueueLight::Render_DepthOfLight_ShadowPassInstance(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex)
{
    Com<ID3D11Device> device = m_graphicSystem->device;
    Com<ID3D11DeviceContext> deviceContext = m_graphicSystem->deviceContext;

    IMaterial* prevMaterial = nullptr;
    IMesh* prevMesh = nullptr;

    for (auto& pairByMaterial : m_shadowPassInstanceRequests)
    {
        IMap_RenderRequestsByMesh& mapByMesh = pairByMaterial.second;
        IMaterial* material = pairByMaterial.first;

        Com<ID3DX11Effect> effect;
        material->GetEffectDesc(effect);
        m_CBufferManager->BeginApply(effect);

        for (auto& pairByMesh : mapByMesh)
        {
            IMap_RenderRequestsBySubMeshIndex& mapByMesh = pairByMesh.second;
            IMesh* mesh = pairByMesh.first;

            for (auto& pairBySubMeshIndex : mapByMesh)
            {
                Vector_RenderRequests& requests = pairBySubMeshIndex.second;
                uint instanceRequestCount = uint(requests.size());
                if (instanceRequestCount == 0)
                    continue;

                const RenderRequest& front = requests.front();

                uint drawCount = 0;
                m_instanceBufferManager->BeginSetDatas(deviceContext, instanceRequestCount);
                {
                    for (uint i = 0; i < instanceRequestCount; ++i)
                    {
                        const RenderRequest& request = requests[i];

                        if ((camera->GetAllowedLayers() & (1 << request.essential.layerIndex)) == 0)
                            continue;

                        if (!CullOp(request, boundings[projectionIndex]))
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
                m_instanceBufferManager->EndSetDatas(deviceContext);

                if (drawCount == 0)
                    continue;

                Com<ID3DX11Effect> effect;
                material->GetEffectDesc(effect);
                m_CBufferManager->BeginApply(effect);
                {
                    ApplyCameraBuffer(deviceContext, lightDesc, projectionIndex);
                    ApplyMaterial(deviceContext, camera, material, front.essential.techniqueIndex, front.essential.passIndex, &prevMaterial);
                    IApplyMesh(deviceContext, m_instanceBufferManager->GetBuffer(), mesh, &prevMesh);
                    ApplyWorldMatrix(deviceContext, front.essential.worldMatrix); // There's no meaning in this render queue
                    IApplyBoneMatricesUsage(deviceContext); // It always set to false(0)

                    if (front.customPrimitiveCount.usePrimitiveCount)
                    {
                        if (FAILED(mesh->DrawInstanceSubMesh(deviceContext, front.essential.subMeshIndex, uint(drawCount), front.customPrimitiveCount.primitiveCount))) continue;
                    }
                    else
                    {
                        if (FAILED(mesh->DrawInstanceSubMesh(deviceContext, front.essential.subMeshIndex, uint(drawCount)))) continue;
                    }
                }
                m_CBufferManager->EndApply();
            }
        }

        m_CBufferManager->EndApply();
    }
}

void RenderQueueLight::Render_LightAccumulate(ICamera* camera, ILight* light, LightDesc lightDesc, const VolumetricDesc& volumetricDesc)
{
    uint passIndex = (uint)lightDesc.Type;

    DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();

    // Inputs
    RenderTarget* albedo = drt->albedo;
    RenderTarget* normal = drt->normal;
    RenderTarget* depth = drt->depth;
    RenderTarget* light_shadow = drt->light_shadow;
    RenderTarget* roughness_metallic = drt->roughness_metallic;
    DepthStencil* lightDepthes[6] = {};
    light->GetDepthes(lightDepthes);

    // Outputs
    drt->SetDeferredLightAccumulateRenderTargets(m_graphicSystem, m_graphicSystem->deviceContext);

    {
        m_CBufferManager->BeginApply(m_shaderLighting->GetEffect());
        {
            m_CBufferManager->ApplyCameraBuffer(m_graphicSystem->deviceContext, camera->GetPosition(), camera->GetDirection(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetSize(), camera->GetNear(), camera->GetFar());
        
            ID3D11ShaderResourceView* depthMapArray[6] = {};
            if (light->IsDrawShadow())
            {
                for (uint i = 0; i < light->GetProjectionCount(); ++i)
                    depthMapArray[i] = lightDepthes[i]->srv.Get();
            }
            m_shaderLighting->SetTextures("_LightDepthMap", depthMapArray, light->GetProjectionCount());

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
            m_shaderLighting->SetRawValue("_VolumetricDesc", &volumetricDesc, sizeof(VolumetricDesc));
            m_shaderLighting->SetTexture("_Albedo", albedo->srv);
            m_shaderLighting->SetTexture("_Normal", normal->srv);
            m_shaderLighting->SetTexture("_Depth", depth->srv);
            m_shaderLighting->SetTexture("_Light_Shadow", light_shadow->srv);
            m_shaderLighting->SetTexture("_Roughness_Metallic", roughness_metallic->srv);

            m_shaderLighting->SetInputLayout(m_graphicSystem->deviceContext, 0, passIndex);
            m_shaderLighting->ApplyPass(m_graphicSystem->deviceContext, 0, passIndex);
            m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
            m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
            m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
        }
        m_CBufferManager->EndApply();
    }
    m_graphicSystem->RollbackRenderTarget(m_graphicSystem->deviceContext);
}

void RenderQueueLight::Render_LightBlend(ICamera* camera)
{
    DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();

    // Inputs
    RenderTarget* albedo = drt->albedo;
    RenderTarget* depth = drt->depth;
    RenderTarget* light_shadow = drt->light_shadow;
    RenderTarget* light = drt->light;
    RenderTarget* volumetric = drt->volumetric;

    // Outputs
    drt->SetDeferredLightBlendRenderTargets(m_graphicSystem, m_graphicSystem->deviceContext);

    {
        m_shaderLightBlending->SetTexture("_Albedo", albedo->srv);
        m_shaderLightBlending->SetTexture("_Depth", depth->srv);
        m_shaderLightBlending->SetTexture("_Light_Shadow", light_shadow->srv);
        m_shaderLightBlending->SetTexture("_Light", light->srv);
        m_shaderLightBlending->SetTexture("_Volumetric", volumetric->srv);

        m_shaderLightBlending->SetInputLayout(m_graphicSystem->deviceContext, 0, 0);
        m_shaderLightBlending->ApplyPass(m_graphicSystem->deviceContext, 0, 0);
        m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
        m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
        m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
    }

    m_graphicSystem->RollbackRenderTarget(m_graphicSystem->deviceContext);
}

void RenderQueueLight::Render_Volumetric(ICamera* camera)
{
    DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();

    // Inputs
    RenderTarget* albedo = drt->albedo;
    RenderTarget* depth = drt->depth;
    RenderTarget* light_shadow = drt->light_shadow;
    RenderTarget* light = drt->light;
    RenderTarget* volumetric = drt->volumetric;

    // Outputs
    drt->SetDeferredVolumetricLightBlendTargets(m_graphicSystem, m_graphicSystem->deviceContext);

    {
        m_shaderLightBlending->SetTexture("_Albedo", albedo->srv);
        m_shaderLightBlending->SetTexture("_Depth", depth->srv);
        m_shaderLightBlending->SetTexture("_Light_Shadow", light_shadow->srv);
        m_shaderLightBlending->SetTexture("_Light", light->srv);
        m_shaderLightBlending->SetTexture("_Volumetric", volumetric->srv);

        m_shaderLightBlending->SetInputLayout(m_graphicSystem->deviceContext, 0, 1);
        m_shaderLightBlending->ApplyPass(m_graphicSystem->deviceContext, 0, 1);
        m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
        m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
        m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
    }

    m_graphicSystem->RollbackRenderTarget(m_graphicSystem->deviceContext);
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
    tstring error;

    m_shaderLightDepthWrite = CompiledShaderDesc::CreateCompiledShaderFromBinaryFolder(m_graphicSystem->device, TEXT("DeferredLightDepthWrite.cso"), error);
    if (!m_shaderLightDepthWrite)
        RETURN_E_FAIL_ERROR_MESSAGE("RenderQueueLight::Initialize::Failed to load DeferredLightDepthWrite.cso");

    m_shaderLighting = CompiledShaderDesc::CreateCompiledShaderFromBinaryFolder(m_graphicSystem->device, TEXT("DeferredLighting.cso"), error);
    if (!m_shaderLighting)
        RETURN_E_FAIL_ERROR_MESSAGE("RenderQueueLight::Initialize::Failed to load DeferredLighting.cso");

    m_shaderLightBlending = CompiledShaderDesc::CreateCompiledShaderFromBinaryFolder(m_graphicSystem->device, TEXT("DeferredLightBlending.cso"), error);
    if (!m_shaderLightBlending)
        RETURN_E_FAIL_ERROR_MESSAGE("RenderQueueLight::Initialize::Failed to load DeferredLightBlending.cso");

    return S_OK;
}
