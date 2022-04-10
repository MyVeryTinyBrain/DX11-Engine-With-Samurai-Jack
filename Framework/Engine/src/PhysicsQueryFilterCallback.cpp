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

	// �� ��ü�� ������ ��ü��� �����մϴ�.
	if (rigidbody == m_ignoreRigidbody)
		return PxQueryHitType::eNONE;

	// Ʈ���� �浹�� ��Ȱ���϶�, �� �ݶ��̴��� Ʈ���Ŷ�� �����մϴ�.
	if (!allowTrigger && collider->IsTrigger())
		return PxQueryHitType::eNONE;

	// �ݶ��̴� �浹�� ��Ȱ���϶�, �� �ݶ��̴��� Ʈ���Ű� �ƴ϶�� �����մϴ�.
	if (!allowCollider && !collider->IsTrigger())
		return PxQueryHitType::eNONE;

	// �� �ݶ��̴��� ���̾ ��� ���̾ �ƴ϶�� �����մϴ�.
	if (!isValidHit)
		return PxQueryHitType::eNONE;

	return m_hitType;
}

PxQueryHitType::Enum PhysicsQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	return PxQueryHitType::eTOUCH;
}
