#include "EnginePCH.h"
#include "CCTFilterCallback.h"
#include "CapsuleCollider.h"
#include "Rigidbody.h"
#include "CharacterController.h"
#include "PhysicsLayerManager.h"

bool CCTFilterCallback::static_cct_filtercallback(PhysicsLayerManager* layerManager, const PxController& a, const PxController& b)
{
    CharacterController* cct0 = (CharacterController*)a.getUserData();
    CharacterController* cct1 = (CharacterController*)b.getUserData();

    assert(cct0 && cct1); // 캐릭터 컨트롤러가 존재하지 않습니다
    if (!cct0 || !cct1)
        return false;

    // 둘 중 하나라도 CCT 충돌이 꺼져있으면 서로 충돌하지 않습니다.
    if (!cct0->collisionWithCCT || !cct1->collisionWithCCT)
        return false;

    Collider* c0 = cct0->capsuleCollider;
    Collider* c1 = cct1->capsuleCollider;
    Rigidbody* r0 = cct0->rigidbody;
    Rigidbody* r1 = cct1->rigidbody;

    assert(c0 && r0); // 캐릭터 컨트롤러의 콜라이더 혹은 리지드바디가 없습니다.
    if (!c0 || !r0)
        return false;

    // 컴포넌트가 하나라도 비활성화 상태라면 충돌하지 않습니다.
    if (!r0->active || !c0->active ||
        !r1->active || !c1->active)
        return false;

    // 캐릭터 컨트롤러의 콜라이더가 트리거라면 충돌하지 않습니다.
    if (c0->IsTrigger() || c1->IsTrigger())
        return false;

    ICollider* ic0 = c0;
    PxFilterData filterData0 = ic0->GetPxShape()->getSimulationFilterData();
    ICollider* ic1 = c1;
    PxFilterData filterData1 = ic1->GetPxShape()->getSimulationFilterData();

    // 각 콜라이더의 레이어 비트입니다.
    PxU32 layer0 = filterData0.word0;
    PxU32 layer1 = filterData1.word0;

    // 각 콜라이더의 충돌 가능한 비트들입니다.
    PxU32 collisionable0 = filterData0.word1 & layerManager->GetCollisionBits(c0->GetLayerIndex());
    PxU32 collisionable1 = filterData1.word1 & layerManager->GetCollisionBits(c1->GetLayerIndex());

    // 두 콜라이더의 충돌 가능성입니다.
    PxU32 collision = (layer0 & collisionable1) && (layer1 & collisionable0);

    if (!collision)
    {
        return false;
    }

    return true;
}
