#include "EnginePCH.h"
#include "DeferredRenderTarget.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

DeferredRenderTarget::DeferredRenderTarget(Com<ID3D11Device> device, uint width, uint height)
{
	m_width = width;
	m_height = height;

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_diffuse);
	m_renderTargets.push_back(m_diffuse);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_normal);
	m_renderTargets.push_back(m_normal);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_worldPosition);
	m_renderTargets.push_back(m_worldPosition);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_depthLightOcclusionShadow);
	m_renderTargets.push_back(m_depthLightOcclusionShadow);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_specularPower);
	m_renderTargets.push_back(m_specularPower);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_emissive);
	m_renderTargets.push_back(m_emissive);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_light);
	m_renderTargets.push_back(m_light);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_specular);
	m_renderTargets.push_back(m_specular);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_lightBlend);
	m_renderTargets.push_back(m_lightBlend);

	RenderTarget::Create(device, width / 2, height / 2, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_div2a);
	RenderTarget::Create(device, width / 4, height / 4, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_div2b);
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

	SafeDelete(m_div2a);
	SafeDelete(m_div2b);
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
	}

	if (!m_div2a->Resize(device, width / 2, height / 2))
		return false;
	if (!m_div2b->Resize(device, width / 2, height / 2))
		return false;

	return true;
}

void DeferredRenderTarget::Clear(Com<ID3D11DeviceContext> deviceContext, const Color& clearColor)
{
	if (!deviceContext)
		return;

	for (auto& renderTarget : m_renderTargets)
	{
		deviceContext->ClearRenderTargetView(renderTarget->rtv.Get(), (FLOAT*)&clearColor);
	}
}
