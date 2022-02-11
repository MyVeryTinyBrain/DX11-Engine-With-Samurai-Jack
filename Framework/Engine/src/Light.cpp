#include "EnginePCH.h"
#include "Light.h"
#include "System.h"
#include "GraphicSystem.h"
#include "LightManager.h"

void Light::Awake()
{
	ILightManager* iLightManager = system->graphicSystem->lightManager;
	iLightManager->AddLight(this);
}

void Light::OnDestroyed()
{
	ILightManager* iLightManager = system->graphicSystem->lightManager;
	iLightManager->RemoveLight(this);
}

bool Light::IsWorking() const
{
	if (V3(m_diffuse.value).magnitude == 0)
		return false;

	return active;
}
