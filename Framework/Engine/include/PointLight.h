#pragma once

#include "Light.h"

ENGINE_BEGIN

class ENGINE_API PointLight : public Light
{
	virtual void Awake() override;

public:

	inline float GetRange() const { return m_range; }
	void SetRange(float value) { m_range = Saturate(value); }

	_declspec(property(get = GetRange, put = SetRange)) float range;

public:

	virtual LightType GetLightType() const override { return LightType::Point; }

	virtual uint GetProjectionCount() const override { return 6; }

	virtual void GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const override;

	virtual LightDesc GetLightDesc(ICamera* camera) const override;

	virtual bool ContainsInCamera(ICamera* camera) const override;

private:

	virtual VolumetricDesc GetVolumetricDesc() const override;

private:

	virtual bool IsWorking() const override;

	void SetupRotations();

private:

	float		m_range = 5.0f;

	// For Shadow

	const V3	m_arrDirection[6] = { V3::up(),V3::left(),V3::forward(),V3::right(),V3::back(),V3::down(), };
	const V3	m_arrUp[6] =		{ V3::forward(),V3::up(),V3::up(),V3::up(),V3::up(),V3::forward(), };
	Q			m_arrRotations[6] = {};

	float		m_near = 0.1f;
};

ENGINE_END
