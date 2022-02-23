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

	m_CBufferManager->BeginApply(m_shaderPostProcessing->GetEffect());
	{
		m_CBufferManager->ApplyCameraBuffer(camera->GetPosition(), camera->GetDirection(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetNear(), camera->GetFar());

		m_shaderPostProcessing->SetVector("_TextureSize", V2(float(drt->width), float(drt->height)));
		m_shaderPostProcessing->SetTexture("_Diffuse", drt->diffuse->srv);
		m_shaderPostProcessing->SetTexture("_Normal", drt->normal->srv);
		m_shaderPostProcessing->SetTexture("_WorldPosition", drt->worldPosition->srv);
		m_shaderPostProcessing->SetTexture("_DepthLightOcclusionShadow", drt->depthLightOcclusionShadow->srv);

		m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
		m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);

		const SSAODesc& ssaoDesc = camera->GetSSAODesc();
		if (ssaoDesc.Enable)
		{
			Render_SSAO_WriteOcclusion(camera, ssaoDesc);
			Render_SSAO_ApplyOcclusion(camera, ssaoDesc);
		}
	}
	m_CBufferManager->EndApply();

	m_graphicSystem->RollbackRenderTarget();
}

void PostProcessing::GetTehcniqueAndPass(Type type, uint& out_technique, uint& out_pass) const
{
	uint technique = 0;
	uint pass = 0;

	switch (type)
	{
		case Type::SSAO_WirteOcclusion:
			technique = 0;
			pass = 0;
			break;
		case Type::SSAO_ApplyOcclusion:
			technique = 0;
			pass = 1;
			break;
		case Type::HorizontalBlur:
			technique = 1;
			pass = 0;
			break;
		case Type::VerticalBlur:
			technique = 1;
			pass = 1;
			break;
		case Type::HorizontalDepthBlur:
			technique = 1;
			pass = 2;
			break;
		case Type::VerticalDepthBlur:
			technique = 1;
			pass = 3;
			break;
	}

	out_technique = technique;
	out_pass = pass;
}

void PostProcessing::Render_SSAO_WriteOcclusion(ICamera* camera, const SSAODesc& ssaoDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->ssao->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::SSAO_WirteOcclusion, technique, pass);

	m_shaderPostProcessing->SetRawValue("_SSAODesc", &ssaoDesc, sizeof(SSAODesc));
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	BlurDesc blurDesc;
	blurDesc.DepthBlur = true;
	blurDesc.NumSamples = ssaoDesc.BlurNumSamples;
	blurDesc.PixelDistance = ssaoDesc.BlurPixelDistance;
	Render_Blur(blurDesc, drt->ssao, drt->bridge, drt->ssao);
}

void PostProcessing::Render_SSAO_ApplyOcclusion(ICamera* camera, const SSAODesc& ssaoDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::SSAO_ApplyOcclusion, technique, pass);

	m_shaderPostProcessing->SetRawValue("_SSAODesc", &ssaoDesc, sizeof(SSAODesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->ssao->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
}

void PostProcessing::Render_Blur(const BlurDesc& desc, RenderTarget* in, RenderTarget* bridge, RenderTarget* out)
{
	ID3D11RenderTargetView* arrRTV[2][8] = {};
	arrRTV[0][0] = bridge->rtv.Get();
	arrRTV[1][0] = out->rtv.Get();

	uint technique[2], pass[2];
	if (desc.DepthBlur)
	{
		GetTehcniqueAndPass(PostProcessing::Type::HorizontalDepthBlur, technique[0], pass[0]);
		GetTehcniqueAndPass(PostProcessing::Type::VerticalDepthBlur, technique[1], pass[1]);
	}
	else
	{
		GetTehcniqueAndPass(PostProcessing::Type::HorizontalBlur, technique[0], pass[0]);
		GetTehcniqueAndPass(PostProcessing::Type::VerticalBlur, technique[1], pass[1]);
	}

	m_shaderPostProcessing->SetRawValue("_BlurDesc", &desc, sizeof(BlurDesc));

	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV[0], nullptr);
	m_shaderPostProcessing->SetTexture("_Sample", in->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique[0], pass[0]);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique[0], pass[0]);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV[1], nullptr);
	m_shaderPostProcessing->SetTexture("_Sample", bridge->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique[1], pass[1]);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique[1], pass[1]);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
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
