#include "EnginePCH.h"
#include "PhysicsFilterShaderCallback.h"
#include "PhysicsLayerManager.h"
#include "Collider.h"

PhysicsFilterShaderCallback::PhysicsFilterShaderCallback(PhysicsLayerManager* physicsLayerManager) :
	m_physicsLayerManager(physicsLayerManager)
{
}

PhysicsFilterShaderCallback::~PhysicsFilterShaderCallback()
{
}

PxFilterFlags PhysicsFilterShaderCallback::pairFound(
	PxU32 pairID, 
	PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0, 
	PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1, 
	PxPairFlags& pairFlags)
{
	// Ʈ���Ÿ� ������ �浹���� ó���մϴ�.

	Collider* c0 = (Collider*)s0->userData;
	Collider* c1 = (Collider*)s1->userData;
	assert(c0 != nullptr); // �ݶ��̴��� �����ϴ�.
	assert(c1 != nullptr); // �ݶ��̴��� �����ϴ�.
	
	// �� �ݶ��̴��� ���̾� ��Ʈ�Դϴ�.
	PxU32 layer0 = filterData0.word0;
	PxU32 layer1 = filterData1.word0;
	// �� �ݶ��̴��� �浹 ������ ��Ʈ���Դϴ�.
	PxU32 collisionable0 = filterData0.word1 & m_physicsLayerManager->GetCollisionBits(c0->GetLayerIndex());
	PxU32 collisionable1 = filterData1.word1 & m_physicsLayerManager->GetCollisionBits(c1->GetLayerIndex());
	// �� �ݶ��̴��� �浹 ���ɼ��Դϴ�.
	PxU32 collision = (layer0 & collisionable1) && (layer1 & collisionable0);

	if (!collision)
	{
		return PxFilterFlag::eSUPPRESS;
	}
	pairFlags = eContactFlags;
	return PxFilterFlag::eDEFAULT;
}

PxFilterFlags PhysicsFilterShaderCallback::PxSimulationFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
	PxFilterObjectAttributes attributes1, PxFilterData filterData1, 
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		// Ʈ���Ÿ� ó������ �ʽ��ϴ�.
		return PxFilterFlag::eSUPPRESS;
	}

	// pairFound �� ȣ���ϴ� �÷����Դϴ�.
	return PxFilterFlag::eCALLBACK;
}
