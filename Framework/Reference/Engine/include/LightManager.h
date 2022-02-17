#pragma once

#include "ILightManager.h"

ENGINE_BEGIN

class ENGINE_API LightManager : public ILightManager
{
public:

	LightManager();

	virtual ~LightManager();

private:

	virtual bool AddLight(ILight* light) final override;

	virtual bool RemoveLight(ILight* light) final override;

public:

	inline uint GetLightCount(LightType type) const;

	inline ILight* GetLight(LightType type, uint index) const;

private:

	unordered_map<LightType, vector<ILight*>>	m_lights;

};

ENGINE_END
