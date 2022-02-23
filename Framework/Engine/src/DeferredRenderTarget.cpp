#include "EnginePCH.h"
#include "DeferredRenderTarget.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "GraphicSystem.h"

DeferredRenderTarget::DeferredRenderTarget(Com<ID3D11Device> device, uint width, uint height)
{
	m_width = width;
	m_height = height;

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_diffuse);
	m_renderTargets.push_back(m_diffuse);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardDiffuse);
	m_forwardRenderTargets.push_back(m_forwardDiffuse);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_normal);
	m_renderTargets.push_back(m_normal);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardNormal);
	m_forwardRenderTargets.push_back(m_forwardNormal);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_worldPosition);
	m_renderTargets.push_back(m_worldPosition);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardWorldPosition);
	m_forwardRenderTargets.push_back(m_forwardWorldPosition);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_depthLightOcclusionShadow);
	m_renderTargets.push_back(m_depthLightOcclusionShadow);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardDepthLightOcclusionShadow);
	m_forwardRenderTargets.push_back(m_forwardDepthLightOcclusionShadow);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_specularPower);
	m_renderTargets.push_back(m_specularPower);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardSpecularPower);
	m_forwardRenderTargets.push_back(m_forwardSpecularPower);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_emissive);
	m_renderTargets.push_back(m_emissive);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_light);
	m_renderTargets.push_back(m_light);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardLight);
	m_forwardRenderTargets.push_back(m_forwardLight);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_specular);
	m_renderTargets.push_back(m_specular);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardSpecular);
	m_forwardRenderTargets.push_back(m_forwardSpecular);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_lightBlend);
	m_renderTargets.push_back(m_lightBlend);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_forwardLightBlend);
	m_forwardRenderTargets.push_back(m_forwardLightBlend);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_result);
	m_renderTargets.push_back(m_result);

	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_bridge);
	m_renderTargets.push_back(m_bridge);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_ssao);
	m_renderTargets.push_back(m_ssao);
	RenderTarget::Create(device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &m_dof);
	m_renderTargets.push_back(m_dof);
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

	return true;
}

void DeferredRenderTarget::Clear(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return;

	Color clearColor = Color::clear();

	for (auto& renderTarget : m_renderTargets)
	{
		deviceContext->ClearRenderTargetView(renderTarget->rtv.Get(), (FLOAT*)&clearColor);
	}
}

void DeferredRenderTarget::ClearForwards(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return;

	Color clearColor = Color::clear();

	for (auto& forwardRenderTarget : m_forwardRenderTargets)
	{
		deviceContext->ClearRenderTargetView(forwardRenderTarget->rtv.Get(), (FLOAT*)&clearColor);
	}
}

void DeferredRenderTarget::SetDeferredRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_diffuse->rtv.Get();
	arrRTV[1] = m_normal->rtv.Get();
	arrRTV[2] = m_worldPosition->rtv.Get();
	arrRTV[3] = m_depthLightOcclusionShadow->rtv.Get();
	arrRTV[4] = m_specularPower->rtv.Get();
	arrRTV[5] = m_emissive->rtv.Get();

	graphicSystem->SetRenderTargets(6, arrRTV);
}

void DeferredRenderTarget::SetForwardRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_result->rtv.Get();
	arrRTV[1] = m_normal->rtv.Get();
	arrRTV[2] = m_worldPosition->rtv.Get();
	arrRTV[3] = m_depthLightOcclusionShadow->rtv.Get();
	arrRTV[4] = m_specularPower->rtv.Get();
	arrRTV[5] = m_emissive->rtv.Get();

	graphicSystem->SetRenderTargets(6, arrRTV);
}

void DeferredRenderTarget::SetForwardLightRenderTargets(GraphicSystem* graphicSystem)
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	ZeroMemory(arrRTV, sizeof(arrRTV));
	arrRTV[0] = m_forwardDiffuse->rtv.Get();
	arrRTV[1] = m_forwardNormal->rtv.Get();
	arrRTV[2] = m_forwardWorldPosition->rtv.Get();
	arrRTV[3] = m_forwardDepthLightOcclusionShadow->rtv.Get();
	arrRTV[4] = m_forwardSpecularPower->rtv.Get();
	arrRTV[5] = emissive->rtv.Get();

	graphicSystem->SetRenderTargets(6, arrRTV);
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
