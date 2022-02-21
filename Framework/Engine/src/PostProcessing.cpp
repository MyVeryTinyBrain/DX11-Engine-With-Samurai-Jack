#include "EnginePCH.h"
#include "PostProcessing.h"
#include "GraphicSystem.h"
#include "CBufferManager.h"
#include "CompiledShaderDesc.h"
#include "ICamera.h"
#include "DeferredRenderTarget.h"
#include "RenderTarget.h"
#include "VIBuffer.h"
#include "PrimitiveVI.h"

PostProcessing::PostProcessing(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferMaanger)
{
	m_graphicSystem = graphicSystem;
	m_CBufferManager = cBufferManager;
	m_instanceBufferManager = instanceBufferMaanger;
}

PostProcessing::~PostProcessing()
{
	SafeDelete(m_normalizedQuad);
	SafeDelete(m_shaderPostProcessing);
}

HRESULT PostProcessing::Initialize()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = SetupQuads()))
		return hr;

	if (FAILED(hr = SetupShaders()))
		return hr;

	return S_OK;
}

void PostProcessing::PostProcess(ICamera* camera, PostProcessing::Step step)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint techniqueIndex = 0;
	uint passIndex = 0;

	m_CBufferManager->BeginApply(m_shaderPostProcessing->GetEffect());
	{
		m_CBufferManager->ApplyCameraBuffer(camera->GetPosition(), camera->GetDirection(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetNear(), camera->GetFar());

		SSAODesc ssaoDesc;
		ssaoDesc.MinZ = 0.0f;
		ssaoDesc.Radius = 0.1f;

		m_shaderPostProcessing->SetRawValue("_SSAODesc", &ssaoDesc, sizeof(ssaoDesc));
		m_shaderPostProcessing->SetVector("_TextureSize", V2(float(drt->width), float(drt->height)));
		m_shaderPostProcessing->SetTexture("_Diffuse", drt->diffuse->srv);
		m_shaderPostProcessing->SetTexture("_Normal", drt->normal->srv);
		m_shaderPostProcessing->SetTexture("_WorldPosition", drt->worldPosition->srv);
		m_shaderPostProcessing->SetTexture("_DepthLightOcclusionShadow", drt->depthLightOcclusionShadow->srv);
		m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, passIndex);

		m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
		m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
		m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
		m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
	}
	m_CBufferManager->EndApply();

	m_graphicSystem->RollbackRenderTarget();
}

HRESULT PostProcessing::SetupQuads()
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

	return S_OK;
}

HRESULT PostProcessing::SetupShaders()
{
	m_shaderPostProcessing = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/PostProcessing.fx"));
	if (!m_shaderPostProcessing)
		RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to load ../Shader/PostProcessing.fx");

	return S_OK;
}
