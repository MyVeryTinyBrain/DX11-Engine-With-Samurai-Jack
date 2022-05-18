#include "EnginePCH.h"
#include "CCTCallback.h"
#include "Collider.h"
#include "Transform.h"
#include "CharacterController.h"
#include "PhysicsSimulationCallbackType.h"
#include "CapsuleCollider.h"
#include "System.h"
#include "PhysicsSystem.h"
#include "PhysicsLayerManager.h"
#include "Rigidbody.h"

void CCTCallback::Initialize(CharacterController* characterController)
{
    m_characterController = characterController;
}

void CCTCallback::onShapeHit(const PxControllerShapeHit& hit)
{
    ICharacterController* iCharacterController = m_characterController;
    
    iCharacterController->OnHitShape(hit);
    CCTInteraction(hit);

    V3 up = m_characterController->upDirection;
    V3 down = -up;
    V3 normal = FromPxVec3(hit.worldNormal);

    float c0 = V3::Dot(normal, down);
    float c1 = m_characterController->slopeLimit;
    if (Abs(c0) > Abs(c1))
    {
        // SlopeLimit보다 작은 각도차이이면 땅에 부딪힌 것으로 인식합니다.
        iCharacterController->OnGroundHit(FromPxVec3(hit.worldNormal));
    }
}

void CCTCallback::onControllerHit(const PxControllersHit& hit)
{
}

void CCTCallback::onObstacleHit(const PxControllerObstacleHit& hit)
{
}

PxControllerBehaviorFlags CCTCallback::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
    ICharacterController* iCharacterController = m_characterController;

    V3 up = m_characterController->upDirection;
    V3 down = -up;
    float contactOffset = m_characterController->contactOffset;
    V3 foot = m_characterController->footPosition + down * contactOffset;

    PxGeometryHolder cctGeometry = m_characterController->capsuleCollider->pxGeometry;
    Collider* cctCollider = (Collider*)shape.userData;

    PxGeometryHolder targetGeometry = shape.getGeometry();
    Collider* targetCollider = (Collider*)shape.userData;

    if (!cctCollider || !targetCollider)
    {
        return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
    }

    PxTransform cctPose, targetPose;
    V3 cctPos, targetPos;
    Q cctQ, targetQ;
    cctPos = cctCollider->transform->position;
    targetPos = targetCollider->transform->position;
    cctQ = cctCollider->transform->rotation;
    targetQ = targetCollider->transform->rotation;
    cctPose.p = ToPxVec3(targetPos);
    targetPose.p = ToPxVec3(targetPos);
    cctPose.q = ToPxQuat(cctQ);
    targetPose.q = ToPxQuat(targetQ);

    PxRaycastHit hit;
    if (PxGeometryQuery::raycast(ToPxVec3(foot), ToPxVec3(down), targetGeometry.any(), targetPose, contactOffset + 1.0f, PxHitFlag::eDEFAULT, 1, &hit))
    {
        V3 worldNormal = targetQ.MultiplyVector(FromPxVec3(hit.normal));
        V3 worldDown = FromPxVec3(down);
        float c0 = V3::Dot(worldNormal, worldDown);
        float c1 = m_characterController->slopeLimit;
        if (Abs(c0) < Abs(c1))
        {
            // SlopeLimit보다 큰 각도차이이면 미끄러짐을 활성화합니다.
            return PxControllerBehaviorFlag::eCCT_SLIDE | PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
        }
        else
        {
            return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
        }
    }
    else
    {
        return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
    }

    return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

PxControllerBehaviorFlags CCTCallback::getBehaviorFlags(const PxController& controller)
{
    return PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxControllerBehaviorFlags CCTCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
    return PxControllerBehaviorFlag::eCCT_SLIDE | PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

PxQueryHitType::Enum CCTCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
    Collider* c0 = m_characterController->capsuleCollider;
    Collider* c1 = (Collider*)shape->userData;
    Rigidbody* r0 = m_characterController->rigidbody;
    Rigidbody* r1 = c1->rigidbody;

    assert(c0 && r0); // 캐릭터 컨트롤러의 콜라이더 혹은 리지드바디가 없습니다.
    if(!c0 || !r0)
        return PxQueryHitType::eNONE;

    // 캐릭터 컨트롤러가 비활성화 상태라면 충돌하지 않습니다.
    if(!m_characterController->active)
        return PxQueryHitType::eNONE;

    // 컴포넌트가 하나라도 비활성화 상태라면 충돌하지 않습니다.
    if (!r0->active || !c0->active ||
        !r1->active || !c1->active)
        return PxQueryHitType::eNONE;

    // 캐릭터 컨트롤러의 콜라이더가 트리거라면 충돌하지 않습니다.
    if (c0->IsTrigger())
        return PxQueryHitType::eNONE;

    ICollider* ic0 = c0;
    PxFilterData filterData0 = ic0->GetPxShape()->getSimulationFilterData();
    ICollider* ic1 = c1;
    PxFilterData filterData1 = ic1->GetPxShape()->getSimulationFilterData();
    
    // 각 콜라이더의 레이어 비트입니다.
    PxU32 layer0 = filterData0.word0;
    PxU32 layer1 = filterData1.word0;
    
    // 각 콜라이더의 충돌 가능한 비트들입니다.
    PxU32 collisionable0 = filterData0.word1 & m_characterController->system->physics->layerManager->GetCollisionBits(c0->GetLayerIndex());
    PxU32 collisionable1 = filterData1.word1 & m_characterController->system->physics->layerManager->GetCollisionBits(c1->GetLayerIndex());

    // 두 콜라이더의 충돌 가능성입니다.
    PxU32 collision = (layer0 & collisionable1) && (layer1 & collisionable0);

    if (!collision)
    {
        return PxQueryHitType::eNONE;
    }

    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum CCTCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
    return PxQueryHitType::eBLOCK;
}

bool CCTCallback::filter(const PxController& a, const PxController& b)
{
    CharacterController* cct0 = (CharacterController*)a.getUserData();
    CharacterController* cct1 = (CharacterController*)b.getUserData();

    assert(cct0 && cct1); // 캐릭터 컨트롤러가 존재하지 않습니다
    if (!cct0 || !cct1)
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
    if (c0->IsTrigger())
        return false;

    ICollider* ic0 = c0;
    PxFilterData filterData0 = ic0->GetPxShape()->getSimulationFilterData();
    ICollider* ic1 = c1;
    PxFilterData filterData1 = ic1->GetPxShape()->getSimulationFilterData();

    // 각 콜라이더의 레이어 비트입니다.
    PxU32 layer0 = filterData0.word0;
    PxU32 layer1 = filterData1.word0;

    // 각 콜라이더의 충돌 가능한 비트들입니다.
    PxU32 collisionable0 = filterData0.word1 & m_characterController->system->physics->layerManager->GetCollisionBits(c0->GetLayerIndex());
    PxU32 collisionable1 = filterData1.word1 & m_characterController->system->physics->layerManager->GetCollisionBits(c1->GetLayerIndex());

    // 두 콜라이더의 충돌 가능성입니다.
    PxU32 collision = (layer0 & collisionable1) && (layer1 & collisionable0);

    if (!collision)
    {
        return false;
    }

    return true;
}

void CCTCallback::CCTInteraction(const PxControllerShapeHit& hit)
{
    PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();

    if (actor)
    {
        if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
            return;

        if (0)
        {
            const PxVec3 p = actor->getGlobalPose().p + hit.dir * 10.0f;

            PxShape* shape;
            actor->getShapes(&shape, 1);
            PxRaycastHit newHit;
            PxU32 n = PxShapeExt::raycast(*shape, *shape->getActor(), p, -hit.dir, 20.0f, PxHitFlag::ePOSITION, 1, &newHit);
            if (n)
            {
                // We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
                // useless stress on the solver. It would be possible to enable/disable vertical pushes on
                // particular objects, if the gameplay requires it.
                const PxVec3 upVector = hit.controller->getUpDirection();
                const PxF32 dp = hit.dir.dot(upVector);
                //		shdfnd::printFormatted("%f\n", fabsf(dp));
                if (fabsf(dp) < 1e-3f)
                    //		if(hit.dir.y==0.0f)
                {
                    const PxTransform globalPose = actor->getGlobalPose();
                    const PxVec3 localPos = globalPose.transformInv(newHit.position);
                    AddForceAtLocalPos(*actor, hit.dir * hit.length * 1000.0f, localPos, PxForceMode::eACCELERATION);
                }
            }
        }

        // We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
        // useless stress on the solver. It would be possible to enable/disable vertical pushes on
        // particular objects, if the gameplay requires it.
        const PxVec3 upVector = hit.controller->getUpDirection();
        const PxF32 dp = hit.dir.dot(upVector);
        //		shdfnd::printFormatted("%f\n", fabsf(dp));
        if (fabsf(dp) < 1e-3f)
            //		if(hit.dir.y==0.0f)
        {
            const PxTransform globalPose = actor->getGlobalPose();
            const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
            AddForceAtLocalPos(*actor, hit.dir * hit.length * 1000.0f, localPos, PxForceMode::eACCELERATION);
        }
    }
}

void CCTCallback::AddForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup)
{	
    //transform pos to world space
    const PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

    AddForceAtPosInternal(body, force, globalForcePos, mode, wakeup);
}

void CCTCallback::AddForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup)
{
    /*	if(mode == PxForceMode::eACCELERATION || mode == PxForceMode::eVELOCITY_CHANGE)
    {
        Ps::getFoundation().error(PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__,
            "PxRigidBodyExt::addForce methods do not support eACCELERATION or eVELOCITY_CHANGE modes");
        return;
    }*/

    const PxTransform globalPose = body.getGlobalPose();
    const PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

    const PxVec3 torque = (pos - centerOfMass).cross(force);
    body.addForce(force, mode, wakeup);
    body.addTorque(torque, mode, wakeup);
}
