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
	if (camera->GetPostProcessingState() == false)
		return;

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
		m_shaderPostProcessing->SetTexture("_Depth_Light_Occlusion_Shadow", drt->depth_Light_Occlusion_Shadow->srv);
		m_shaderPostProcessing->SetTexture("_Reflection_ReflectionBlur_ReflectMask", drt->reflection_ReflectionBlur_ReflectMask->srv);

		m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
		m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);

		switch (step)
		{
			case PostProcessing::Step::Deferred:
				PostProcess_Deferred(camera);
			break;
			case PostProcessing::Step::After:
				PostProcess_After(camera);
			break;
		}
	}
	m_CBufferManager->EndApply();

	m_graphicSystem->RollbackRenderTarget();
}

void PostProcessing::Blur(const BlurDesc& desc, RenderTarget* in, RenderTarget* bridge, RenderTarget* out)
{
	Render_Blur(desc, in, bridge, out);
}

void PostProcessing::PostProcess_Deferred(ICamera* camera)
{
	const SSAODesc& ssaoDesc = camera->GetSSAODesc();
	if (ssaoDesc.Enable)
	{
		Render_SSAO_WriteOcclusion(camera, ssaoDesc);
		Render_SSAO_ApplyOcclusion(camera, ssaoDesc);
	}

	const SSRDesc& ssrDesc = camera->GetSSRDesc();
	if (ssrDesc.Enable)
	{
		Render_SSR_Write(camera, ssrDesc);
		Render_SSR_Apply(camera, ssrDesc);
	}

	const DOFDesc& dofDesc = camera->GetDOFDesc();
	if (dofDesc.Enable)
	{
		Render_DOF_Write(camera, dofDesc);
		Render_DOF_Apply(camera, dofDesc);
	}
}

void PostProcessing::PostProcess_After(ICamera* camera)
{
	const FogDesc& fogDesc = camera->GetFogDesc();
	if (fogDesc.Enable)
	{
		Render_Fog_Apply(camera, fogDesc);
	}

	const BloomDesc& bloomDesc = camera->GetBloomDesc();
	if (bloomDesc.Enable)
	{
		Render_Bloom_Extract(camera, bloomDesc);
		Render_Bloom_Apply(camera, bloomDesc);
	}

	const ChromaticAberrationDesc& chromaticAberrationDesc = camera->GetChromaticAberrationDesc();
	if (chromaticAberrationDesc.Enable)
	{
		Render_ChromaticAberration_Write(camera, chromaticAberrationDesc);
		Render_ChromaticAberration_Apply(camera, chromaticAberrationDesc);
	}
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
		case Type::SSR_Write:
			technique = 0;
			pass = 2;
			break;
		case Type::SSR_Apply:
			technique = 0;
			pass = 3;
			break;
		case Type::DOF_WritePass0:
			technique = 0;
			pass = 4;
			break;
		case Type::DOF_WritePass1:
			technique = 0;
			pass = 5;
			break;
		case Type::DOF_Apply:
			technique = 0;
			pass = 6;
			break;
		case Type::Fog_Apply_Distance:
			technique = 0;
			pass = 7;
			break;
		case Type::Fog_Apply_Z:
			technique = 0;
			pass = 8;
			break;
		case Type::Bloom_Extract:
			technique = 0;
			pass = 9;
			break;
		case Type::Bloom_Apply_Add:
			technique = 0;
			pass = 10;
			break;
		case Type::Bloom_Apply_Mix:
			technique = 0;
			pass = 11;
			break;
		case Type::ChromaticAberration_Write:
			technique = 0;
			pass = 12;
			break;
		case Type::ChromaticAberration_Apply:
			technique = 0;
			pass = 13;
			break;
		case Type::HorizontalBlur:
			technique = 1;
			pass = 0;
			break;
		case Type::VerticalBlur:
			technique = 1;
			pass = 1;
			break;
		case Type::HorizontalInvDepthBlur:
			technique = 1;
			pass = 2;
			break;
		case Type::VerticalInvDepthBlur:
			technique = 1;
			pass = 3;
			break;
		case Type::HorizontalDepthBlur:
			technique = 1;
			pass = 4;
			break;
		case Type::VerticalDepthBlur:
			technique = 1;
			pass = 5;
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

	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	iGraphicSystem->SetViewport(drt->ssao->width, drt->ssao->height);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::SSAO_WirteOcclusion, technique, pass);

	m_shaderPostProcessing->SetRawValue("_SSAODesc", &ssaoDesc, sizeof(SSAODesc));
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	BlurDesc blurDesc;
	blurDesc.Type = BlurType::InvDepth;
	blurDesc.NumSamples = ssaoDesc.BlurNumSamples;
	blurDesc.PixelDistance = ssaoDesc.BlurPixelDistance;
	Render_Blur(blurDesc, drt->ssao, drt->bridgeHalf, drt->ssao);

	iGraphicSystem->SetViewport(drt->result->width, drt->result->height);
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

void PostProcessing::Render_Fog_Apply(ICamera* camera, const FogDesc& fogDesc)
{
	switch (fogDesc.Type)
	{
		case FogType::Z:
			Render_Fog_Apply_Z(camera, fogDesc);
			break;
		case FogType::Distance:
			Render_Fog_Apply_Distance(camera, fogDesc);
			break;
	}
}

void PostProcessing::Render_Fog_Apply_Distance(ICamera* camera, const FogDesc& fogDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::Fog_Apply_Distance, technique, pass);

	m_shaderPostProcessing->SetRawValue("_FogDesc", &fogDesc, sizeof(FogDesc));
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
}

void PostProcessing::Render_Fog_Apply_Z(ICamera* camera, const FogDesc& fogDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::Fog_Apply_Z, technique, pass);

	m_shaderPostProcessing->SetRawValue("_FogDesc", &fogDesc, sizeof(FogDesc));
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
}

void PostProcessing::Render_Bloom_Extract(ICamera* camera, const BloomDesc& bloomDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->bloom->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	iGraphicSystem->SetViewport(drt->bloom->width, drt->bloom->height);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::Bloom_Extract, technique, pass);

	m_shaderPostProcessing->SetRawValue("_BloomDesc", &bloomDesc, sizeof(BloomDesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->result->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	BlurDesc blurDesc;
	blurDesc.Type = BlurType::Default;
	blurDesc.NumSamples = bloomDesc.BlurNumSamples;
	blurDesc.PixelDistance = bloomDesc.BlurPixelDistance;
	Render_Blur(blurDesc, drt->bloom, drt->bridgeHalf, drt->bloom);

	iGraphicSystem->SetViewport(drt->result->width, drt->result->height);
}

void PostProcessing::Render_Bloom_Apply(ICamera* camera, const BloomDesc& bloomDesc)
{
	PostProcessing::Type bloomType = PostProcessing::Type::Bloom_Apply_Mix;
	switch (bloomDesc.Type)
	{
		case BloomType::Mix:
			bloomType = PostProcessing::Type::Bloom_Apply_Mix;
			break;
		case BloomType::Add:
			bloomType = PostProcessing::Type::Bloom_Apply_Add;
			break;
	}

	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(bloomType, technique, pass);

	m_shaderPostProcessing->SetRawValue("_BloomDesc", &bloomDesc, sizeof(BloomDesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->bloom->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
}

void PostProcessing::Render_ChromaticAberration_Write(ICamera* camera, const ChromaticAberrationDesc& chromaticAberrationDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->bridge->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	iGraphicSystem->SetViewport(drt->bridge->width, drt->bridge->height);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::ChromaticAberration_Write, technique, pass);

	m_shaderPostProcessing->SetRawValue("_ChromaticAberrationDesc", &chromaticAberrationDesc, sizeof(ChromaticAberrationDesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->result->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	iGraphicSystem->SetViewport(drt->result->width, drt->result->height);
}

void PostProcessing::Render_ChromaticAberration_Apply(ICamera* camera, const ChromaticAberrationDesc& chromaticAberrationDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::ChromaticAberration_Apply, technique, pass);

	m_shaderPostProcessing->SetRawValue("_ChromaticAberrationDesc", &chromaticAberrationDesc, sizeof(ChromaticAberrationDesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->bridge->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
}

void PostProcessing::Render_SSR_Write(ICamera* camera, const SSRDesc& ssrDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->ssr->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	iGraphicSystem->SetViewport(drt->ssr->width, drt->ssr->height);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::SSR_Write, technique, pass);

	m_shaderPostProcessing->SetRawValue("_SSRDesc", &ssrDesc, sizeof(SSRDesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->result->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	if (ssrDesc.BlurEnable)
	{
		BlurDesc blurDesc;
		blurDesc.NumSamples = ssrDesc.BlurNumSamples;
		blurDesc.PixelDistance = ssrDesc.BlurPixelDistance;
		blurDesc.Type = ssrDesc.BlurType;
		Render_Blur(blurDesc, drt->ssr, drt->bridgeHalf, drt->ssrBlur);
	}

	iGraphicSystem->SetViewport(drt->result->width, drt->result->height);
}

void PostProcessing::Render_SSR_Apply(ICamera* camera, const SSRDesc& ssrDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::SSR_Apply, technique, pass);

	m_shaderPostProcessing->SetRawValue("_SSRDesc", &ssrDesc, sizeof(SSRDesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->ssr->srv.Get());
	if (ssrDesc.BlurEnable) { m_shaderPostProcessing->SetTexture("_Other", drt->ssrBlur->srv.Get()); }
	else					{ m_shaderPostProcessing->SetTexture("_Other", drt->ssr->srv.Get()); }
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique, pass);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique, pass);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);
}

void PostProcessing::Render_DOF_Write(ICamera* camera, const DOFDesc& dofDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[2][8] = {};
	arrRTV[0][0] = drt->bridgeHalf->rtv.Get();
	arrRTV[1][0] = drt->dof->rtv.Get();

	uint technique[2], pass[2];
	GetTehcniqueAndPass(PostProcessing::Type::DOF_WritePass0, technique[0], pass[0]);
	GetTehcniqueAndPass(PostProcessing::Type::DOF_WritePass1, technique[1], pass[1]);

	m_shaderPostProcessing->SetRawValue("_DOFDesc", &dofDesc, sizeof(DOFDesc));

	IGraphicSystem* iGraphicSystem = m_graphicSystem;

	iGraphicSystem->SetViewport(drt->bridgeHalf->width, drt->bridgeHalf->height);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV[0], nullptr);
	m_shaderPostProcessing->SetTexture("_Sample", drt->result->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique[0], pass[0]);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique[0], pass[0]);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	iGraphicSystem->SetViewport(drt->result->width, drt->result->height);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV[1], nullptr);
	m_shaderPostProcessing->SetTexture("_Sample", drt->bridgeHalf->srv.Get());
	m_shaderPostProcessing->SetTexture("_Result", drt->result->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique[1], pass[1]);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique[1], pass[1]);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	iGraphicSystem->SetViewport(drt->result->width, drt->result->height);
	iGraphicSystem->SetViewport(drt->result->width, drt->result->height);
}

void PostProcessing::Render_DOF_Apply(ICamera* camera, const DOFDesc& linearDOFDesc)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = drt->result->rtv.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	uint technique, pass;
	GetTehcniqueAndPass(PostProcessing::Type::DOF_Apply, technique, pass);

	m_shaderPostProcessing->SetRawValue("_DOFDesc", &linearDOFDesc, sizeof(DOFDesc));
	m_shaderPostProcessing->SetTexture("_Sample", drt->dof->srv.Get());
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
	switch (desc.Type)
	{
		case BlurType::Default:
			GetTehcniqueAndPass(PostProcessing::Type::HorizontalBlur, technique[0], pass[0]);
			GetTehcniqueAndPass(PostProcessing::Type::VerticalBlur, technique[1], pass[1]);
			break;
		case BlurType::InvDepth:
			GetTehcniqueAndPass(PostProcessing::Type::HorizontalInvDepthBlur, technique[0], pass[0]);
			GetTehcniqueAndPass(PostProcessing::Type::VerticalInvDepthBlur, technique[1], pass[1]);
			break;
		case BlurType::Depth:
			GetTehcniqueAndPass(PostProcessing::Type::HorizontalDepthBlur, technique[0], pass[0]);
			GetTehcniqueAndPass(PostProcessing::Type::VerticalDepthBlur, technique[1], pass[1]);
			break;
	}

	m_shaderPostProcessing->SetRawValue("_BlurDesc", &desc, sizeof(BlurDesc));

	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	uint2 prevViewport = iGraphicSystem->GetViewport();

	iGraphicSystem->SetViewport(bridge->width, bridge->height);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV[0], nullptr);
	m_shaderPostProcessing->SetTexture("_Sample", in->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique[0], pass[0]);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique[0], pass[0]);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	iGraphicSystem->SetViewport(out->width, out->height);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV[1], nullptr);
	m_shaderPostProcessing->SetTexture("_Sample", bridge->srv.Get());
	m_shaderPostProcessing->SetInputLayout(m_graphicSystem->deviceContext, technique[1], pass[1]);
	m_shaderPostProcessing->ApplyPass(m_graphicSystem->deviceContext, technique[1], pass[1]);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	iGraphicSystem->SetViewport(prevViewport.x, prevViewport.y);
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
	tstring error;
	m_shaderPostProcessing = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/PostProcessing.fx"), error);
	if (!m_shaderPostProcessing)
		RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to load ../Shader/PostProcessing.fx");

	return S_OK;
}
