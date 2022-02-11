#pragma once

#include "Component.h"
#include "ILight.h"

ENGINE_BEGIN

class ENGINE_API Light : public Component, public ILight
{
protected:

	virtual void Awake() override;

	virtual void OnDestroyed() override;

public:

	virtual inline float GetIntensity() const override { return m_intensity; }
	inline void SetIntensity(float value) { m_intensity = value; }

	inline const Color& GetDiffuse() const { return m_diffuse; }
	inline void SetDiffuse(const Color& value) { m_diffuse = value; }

	virtual inline bool IsDrawShadow() const override { return m_drawShadow; }
	inline void SetDrawShadowMode(bool value) { m_drawShadow = value; }

	virtual inline float GetShadowWhiteness() const override { return m_shadowWhiteness; }
	inline void SetShadowWhiteness(float value) { m_shadowWhiteness = value; }

	_declspec(property(get = GetIntensity, put = SetIntensity)) float intensity;
	_declspec(property(get = GetDiffuse, put = SetDiffuse)) const Color& diffuse;
	_declspec(property(get = IsDrawShadow, put = SetDrawShadowMode)) bool drawShadow;
	_declspec(property(get = GetShadowWhiteness, put = SetShadowWhiteness)) float shadowWhiteness;

protected:

	virtual bool IsWorking() const override;

protected:

	float	m_intensity = 1.0f;

	Color	m_diffuse = Color::white();

	bool	m_drawShadow = false;
	float	m_shadowWhiteness = 0.5f;
};

ENGINE_END
