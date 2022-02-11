#include "EnginePCH.h"
#include "DeferredScreenRender.h"
#include "GraphicSystem.h"
#include "PrimitiveVI.h"
#include "VIBuffer.h"
#include "VI.h"
#include "CompiledShaderDesc.h"
#include "TechniqueDesc.h"
#include "PassDesc.h"
#include "RenderTarget.h"
#include "Light.h"
#include "CBufferManager.h"
#include "InstanceBufferManager.h"
#include "RenderRequestInput.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

DeferredScreenRender::DeferredScreenRender(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferMaanger)
{
	m_graphicSystem = graphicSystem;
	m_CBufferManager = cBufferManager;
	m_instanceBufferManager = instanceBufferMaanger;
}

DeferredScreenRender::~DeferredScreenRender()
{
	SafeDelete(m_quad);
	SafeDelete(m_normalizedQuad);
	SafeDelete(m_shaderScreen);
	SafeDelete(m_shaderLightDepthWrite);
	SafeDelete(m_shaderLighting);
	SafeDelete(m_shaderLightBlending);
}

HRESULT DeferredScreenRender::Initialize()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = SetupQuads()))
		return hr;

	if (FAILED(hr = SetupShaders()))
		return hr;

	return S_OK;
}

void DeferredScreenRender::DrawTextureInClient(Com<ID3D11ShaderResourceView> in_srv, uint x, uint y, uint width, uint height, Blend blend)
{
	RECT rect = {};
	GetClientRect(m_graphicSystem->windowHandle, &rect);
	uint cw = rect.right;
	uint ch = rect.bottom;
	float dw = float(cw) * 0.5f;
	float dh = float(ch) * 0.5f * -1.0f;

	VI* vi = m_quad->GetVI();
	Vertex* vertices = vi->GetVertices();
	vertices[0].position = V3(float(x) / dw - 1.0f, float(y) / dh + 1.0f, 0.0f);
	vertices[1].position = V3(float(x + width) / dw - 1.0f, float(y) / dh + 1.0f, 0.0f);
	vertices[2].position = V3(float(x + width) / dw - 1.0f, float(y + height) / dh + 1.0f, 0.0f);
	vertices[3].position = V3(float(x) / dw - 1.0f, float(y + height) / dh + 1.0f, 0.0f);
	m_quad->UpdateVertexBuffer();

	uint techniqueIndex = 0;
	uint passIndex = 0;
	switch (blend)
	{
		case Engine::DeferredScreenRender::Blend::Blend:
			m_shaderScreen->GetPassIndexByName("Blend", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::AlphaTest:
			m_shaderScreen->GetPassIndexByName("AlphaTest", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::PerspectiveDepthVisualize:
			m_shaderScreen->GetPassIndexByName("PerspectiveDepthVisualize", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::OrthoDepthVisualize:
			m_shaderScreen->GetPassIndexByName("OrthoDepthVisualize", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::None:
			m_shaderScreen->GetPassIndexByName("NonBlend", techniqueIndex, passIndex);
			break;
	}

	// Setup Shader Variables
	m_shaderScreen->SetTexture("_Diffuse", in_srv);
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, passIndex);

	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_quad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_quad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_quad->DrawOnce(m_graphicSystem->deviceContext);
}

void DeferredScreenRender::DeferredDrawTexture(Com<ID3D11ShaderResourceView> in, Com<ID3D11RenderTargetView> out, Blend blend)
{
	// Set render target view array
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = out.Get();
	//m_graphicSystem->SetRenderTargets(1, arrRTV);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	// Select pass index
	uint techniqueIndex = 0;
	uint passIndex = 0;
	switch (blend)
	{
		case Engine::DeferredScreenRender::Blend::Blend:
			m_shaderScreen->GetPassIndexByName("Blend", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::AlphaTest:
			m_shaderScreen->GetPassIndexByName("AlphaTest", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::None:
			m_shaderScreen->GetPassIndexByName("NonBlend", techniqueIndex, passIndex);
			break;
	}

	// Setup Shader Variables
	m_shaderScreen->SetTexture("_Diffuse", in);

	// Draw
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Rollback to main render target
	m_graphicSystem->RollbackRenderTarget();
}

void DeferredScreenRender::DeferredDrawBlurTexture(
	uint2 originalViewport, uint2 outViewport, uint blur,
	Com<ID3D11ShaderResourceView> in, Com<ID3D11RenderTargetView> out)
{
	// Change Viewport
	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	iGraphicSystem->SetViewport(outViewport.x, outViewport.y);

	// Set render target view array
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = out.Get();
	//m_graphicSystem->SetRenderTargets(1, arrRTV);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	// Select pass index
	uint techniqueIndex = 0;
	uint horizontalBlurPassIndex = 0;
	uint verticalBlurPassIndex = 0;
	m_shaderScreen->GetPassIndexByName("HorizontalBlur", techniqueIndex, horizontalBlurPassIndex);
	m_shaderScreen->GetPassIndexByName("VerticalBlur", techniqueIndex, verticalBlurPassIndex);

	// Setup Shader Variables
	m_shaderScreen->SetTexture("_Diffuse", in);
	m_shaderScreen->SetRawValue("_Blur", &blur, sizeof(blur));

	// Draw by horizontal blur
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, horizontalBlurPassIndex);
	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, horizontalBlurPassIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Draw by vertical blur
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, verticalBlurPassIndex);
	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, verticalBlurPassIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Rollback Viewport
	iGraphicSystem->SetViewport(originalViewport.x, originalViewport.y);
}

void DeferredScreenRender::DeferredStepLightDepthWrite(
	uint2 originalViewport,
	ILight* in_light, ICamera* in_camera,
	const unordered_map<IMesh*, vector<RenderRequest>>& shadowRenderInstanceRequests, const vector<RenderRequest>& shadowRenderSkinnedRequests,
	DepthStencil* const* out_lightDepthes)
{
	if (!in_light->IsDrawShadow())
		return;

	IGraphicSystem* iGraphicSystem = m_graphicSystem;

	LightDesc lightDesc = in_light->GetLightDesc(in_camera);
	BoundingHolder lightBounds[8];
	in_light->GetBoundingHolders(in_camera, lightBounds);

	// Select pass index by light type
	XM_CONSTEXPR uint instanceTechniqueIndex = 0;
	XM_CONSTEXPR uint skinnedTechniqueIndex = 1;
	DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(in_light);
	PointLight* pointLight = dynamic_cast<PointLight*>(in_light);
	SpotLight* spotLight = dynamic_cast<SpotLight*>(in_light);

	if (!directionalLight && !pointLight && !spotLight)
		return;

	uint projectionCount = in_light->GetProjectionCount();
	for (uint i = 0; i < projectionCount; ++i)
	{
		// Clear
		out_lightDepthes[i]->Clear(m_graphicSystem->deviceContext);

		// Set viewport
		iGraphicSystem->SetViewport(out_lightDepthes[i]->width, out_lightDepthes[i]->height);

		// Set render target view array
		ID3D11RenderTargetView* arrRTV[8] = {};
		m_graphicSystem->SetRenderTargetsWithDepthStencil(0, arrRTV, out_lightDepthes[i]->dsv.Get());

		// Set Skinned Shader
		m_shaderLightDepthWrite->SetInputLayout(m_graphicSystem->deviceContext, skinnedTechniqueIndex, 0);
		m_shaderLightDepthWrite->ApplyPass(m_graphicSystem->deviceContext, skinnedTechniqueIndex, 0);

		// Draw Skinned
		for (auto& request : shadowRenderSkinnedRequests)
		{
			if (!request.shadow.draw)
				continue;

			// Culling
			if (request.op.boundsOp && !lightBounds[i].Intersects(request.op.boundsOp->GetBounds()))
				continue;

			m_CBufferManager->BeginApply(m_shaderLightDepthWrite->GetEffect());
			{
				m_CBufferManager->ApplyWorldMatrixBuffer(request.essential.worldMatrix);
				m_CBufferManager->ApplyBoneMatricesUsageBuffer(request.op.boneOp != nullptr);
				if (request.op.boneOp != nullptr)
					request.op.boneOp->OnSetBoneMatricesCBuffer(request.essential.subMeshIndex, m_CBufferManager->GetBoneMatricesBufferData());
				m_CBufferManager->ApplyBoneMatrices();
				m_CBufferManager->ApplyCameraBuffer(lightDesc.Position, lightDesc.Direction, lightDesc.ViewMatrix[i], lightDesc.ProjectionMatrix[i], in_camera->GetNear(), in_camera->GetFar());
			}
			m_CBufferManager->EndApply();

			// Bake Depth Map by Render Request Input
			request.essential.mesh->ApplyVertexBuffer(m_graphicSystem->deviceContext);
			request.essential.mesh->ApplyIndexBuffer(m_graphicSystem->deviceContext);
			request.essential.mesh->DrawSubMesh(m_graphicSystem->deviceContext, request.essential.subMeshIndex);
		}

		// Set Instance Shader
		m_shaderLightDepthWrite->SetInputLayout(m_graphicSystem->deviceContext, instanceTechniqueIndex, 0);
		m_shaderLightDepthWrite->ApplyPass(m_graphicSystem->deviceContext, instanceTechniqueIndex, 0);

		// Draw Instanced
		for (auto& pairByMesh : shadowRenderInstanceRequests)
		{
			const vector<RenderRequest>& requests = pairByMesh.second;
			size_t instanceRequestCount = requests.size();
			const RenderRequest& front = requests.front();

			size_t drawCount = 0;
			m_instanceBufferManager->BeginSetDatas(instanceRequestCount);
			{
				for (auto& request : requests)
				{
					if (!request.shadow.draw)
						continue;

					// Culling
					if (request.op.boundsOp && !lightBounds[i].Intersects(request.op.boundsOp->GetBounds()))
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
				m_CBufferManager->ApplyWorldMatrixBuffer(front.essential.worldMatrix);
				m_CBufferManager->ApplyBoneMatricesUsageBuffer(false);
				m_CBufferManager->ApplyBoneMatrices();
				m_CBufferManager->ApplyCameraBuffer(lightDesc.Position, lightDesc.Direction, lightDesc.ViewMatrix[i], lightDesc.ProjectionMatrix[i], in_camera->GetNear(), in_camera->GetFar());

				if (FAILED(front.essential.mesh->ApplyVertexAndInstanceBuffer(m_graphicSystem->deviceContext, m_instanceBufferManager->GetBuffer())))
					continue;

				if (FAILED(front.essential.mesh->ApplyIndexBuffer(m_graphicSystem->deviceContext)))
					continue;

				if (FAILED(front.essential.mesh->DrawInstanceSubMesh(m_graphicSystem->deviceContext, front.essential.subMeshIndex, uint(drawCount))))
					continue;
			}
			m_CBufferManager->EndApply();
		}

		// Rollback to main render target
		m_graphicSystem->RollbackRenderTarget();

		// Rollback to original viewport
		iGraphicSystem->SetViewport(originalViewport.x, originalViewport.y);
	}
}

void DeferredScreenRender::DeferredStepLighting(
	ILight* in_light, ICamera* in_camera, 
	const RenderTarget* in_normal, const RenderTarget* in_worldPositions, const RenderTarget* in_depthLightOcclusionShadow, const RenderTarget* in_specularPower,
	DepthStencil* const* in_lightDepthes,
	RenderTarget* out_light, RenderTarget* out_specular)
{
	// Set render target view array
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = out_light->rtv.Get();
	arrRTV[1] = out_specular->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(2, arrRTV, nullptr);

	uint techniqueIndex = 0;
	uint passIndex = 0;

	LightDesc lightDesc = in_light->GetLightDesc(in_camera);

	// Select pass index by light type
	DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(in_light);
	PointLight* pointLight = dynamic_cast<PointLight*>(in_light);
	SpotLight* spotLight = dynamic_cast<SpotLight*>(in_light);

	if (directionalLight)
		passIndex = 0;
	else if (pointLight)
		passIndex = 1;
	else if (spotLight)
		passIndex = 2;
	else
		return;

	ID3D11ShaderResourceView* depthMapArray[6] = {};
	depthMapArray[0] = in_lightDepthes[0]->srv.Get();
	depthMapArray[1] = in_lightDepthes[1]->srv.Get();
	depthMapArray[2] = in_lightDepthes[2]->srv.Get();
	depthMapArray[3] = in_lightDepthes[3]->srv.Get();
	depthMapArray[4] = in_lightDepthes[4]->srv.Get();
	depthMapArray[5] = in_lightDepthes[5]->srv.Get();
	m_shaderLighting->SetTextureArray("_LightDepthMap", depthMapArray, in_light->GetProjectionCount());

	m_CBufferManager->BeginApply(m_shaderLighting->GetEffect());
	{
		m_CBufferManager->ApplyCameraBuffer(in_camera->GetPosition(), in_camera->GetDirection(), in_camera->GetViewMatrix(), in_camera->GetProjectionMatrix(), in_camera->GetNear(), in_camera->GetFar());
	}
	m_CBufferManager->EndApply();

	// Setup Shader Variables
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
	m_shaderLighting->SetTexture("_Normal", in_normal->srv);
	m_shaderLighting->SetTexture("_WorldPosition", in_worldPositions->srv);
	m_shaderLighting->SetTexture("_DepthLightOcclusionShadow", in_depthLightOcclusionShadow->srv);
	m_shaderLighting->SetTexture("_SpecularPower", in_specularPower->srv);

	// Draw
	m_shaderLighting->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_shaderLighting->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Rollback to main render target
	m_graphicSystem->RollbackRenderTarget();
}

void DeferredScreenRender::DeferredStepLightBlending(
	const Color& ambient, 
	const RenderTarget* in_diffuse, const RenderTarget* in_depthLightOcclusionShadow, const RenderTarget* in_light, const RenderTarget* in_specular,
	const RenderTarget* out)
{
	// Set render target view array
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = out->rtv.Get();
	//m_graphicSystem->SetRenderTargets(1, arrRTV);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint techniqueIndex = 0;
	uint passIndex = 0;

	// Setup Shader Variables
	m_shaderLightBlending->SetColor("_Ambient", ambient);
	m_shaderLightBlending->SetTexture("_Diffuse", in_diffuse->srv);
	m_shaderLightBlending->SetTexture("_DepthLightOcclusionShadow", in_depthLightOcclusionShadow->srv);
	m_shaderLightBlending->SetTexture("_Light", in_light->srv);
	m_shaderLightBlending->SetTexture("_Specular", in_specular->srv);

	// Draw
	m_shaderLightBlending->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_shaderLightBlending->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Rollback to main render target
	m_graphicSystem->RollbackRenderTarget();
}

HRESULT DeferredScreenRender::SetupQuads()
{
	// Setup Quad
	{
		VI* vi = PrimitiveVI::CreateQuad();
		HRESULT hr = VIBuffer::CreateVIBufferNocopy(
			m_graphicSystem->device, m_graphicSystem->deviceContext,
			&vi,
			D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
			D3D11_USAGE_IMMUTABLE, 0, 0,
			&m_quad);
		if (FAILED(hr))
			RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to create VIBuffer(Quad)");
	}

	// Setup Normalized Quad
	{
		VI* vi = PrimitiveVI::CreateNormalizedQuad();
		HRESULT hr = VIBuffer::CreateVIBufferNocopy(
			m_graphicSystem->device, m_graphicSystem->deviceContext,
			&vi,
			D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
			D3D11_USAGE_IMMUTABLE, 0, 0,
			&m_normalizedQuad);
		if (FAILED(hr))
			RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to create VIBuffer(Normalized Quad)");
	}

	return S_OK;
}

HRESULT DeferredScreenRender::SetupShaders()
{
	m_shaderScreen = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredScreen.fx"));
	if (!m_shaderScreen)
		RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to load ../Shader/DeferredScreen.fx");

	m_shaderLightDepthWrite = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredLightDepthWrite.fx"));
	if (!m_shaderLightDepthWrite)
		RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to load ../Shader/DeferredLightDepthWrite.fx");

	m_shaderLighting = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredLighting.fx"));
	if (!m_shaderLighting)
		RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to load ../Shader/DeferredLighting.fx");

	m_shaderLightBlending = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredLightBlending.fx"));
	if (!m_shaderLightBlending)
		RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to load ../Shader/DeferredLightBlending.fx");

	return S_OK;
}
