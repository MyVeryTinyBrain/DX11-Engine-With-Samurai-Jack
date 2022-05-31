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

    assert(cct0 && cct1); // ĳ���� ��Ʈ�ѷ��� �������� �ʽ��ϴ�
    if (!cct0 || !cct1)
        return false;

    // �� �� �ϳ��� CCT �浹�� ���������� ���� �浹���� �ʽ��ϴ�.
    if (!cct0->collisionWithCCT || !cct1->collisionWithCCT)
        return false;

    Collider* c0 = cct0->capsuleCollider;
    Collider* c1 = cct1->capsuleCollider;
    Rigidbody* r0 = cct0->rigidbody;
    Rigidbody* r1 = cct1->rigidbody;

    assert(c0 && r0); // ĳ���� ��Ʈ�ѷ��� �ݶ��̴� Ȥ�� ������ٵ� �����ϴ�.
    if (!c0 || !r0)
        return false;

    // ������Ʈ�� �ϳ��� ��Ȱ��ȭ ���¶�� �浹���� �ʽ��ϴ�.
    if (!r0->active || !c0->active ||
        !r1->active || !c1->active)
        return false;

    // ĳ���� ��Ʈ�ѷ��� �ݶ��̴��� Ʈ���Ŷ�� �浹���� �ʽ��ϴ�.
    if (c0->IsTrigger() || c1->IsTrigger())
        return false;

    ICollider* ic0 = c0;
    PxFilterData filterData0 = ic0->GetPxShape()->getSimulationFilterData();
    ICollider* ic1 = c1;
    PxFilterData filterData1 = ic1->GetPxShape()->getSimulationFilterData();

    // �� �ݶ��̴��� ���̾� ��Ʈ�Դϴ�.
    PxU32 layer0 = filterData0.word0;
    PxU32 layer1 = filterData1.word0;

    // �� �ݶ��̴��� �浹 ������ ��Ʈ���Դϴ�.
    PxU32 collisionable0 = filterData0.word1 & layerManager->GetCollisionBits(c0->GetLayerIndex());
    PxU32 collisionable1 = filterData1.word1 & layerManager->GetCollisionBits(c1->GetLayerIndex());

    // �� �ݶ��̴��� �浹 ���ɼ��Դϴ�.
    PxU32 collision = (layer0 & collisionable1) && (layer1 & collisionable0);

    if (!collision)
    {
        return false;
    }

    return true;
}
