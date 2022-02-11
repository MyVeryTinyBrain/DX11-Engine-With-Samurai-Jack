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
	// 트리거를 제외한 충돌만을 처리합니다.

	Collider* c0 = (Collider*)s0->userData;
	Collider* c1 = (Collider*)s1->userData;

	// 콜라이더가 없습니다.
	assert(c0 != nullptr);
	assert(c1 != nullptr);
	
	// 각 콜라이더의 레이어 비트입니다.
	PxU32 layer0 = filterData0.word0;
	PxU32 layer1 = filterData1.word0;

	// 각 콜라이더의 충돌 가능한 비트들입니다.
	PxU32 collisionable0 = filterData0.word1 & m_physicsLayerManager->GetCollisionBits(c0->GetLayerIndex());
	PxU32 collisionable1 = filterData1.word1 & m_physicsLayerManager->GetCollisionBits(c1->GetLayerIndex());

	// 두 콜라이더의 충돌 가능성입니다.
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
		// 트리거를 처리하지 않습니다.
		return PxFilterFlag::eSUPPRESS;
	}

	// pairFound 를 호출하는 플래그입니다.
	return PxFilterFlag::eCALLBACK;
}
