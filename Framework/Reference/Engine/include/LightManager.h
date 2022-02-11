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

public:

	const Color& GetAmbient() const { return m_ambient; }

	void SetAmbient(const Color& value) { m_ambient = value; }

	_declspec(property(get = GetAmbient, put = SetAmbient)) const Color& ambient;

private:

	unordered_map<LightType, vector<ILight*>>	m_lights;

	Color										m_ambient = Color::white() * 0.05f;
};

ENGINE_END
