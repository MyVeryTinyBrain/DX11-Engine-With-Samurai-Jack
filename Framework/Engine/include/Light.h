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

	inline const Color& GetAmbient() const { return m_ambient; }
	inline void SetAmbient(const Color& value) { m_ambient = value; }

	virtual inline bool IsDrawShadow() const override { return m_drawShadow; }
	void SetDrawShadowMode(bool value);

	virtual inline float GetShadowWhiteness() const override { return m_shadowWhiteness; }
	inline void SetShadowWhiteness(float value) { m_shadowWhiteness = value; }

	virtual inline float GetShadowBias() const { return m_shadowBias; }
	inline void SetShadowBias(float value) { m_shadowBias = value; }

	virtual inline float GetShadowFadeDistance() const { return m_shadowFadeDistance; }
	inline void SetShadowFadeDistance(float value) { m_shadowFadeDistance = value; }

	virtual inline float GetShadowBeginFadePercent() const { return m_shadowFadeBeginPercent; }
	inline void GetShadowBeginFadePercent(float value) { m_shadowFadeBeginPercent = value; }

	virtual inline uint GetDepthSize() const override { return m_depthSize; }

	_declspec(property(get = GetIntensity, put = SetIntensity)) float intensity;
	_declspec(property(get = GetDiffuse, put = SetDiffuse)) const Color& diffuse;
	_declspec(property(get = GetAmbient, put = SetAmbient)) const Color& ambient;
	_declspec(property(get = IsDrawShadow, put = SetDrawShadowMode)) bool drawShadow;
	_declspec(property(get = GetShadowWhiteness, put = SetShadowWhiteness)) float shadowWhiteness;
	_declspec(property(get = GetShadowBias, put = SetShadowBias)) float shadowBias;
	_declspec(property(get = GetShadowFadeDistance, put = SetShadowFadeDistance)) float shadowFadeDistance;
	_declspec(property(get = GetShadowBeginFadePercent, put = GetShadowBeginFadePercent)) float shadowFadeBeginPercent;
	_declspec(property(get = GetDepthSize)) uint depthSize;

public:

	bool GetVolumetricState() const { return m_volumetricLightState; }
	void SetVolumetricState(bool value) { m_volumetricLightState = value; }

	uint GetVolumetricNumSamples() const { return m_volumetricNumSamples; }
	void SetVolumetricNumSamples(uint value) { m_volumetricNumSamples = value; }

	float GetVolumetricIntensity() const { return m_volumetricIntensity; }
	void SetVolumetricIntensity(float value) { m_volumetricIntensity = value; }

	float GetVolumetricPower() const { return m_volumetricPower; }
	void SetVolumetricPower(float value) { m_volumetricPower = value; }

	_declspec(property(get = GetVolumetricState, put = SetVolumetricState)) bool volumetricLightState;
	_declspec(property(get = GetVolumetricNumSamples, put = SetVolumetricNumSamples)) uint volumetricLightNumSamples;
	_declspec(property(get = GetVolumetricIntensity, put = SetVolumetricIntensity)) float volumetricLightIntensity;
	_declspec(property(get = GetVolumetricPower, put = SetVolumetricPower)) float volumetricLightPower;

protected:

	virtual bool IsWorking() const override;

private:

	virtual inline void GetDepthes(DepthStencil** out_depthes) const override { memcpy(out_depthes, m_depthes, sizeof(m_depthes)); }
	virtual void ClearDepthes(Com<ID3D11DeviceContext> deviceContext) override;
	void DeleteDepthes();
	void ResizeDepthes(uint size);

private:

	float					m_intensity = 1.0f;

	Color					m_diffuse = Color::white();
	Color					m_ambient = Color::black();

	bool					m_drawShadow = false;
	float					m_shadowWhiteness = 0.2f;
	float					m_shadowBias = 0.0001f;
	float					m_shadowFadeDistance = 50.0f;
	float					m_shadowFadeBeginPercent = 0.8f;

	bool					m_volumetricLightState = false;
	uint					m_volumetricNumSamples = 50;
	float					m_volumetricIntensity = 1.5f;
	float					m_volumetricPower = 1.5f;

	uint					m_depthSize = 2048;
	DepthStencil*			m_depthes[6] = {};
};

ENGINE_END
