#include "EnginePCH.h"
#include "Light.h"
#include "System.h"
#include "GraphicSystem.h"
#include "LightManager.h"
#include "DepthStencil.h"

void Light::Awake()
{
	ILightManager* iLightManager = system->graphicSystem->lightManager;
	iLightManager->AddLight(this);
}

void Light::OnDestroyed()
{
	DeleteDepthes();

	ILightManager* iLightManager = system->graphicSystem->lightManager;
	iLightManager->RemoveLight(this);
}

void Light::SetDrawShadowMode(bool value)
{
	if (m_drawShadow == value)
		return;

	if (value == true)
	{
		uint projectionCount = GetProjectionCount();
		for (uint i = 0; i < projectionCount; ++i)
		{
			HRESULT hr = S_OK;
			hr = DepthStencil::Create(
				system->graphicSystem->device,
				m_depthSize, m_depthSize,
				false, DepthStencil::Type::SRV_DEPTH,
				&m_depthes[i]);

			if (FAILED(hr))
			{
				ERROR_MESSAGE("Light::SetDrawShadowMode::Failed to create DepthStencil for light");
				return;
			}
		}
	}
	else
	{
		DeleteDepthes();
	}

	m_drawShadow = true;
}

bool Light::IsWorking() const
{
	if (V3(m_diffuse.value).magnitude == 0 && V3(m_ambient.value).magnitude == 0)
		return false;

	return active;
}

void Light::ClearDepthes()
{
	uint projectionCount = GetProjectionCount();
	for (uint i = 0; i < projectionCount; ++i)
	{
		if (m_depthes[i] == nullptr)
			continue;

		m_depthes[i]->Clear(system->graphicSystem->deviceContext);
	}
}

void Light::DeleteDepthes()
{
	uint projectionCount = GetProjectionCount();
	for (uint i = 0; i < projectionCount; ++i)
		SafeDelete(m_depthes[i]);
}

void Light::ResizeDepthes(uint size)
{
	if (size == m_depthSize)
		return;

	m_depthSize = size;

	DeleteDepthes();

	uint projectionCount = GetProjectionCount();
	for (uint i = 0; i < projectionCount; ++i)
	{
		HRESULT hr = S_OK;
		hr = DepthStencil::Create(
			system->graphicSystem->device,
			m_depthSize, m_depthSize,
			false, DepthStencil::Type::SRV_DEPTH,
			&m_depthes[i]);

		if (FAILED(hr))
		{
			ERROR_MESSAGE("Light::SetDrawShadowMode::Failed to create DepthStencil for light");
			return;
		}
	}
}
