#include "EnginePCH.h"
#include "PhysicsQueryFilterCallback.h"
#include "Collider.h"
#include "Rigidbody.h"

PhysicsQueryFilterCallback::PhysicsQueryFilterCallback(const PhysicsQueryFilterCallbackDesc& desc)
{
	m_layerBitmask = desc.LayerBitmask;
	m_queryType = desc.Type;
	m_hitType = desc.QueryOnce ? PxQueryHitType::eBLOCK : PxQueryHitType::eTOUCH;
	m_ignoreRigidbody = desc.IgnoreRigidbody;
}

PxQueryHitType::Enum PhysicsQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	Collider* collider = (Collider*)shape->userData;
	Rigidbody* rigidbody = (Rigidbody*)actor->userData;

	bool allowTrigger = uint(m_queryType) & uint(PhysicsQueryTypes::Trigger);
	bool allowCollider = uint(m_queryType) & uint(PhysicsQueryTypes::Collider);
	bool isValidHit = (1 << collider->GetLayerIndex()) & m_layerBitmask;

	// 이 강체가 무시할 강체라면 종료합니다.
	if (rigidbody == m_ignoreRigidbody)
		return PxQueryHitType::eNONE;

	// 트리거 충돌이 비활성일때, 이 콜라이더가 트리거라면 종료합니다.
	if (!allowTrigger && collider->IsTrigger())
		return PxQueryHitType::eNONE;

	// 콜라이더 충돌이 비활성일때, 이 콜라이더가 트리거가 아니라면 종료합니다.
	if (!allowCollider && !collider->IsTrigger())
		return PxQueryHitType::eNONE;

	// 이 콜라이더의 레이어가 대상 레이어가 아니라면 종료합니다.
	if (!isValidHit)
		return PxQueryHitType::eNONE;

	return m_hitType;
}

PxQueryHitType::Enum PhysicsQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	return PxQueryHitType::eTOUCH;
}
