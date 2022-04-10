#pragma once

#include "PhysicsQueryType.h"

ENGINE_BEGIN

class Rigidbody;

struct PhysicsQueryFilterCallbackDesc
{
	PhysicsQueryType	Type;
	PxU32				LayerBitmask;
	bool				QueryOnce;
	Rigidbody*			IgnoreRigidbody;
};

class PhysicsQueryFilterCallback : public PxQueryFilterCallback
{
public:

	PhysicsQueryFilterCallback(const PhysicsQueryFilterCallbackDesc& desc);

private:

	virtual PxQueryHitType::Enum preFilter(
		const PxFilterData& filterData,
		const PxShape* shape,
		const PxRigidActor* actor,
		PxHitFlags& queryFlags
	) override;

	virtual PxQueryHitType::Enum postFilter(
		const PxFilterData& filterData,
		const PxQueryHit& hit
	) override;

private:

	PxU32					m_layerBitmask;
	PhysicsQueryType		m_queryType;
	PxQueryHitType::Enum	m_hitType;
	Rigidbody*				m_ignoreRigidbody = nullptr;
};

ENGINE_END
