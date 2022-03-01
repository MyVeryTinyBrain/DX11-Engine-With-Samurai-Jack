#include "EnginePCH.h"
#include "DeferredRenderTarget.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "GraphicSystem.h"

DeferredRenderTarget::DeferredRenderTarget(Com<ID3D11Device> device, uint width, uint height)
{
	m_width = width;
	m_height = height;

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_Diffuse);
	m_renderTargets.push_back(m_Diffuse);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_forwardDiffuse);
	m_forwardRenderTargets.push_back(m_forwardDiffuse);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_Normal);
	m_renderTargets.push_back(m_Normal);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_forwardNormal);
	m_forwardRenderTargets.push_back(m_forwardNormal);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_WorldPosition);
	m_renderTargets.push_back(m_WorldPosition);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardWorldPosition);
	m_forwardRenderTargets.push_back(m_forwardWorldPosition);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_Depth_Light_Occlusion_Shadow);
	m_renderTargets.push_back(m_Depth_Light_Occlusion_Shadow);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardDepth_Light_Occlusion_Shadow);
	m_forwardRenderTargets.push_back(m_forwardDepth_Light_Occlusion_Shadow);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R8G8B8A8_UNORM, &m_Specular_Power);
	m_renderTargets.push_back(m_Specular_Power);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R8G8B8A8_UNORM, &m_forwardSpecular_Power);
	m_forwardRenderTargets.push_back(m_forwardSpecular_Power);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_Emissive);
	m_renderTargets.push_back(m_Emissive);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R8G8B8A8_UNORM, &m_Reflection_ReflectMask);
	m_renderTargets.push_back(m_Reflection_ReflectMask);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_light);
	m_renderTargets.push_back(m_light);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_forwardLight);
	m_forwardRenderTargets.push_back(m_forwardLight);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_specular);
	m_renderTargets.push_back(m_specular);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_forwardSpecular);
	m_forwardRenderTargets.push_back(m_forwardSpecular);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_lightBlend);
	m_renderTargets.push_back(m_lightBlend);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_forwardLightBlend);
	m_forwardRenderTargets.push_back(m_forwardLightBlend);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_result);
	m_renderTargets.push_back(m_result);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_bridge);
	m_postProcessingRenderTargets.push_back(m_bridge);
	RenderTarget::Create(device, width / 2, height / 2, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_bridgeHalf);
	m_postProcessingRenderTargets.push_back(m_bridgeHalf);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_ssao);
	m_postProcessingRenderTargets.push_back(m_ssao);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_ssr);
	m_postProcessingRenderTargets.push_back(m_ssr);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_dof);
	m_postProcessingRenderTargets.push_back(m_dof);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_bloom);
	m_postProcessingRenderTargets.push_back(m_bloom);
}

DeferredRenderTarget* DeferredRenderTarget::Create(Com<ID3D11Device> device, uint width, uint height)
{
	if (!device)
		return nullptr;

	if (width * height == 0)
		return nullptr;

	return new DeferredRenderTarget(device, width, height);
}

DeferredRenderTarget::~DeferredRenderTarget()
{
	for (auto& renderTarget : m_renderTargets)
		SafeDelete(renderTarget);
	for (auto& forwardRenderTarget : m_forwardRenderTargets)
		SafeDelete(forwardRenderTarget);
	for (auto& postProcessingRenderTarget : m_forwardRenderTargets)
		SafeDelete(postProcessingRenderTarget);
}

bool DeferredRenderTarget::Resize(Com<ID3D11Device> device, uint width, uint height)
{
	bool diffWidth = m_width != width;
	bool diffHeight = m_height != height;

	m_width = width;
	m_height = height;

	if (diffWidth || diffHeight)
	{
		for (auto& renderTarget : m_renderTargets)
		{
			if (!renderTarget->Resize(device, width, height))
				return false;
		}
		for (auto& forwardRenderTarget : m_forwardRenderTargets)
		{
			if (!forwardRenderTarget->Resize(device, width, height))
				return false;
		}
	}

	// Post Processing Render Targets
	{
		if (!m_bridge->Resize(device, width, height))
			return false;
		if (!m_bridgeHalf->Resize(device, width / 2, height / 2))
			return false;
		if (!m_ssao->Resize(device, width, height))
			return false;
		if (!m_ssr->Resize(device, width, height))
			return false;
		if (!m_dof->Resize(device, width, height))
			return false;
		if (!m_bloom->Resize(device, width, height))
			return false;
	}

	return true;
}

void DeferredRenderTarget::Clear(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return;

	Color clearColor = Color::clear();

	for (auto& renderTarget : m_renderTargets)
	{
		if (renderTarget == m_Depth_Light_Occlusion_Shadow)
			renderTarget->Clear(deviceContext, Color::white());
		else
			renderTarget->Clear(deviceContext, clearColor);
	}
}

void DeferredRenderTarget::ClearForwards(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return;

	Color clearColor = Color::clear();

	for (auto& forwardRenderTarget : m_forwardRenderTargets)
	{
		if (forwardRenderTarget == m_Depth_Light_Occlusion_Shadow)
			forwardRenderTarget->Clear(deviceContext, Color::white());
		else
			forwardRenderTarget->Clear(deviceContext, clearColor);
	}
}

void DeferredRenderTarget::ClearPostProcessings(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return;

	Color clearColor = Color::clear();

	for (auto& postProcessingRenderTarget : m_postProcessingRenderTargets)
	{
		postProcessingRenderTarget->Clear(deviceContext, clearColor);
	}
}

void DeferredRenderTarget::SetDeferredRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_Diffuse->rtv.Get();
	arrRTV[1] = m_Normal->rtv.Get();
	arrRTV[2] = m_WorldPosition->rtv.Get();
	arrRTV[3] = m_Depth_Light_Occlusion_Shadow->rtv.Get();
	arrRTV[4] = m_Specular_Power->rtv.Get();
	arrRTV[5] = m_Emissive->rtv.Get();
	arrRTV[6] = m_Reflection_ReflectMask->rtv.Get();

	graphicSystem->SetRenderTargets(7, arrRTV);
}

void DeferredRenderTarget::SetForwardRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_result->rtv.Get();
	arrRTV[1] = m_Normal->rtv.Get();
	arrRTV[2] = m_WorldPosition->rtv.Get();
	arrRTV[3] = m_Depth_Light_Occlusion_Shadow->rtv.Get();
	arrRTV[4] = m_Specular_Power->rtv.Get();
	arrRTV[5] = m_Emissive->rtv.Get();
	arrRTV[6] = m_Reflection_ReflectMask->rtv.Get();

	graphicSystem->SetRenderTargets(7, arrRTV);
}

void DeferredRenderTarget::SetForwardLightRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_forwardDiffuse->rtv.Get();
	arrRTV[1] = m_forwardNormal->rtv.Get();
	arrRTV[2] = m_forwardWorldPosition->rtv.Get();
	arrRTV[3] = m_forwardDepth_Light_Occlusion_Shadow->rtv.Get();
	arrRTV[4] = m_forwardSpecular_Power->rtv.Get();
	arrRTV[5] = emissive->rtv.Get();
	arrRTV[6] = m_Reflection_ReflectMask->rtv.Get();

	graphicSystem->SetRenderTargets(7, arrRTV);
}

void DeferredRenderTarget::SetDeferredLightAccumulateRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	arrRTV[0] = m_light->rtv.Get();
	arrRTV[1] = m_specular->rtv.Get();

	graphicSystem->SetRenderTargets(2, arrRTV);
}

void DeferredRenderTarget::SetForwardLightAccumulateRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = m_forwardLight->rtv.Get();
	arrRTV[1] = m_forwardSpecular->rtv.Get();

	graphicSystem->SetRenderTargets(2, arrRTV);
}

void DeferredRenderTarget::SetDeferredLightBlendRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	arrRTV[0] = m_lightBlend->rtv.Get();

	graphicSystem->SetRenderTargets(1, arrRTV);
}

void DeferredRenderTarget::SetForwardLightBlendRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	arrRTV[0] = m_forwardLightBlend->rtv.Get();

	graphicSystem->SetRenderTargets(1, arrRTV);
}
