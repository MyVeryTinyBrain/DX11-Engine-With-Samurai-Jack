#pragma once

#include "Light.h"

ENGINE_BEGIN

class ENGINE_API SpotLight : public Light
{
	virtual void Awake() override;

public:

	inline float GetRange() const { return m_range; }
	void SetRange(float value) { m_range = Saturate(value); }

	inline float GetAngle() const { return m_angle; }
	void SetAngle(float value) { m_angle = Clamp(value, 0.0f, 90.0f); }

	_declspec(property(get = GetRange, put = SetRange)) float range;
	_declspec(property(get = GetAngle, put = SetAngle)) float angle;

public:

	virtual LightType GetLightType() const override { return LightType::Spot; }

	virtual uint GetProjectionCount() const override { return 1; }

	virtual void GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const override;

	virtual LightDesc GetLightDesc(ICamera* camera) const override;

	virtual bool ContainsInCamera(ICamera* camera) const override;

private:

	virtual bool IsWorking() const override;

private:

	float	m_range = 10.0f;
	float	m_angle = 45.0f;

	// For Shadow

	float	m_near = 0.1f;
};

ENGINE_END
