#include "EnginePCH.h"
#include "DeferredRenderTarget.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "GraphicSystem.h"

DeferredRenderTarget::DeferredRenderTarget(Com<ID3D11Device> device, uint width, uint height)
{
	m_width = width;
	m_height = height;

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R8G8B8A8_UNORM, &m_albedo);
	m_renderTargets.push_back(m_albedo);
	
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_normal);
	m_renderTargets.push_back(m_normal);
	
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32_FLOAT, &m_depth[0]);
	m_renderTargets.push_back(m_depth[0]);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32_FLOAT, &m_depth[1]);
	m_copyTargets.push_back(m_depth[1]);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R8G8B8A8_UNORM, &m_light_shadow);
	m_renderTargets.push_back(m_light_shadow);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_roughness_metallic);
	m_renderTargets.push_back(m_roughness_metallic);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_emissive);
	m_renderTargets.push_back(m_emissive);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R8G8B8A8_UNORM, &m_occlusion_reflection_reflectionBlur_reflectMask);
	m_renderTargets.push_back(m_occlusion_reflection_reflectionBlur_reflectMask);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_light);
	m_renderTargets.push_back(m_light);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_volumetric);
	m_renderTargets.push_back(m_volumetric);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_lightBlend);
	m_renderTargets.push_back(m_lightBlend);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_result[0]);
	m_renderTargets.push_back(m_result[0]);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_result[1]);
	m_copyTargets.push_back(m_result[1]);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_bridge);
	m_postProcessingRenderTargets.push_back(m_bridge);
	RenderTarget::Create(device, width / 2, height / 2, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_bridgeHalf);
	m_postProcessingRenderTargets.push_back(m_bridgeHalf);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R8_UNORM, &m_ssao);
	m_postProcessingRenderTargets.push_back(m_ssao);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_bloom);
	m_postProcessingRenderTargets.push_back(m_bloom);
	RenderTarget::Create(device, width / 2, height / 2, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_ssr);
	m_postProcessingRenderTargets.push_back(m_ssr);
	RenderTarget::Create(device, width / 2, height / 2, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_ssrBlur);
	m_postProcessingRenderTargets.push_back(m_ssrBlur);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R16G16B16A16_UNORM, &m_dof);
	m_postProcessingRenderTargets.push_back(m_dof);
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

	for (auto& renderTarget : m_postProcessingRenderTargets)
		SafeDelete(renderTarget);

	for (auto& renderTarget : m_copyTargets)
		SafeDelete(renderTarget);
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
		for (auto& renderTarget : m_copyTargets)
		{
			if (!renderTarget->Resize(device, width, height))
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
		if (!m_bloom->Resize(device, width, height))
			return false;
		if (!m_ssr->Resize(device, width / 2, height / 2))
			return false;
		if (!m_ssrBlur->Resize(device, width / 2, height / 2))
			return false;
		if (!m_dof->Resize(device, width, height))
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
		if (renderTarget == m_depth[0])
			renderTarget->Clear(deviceContext, Color::white());
		else if (renderTarget == m_volumetric)
			renderTarget->Clear(deviceContext, Color::clear());
		else
			renderTarget->Clear(deviceContext, clearColor);
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

bool DeferredRenderTarget::ReadyToDraw(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return false;

	Clear(deviceContext);
	ClearPostProcessings(deviceContext);
	ClearCopyTargets();

	return true;
}

void DeferredRenderTarget::SetDeferredRenderTargets(GraphicSystem* graphicSystem, Com<ID3D11DeviceContext> deviceContext)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_albedo->rtv.Get();
	arrRTV[1] = m_normal->rtv.Get();
	arrRTV[2] = m_depth[0]->rtv.Get();
	arrRTV[3] = m_light_shadow->rtv.Get();
	arrRTV[4] = m_roughness_metallic->rtv.Get();
	arrRTV[5] = m_emissive->rtv.Get();
	arrRTV[6] = m_occlusion_reflection_reflectionBlur_reflectMask->rtv.Get();

	graphicSystem->SetRenderTargets(deviceContext, 7, arrRTV);
}

void DeferredRenderTarget::SetForwardRenderTargets(GraphicSystem* graphicSystem, Com<ID3D11DeviceContext> deviceContext)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_result[0]->rtv.Get();
	arrRTV[1] = m_normal->rtv.Get();
	arrRTV[2] = m_depth[0]->rtv.Get();
	arrRTV[3] = m_light_shadow->rtv.Get();
	arrRTV[4] = m_roughness_metallic->rtv.Get();
	arrRTV[5] = m_emissive->rtv.Get();
	arrRTV[6] = m_occlusion_reflection_reflectionBlur_reflectMask->rtv.Get();

	graphicSystem->SetRenderTargets(deviceContext, 7, arrRTV);
}

void DeferredRenderTarget::SetDeferredLightAccumulateRenderTargets(GraphicSystem* graphicSystem, Com<ID3D11DeviceContext> deviceContext)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	arrRTV[0] = m_light->rtv.Get();
	arrRTV[1] = m_volumetric->rtv.Get();

	graphicSystem->SetRenderTargets(deviceContext, 2, arrRTV);
}

void DeferredRenderTarget::SetDeferredLightBlendRenderTargets(GraphicSystem* graphicSystem, Com<ID3D11DeviceContext> deviceContext)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	arrRTV[0] = m_lightBlend->rtv.Get();

	graphicSystem->SetRenderTargets(deviceContext, 1, arrRTV);
}

void DeferredRenderTarget::SetDeferredVolumetricLightBlendTargets(GraphicSystem* graphicSystem, Com<ID3D11DeviceContext> deviceContext)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	arrRTV[0] = m_result[0]->rtv.Get();

	graphicSystem->SetRenderTargets(deviceContext, 1, arrRTV);
}

void DeferredRenderTarget::ClearCopyTargets()
{
	m_depthWasCopied = false;
	m_resultWasCopied = false;
}

bool DeferredRenderTarget::DepthWasCopied() const
{
	return m_depthWasCopied;
}

bool DeferredRenderTarget::ResultWasCopied() const
{
	return m_resultWasCopied;
}

void DeferredRenderTarget::CopyDepth(Com<ID3D11DeviceContext> deviceContext)
{
	deviceContext->CopyResource(m_depth[0]->texture.Get(), m_depth[1]->texture.Get());
	m_depthWasCopied = true;
}

void DeferredRenderTarget::CopyResult(Com<ID3D11DeviceContext> deviceContext)
{
	deviceContext->CopyResource(m_result[0]->texture.Get(), m_result[1]->texture.Get());
	m_resultWasCopied = true;
}
