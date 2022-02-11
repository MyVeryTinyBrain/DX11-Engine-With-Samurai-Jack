#include "EnginePCH.h"
#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

bool LightManager::AddLight(ILight* light)
{
	vector<ILight*>& lights = m_lights[light->GetLightType()];

	const auto& find_it = std::find(lights.begin(), lights.end(), light);
	if (find_it != lights.end())
		return false;

	lights.push_back(light);

	return true;
}

bool LightManager::RemoveLight(ILight* light)
{
	vector<ILight*>& lights = m_lights[light->GetLightType()];

	const auto& find_it = std::find(lights.begin(), lights.end(), light);
	if (find_it == lights.end())
		return false;

	lights.erase(find_it);

	return true;
}

inline uint LightManager::GetLightCount(LightType type) const
{
	const auto& find_map = m_lights.find(type);
	if (find_map == m_lights.end())
		return 0;

	const vector<ILight*>& lights = find_map->second;
	return uint(lights.size());
}

inline ILight* LightManager::GetLight(LightType type, uint index) const
{
	const auto& find_map = m_lights.find(type);
	if (find_map == m_lights.end())
		return nullptr;

	const vector<ILight*>& lights = find_map->second;
	if (index >= lights.size())
		return nullptr;

	return lights[index];
}


