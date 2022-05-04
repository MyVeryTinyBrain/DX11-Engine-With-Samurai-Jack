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

	V3 CalculateViewPosition(ICamera* camera, float camNearPercent, float camFarPercent) const;

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

	float	m_nearPercent[3] = { 0.0f, 0.008f, 0.04f };
	float	m_farPercent[3] = { 0.008f, 0.04f, 0.2f };

	//float	m_camNear[3] = { 0.0f, 0.0f, 0.0f };
	//float	m_camFar[3] = { 2.0f, 10.0f, 50.0f };
};

ENGINE_END
