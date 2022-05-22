#pragma once

#include "Light.h"

ENGINE_BEGIN
class ICamera;
class ENGINE_API DirectionalLight : public Light
{
	virtual void Awake() override;

public:

	inline float GetOrthoWidth() const { return m_orthoWidth; }
	inline float GetOrthoHeight() const { return m_orthoHeight; }

	_declspec(property(get = GetOrthoWidth)) float orthoWidth;
	_declspec(property(get = GetOrthoHeight)) float orthoHeight;

public:

	V3 CalculateViewPosition(ICamera* camera, float camNear, float camFar) const;

	virtual LightType GetLightType() const override { return LightType::Directional; }

	virtual uint GetProjectionCount() const override { return 3; }

	virtual void GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const override;

	virtual LightDesc GetLightDesc(ICamera* camera) const override;

	virtual bool ContainsInCamera(ICamera* camera) const override;
	virtual FRect GetDeferredScreenQuad(ICamera* camera) const override;

private:

	virtual VolumetricDesc GetVolumetricDesc() const override;

private:

	// For Shadow

	float	m_orthoWidth = 20;
	float	m_orthoHeight = 20;
	float	m_near = 0.1f;
	float	m_far = 500;

	float	m_camNear[3] = { 0.0f, 3.0f, 6.0f };
	float	m_camFar[3] = { 3.01f, 6.01f, 30.01f };
};

ENGINE_END
