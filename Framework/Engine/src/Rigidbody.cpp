#include "EnginePCH.h"
#include "Rigidbody.h"
#include "System.h"
#include "PhysicsSystem.h"
#include "GameObject.h"
#include "Collider.h"
#include "RigidbodyInterpolate.h"
#include "RigidbodyExtrapolate.h"

void Rigidbody::Awake()
{
	IPhysicsSystem* iPhysicsSystem = system->physics;
	PxPhysics* physics = iPhysicsSystem->GetPhysics();
	PxScene* pxScene = iPhysicsSystem->GetScene();

	PxTransform pose(PxIdentity);
	m_body = physics->createRigidDynamic(pose);
	m_body->setMass(1.0f);
	m_body->userData = this;
	pxScene->addActor(*m_body);

	ApplyFlags();
	AttachAll();
	ApplyBodyTransformFromGameObject();

	m_interpolater = new RigidbodyInterpolate(system->time, this);
	m_extrapolater = new RigidbodyExtrapolate(system->time, this);

	iPhysicsSystem->RegistPhysicsObject(this);
}

void Rigidbody::Awake(void* arg)
{
	IPhysicsSystem* iPhysicsSystem = system->physics;
	PxPhysics* physics = iPhysicsSystem->GetPhysics();

	m_body = (PxRigidDynamic*)arg;
	assert(m_body != nullptr); // PxRigidDynamic�� �������� �ʽ��ϴ�.

	m_body->userData = this;

	m_isCCTComponent = true;

	ApplyFlags();
	AttachAll();
	ApplyBodyTransformFromGameObject();

	m_interpolater = new RigidbodyInterpolate(system->time, this);
	m_extrapolater = new RigidbodyExtrapolate(system->time, this);

	iPhysicsSystem->RegistPhysicsObject(this);
}

void Rigidbody::Start()
{
}

void Rigidbody::BeforePhysicsSimulation()
{
	if (m_currentInterpolate)
	{
		m_currentInterpolate->RollbackPose();
	}

	ApplyBodyTransformFromGameObject();
}

void Rigidbody::AfterPhysicsSimulation()
{
	ApplyGameObjectTransfromFromBody();

	if (m_currentInterpolate)
	{
		m_currentInterpolate->BackupPose();
	}

	//if (m_currentInterpolate)
	//{
	//	m_currentInterpolate->InterpolatePose();
	//}
}

void Rigidbody::PostFixedUpdate()
{
	//if (m_currentInterpolate)
	//{
	//	m_currentInterpolate->CheckPoseChange();
	//}
}

void Rigidbody::PreUpdate()
{
	if (m_currentInterpolate)
	{
		m_currentInterpolate->InterpolatePose();
	}
}

void Rigidbody::PostUpdate()
{
	if (m_currentInterpolate)
	{
		m_currentInterpolate->CheckPoseChange();
	}
}

void Rigidbody::OnEnable()
{
	m_body->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	m_body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	SetVelocity(V3::zero());
	ClearForce(Rigidbody::ForceMode::Acceleration);
	ClearForce(Rigidbody::ForceMode::Force);
	ClearForce(Rigidbody::ForceMode::Impulse);
	ClearForce(Rigidbody::ForceMode::Velocity);
	WakeUp();
}

void Rigidbody::OnDisable()
{
	m_body->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	m_body->setActorFlag(PxActorFlag::eVISUALIZATION, false);
}

void Rigidbody::OnDestroyed()
{
	IPhysicsSystem* iPhysicsSystem = system->physics;
	iPhysicsSystem->UnregistPhysicsObject(this);

	if (m_isCCTComponent)
		return;

	DetachAll();
}

Rigidbody::~Rigidbody()
{
	m_currentInterpolate = nullptr;
	SafeDelete(m_interpolater);
	SafeDelete(m_extrapolater);

	if (m_isCCTComponent)
		return;

	IPhysicsSystem* iPhysicsSystem = system->physics;
	PxScene* pxScene = iPhysicsSystem->GetScene();
	pxScene->removeActor(*m_body);

	PxRelease(m_body);
}

void Rigidbody::SetContinousDetctionMode(bool value)
{
	m_continous = value;
	ApplyFlags();
}

bool Rigidbody::IsLockedRotate(Rigidbody::Axis axes)
{
	PxU32 flag = (PxU32)axes << 3;
	PxU32 flags = (PxU32)m_body->getRigidDynamicLockFlags();
	return flag & flags;
}

void Rigidbody::SetRotationLock(Rigidbody::Axis axes, bool value)
{
	PxU32 flag = (PxU32)axes << 3;
	m_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum(flag), value);
}

bool Rigidbody::IsLockedTranslate(Rigidbody::Axis axes)
{
	PxU32 flag = (PxU32)axes << 0;
	PxU32 flags = (PxU32)m_body->getRigidDynamicLockFlags();
	return flag & flags;
}

void Rigidbody::SetTranslationLock(Rigidbody::Axis axes, bool value)
{
	PxU32 flag = (PxU32)axes << 0;
	m_body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum(flag), value);
}

Rigidbody::Interpolate Rigidbody::GetInterpolateMode() const
{
	if (!m_currentInterpolate)
	{
		return Rigidbody::Interpolate::None;
	}
	else if (m_currentInterpolate == m_interpolater)
	{
		return Rigidbody::Interpolate::Interpolate;
	}
	else if (m_currentInterpolate == m_extrapolater)
	{
		return Rigidbody::Interpolate::Extrapolate;
	}

	return Rigidbody::Interpolate::None;
}

void Rigidbody::SetInterpolateMode(Rigidbody::Interpolate mode)
{
	if (mode == GetInterpolateMode())
	{
		return;
	}

	switch (mode)
	{
		case Rigidbody::Interpolate::Interpolate:
			m_currentInterpolate = m_interpolater;
			m_currentInterpolate->OnEnable();
			break;
		case Rigidbody::Interpolate::Extrapolate:
			m_currentInterpolate = m_extrapolater;
			m_currentInterpolate->OnEnable();
			break;
		default:
			m_currentInterpolate = nullptr;
			break;
	}
}

V3 Rigidbody::GetVelocity() const
{
	PxVec3 pxVelocity = m_body->getLinearVelocity();
	return FromPxVec3(pxVelocity);
}

void Rigidbody::SetVelocity(const V3& velocity)
{
	if (IsKinematic())
	{
		return;
	}

	PxVec3 pxVelocity = ToPxVec3(velocity);
	m_body->setLinearVelocity(pxVelocity);
}

V3 Rigidbody::GetAngularVelocity() const
{
	PxVec3 pxAVelocity = m_body->getAngularVelocity();
	return FromPxVec3(pxAVelocity) * Rad2Deg;
}

void Rigidbody::SetAngularVelocity(const V3& angularVelocity)
{
	PxVec3 pxAngularVelocity = ToPxVec3(angularVelocity);
	m_body->setAngularVelocity(pxAngularVelocity * Deg2Rad);
}

void Rigidbody::AddForce(const V3& force, Rigidbody::ForceMode forceMode)
{
	if (IsKinematic())
	{
		return;
	}

	PxVec3 pxForce = ToPxVec3(force);
	m_body->addForce(pxForce, (PxForceMode::Enum)forceMode);
}

void Rigidbody::ClearForce(Rigidbody::ForceMode forceMode)
{
	if (IsKinematic())
	{
		return;
	}

	m_body->clearForce((PxForceMode::Enum)forceMode);
}

void Rigidbody::AttachAll()
{
	DetachAll();

	std::vector<Collider*> colliders = gameObject->GetComponentsInChilds<Collider>();
	for (auto collider : colliders)
	{
		ICollider* iCollider = collider;
		PxShape* pxShape = iCollider->GetPxShape();
		if (pxShape)
		{
			if (!collider)
				continue;

			if (collider && collider->IsCCTComponent())
				continue;

			m_body->attachShape(*pxShape);
			PxRigidBodyExt::updateMassAndInertia(*m_body, m_body->getMass());
		}
	}
}

void Rigidbody::DetachAll()
{
	PxU32 nb = m_body->getNbShapes();
	PxShape** pxShapes = new PxShape * [nb];
	m_body->getShapes(pxShapes, sizeof(PxShape*) * nb);
	
	for (PxU32 i = 0; i < nb; ++i)
	{
		Collider* collider = (Collider*)pxShapes[i]->userData;

		if (!collider)
			continue;

		if (collider && collider->IsCCTComponent())
			continue;

		m_body->detachShape(*pxShapes[i]);
	}

	SafeDeleteArray(pxShapes);
}

void Rigidbody::ApplyFlags()
{	
	// ���� �浹 ���� ��带 �����մϴ�.
	bool continousFlag = m_continous && !IsKinematic();
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, continousFlag);

	// ���� �浹 ��� ���� ������ ����մϴ�.
	m_body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD_FRICTION, continousFlag);

	// A -- B ���� �浹 �˸��� ���� �����մϴ�.
	// eFORCE_KINE_KINE_NOTIFICATIONS
	// eFORCE_STATIC_KINE_NOTIFICATIONS 
	// m_body->setRigidBodyFlag(PxRigidBodyFlag::eFORCE_KINE_KINE_NOTIFICATIONS, true);
	// m_body->setRigidBodyFlag(PxRigidBodyFlag::eFORCE_STATIC_KINE_NOTIFICATIONS, true);

	// �ùķ��̼� �߿� Kinemtic Rigidbody�� �����Ҽ� �ְ� �˴ϴ�.
	// �� �����ӿ�ũ�� �ùķ��̼��� ���� �Ŀ� �����ϰ� �˴ϴ�.
	// m_body->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);

	// �ùķ��̼ǿ� ���� Pose ��ȭ�� �����մϴ�.
	// PxSimulationEventCallback::onAdvance() �Լ��� ȣ���մϴ�.
	// m_body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW, true);
}

void Rigidbody::ApplyBodyTransformFromGameObject()
{
	PxTransform pose = m_body->getGlobalPose();

	bool hasChanged = false;

	V3 deltaPos = transform->position - FromPxVec3(pose.p);

	if (deltaPos.magnitude > Epsilon)
	{
		hasChanged = true;
		pose.p = ToPxVec3(transform->position);
	}

	if (Q::Radian(FromPxQuat(pose.q), transform->rotation) > Epsilon)
	{
		hasChanged = true;
		pose.q = ToPxQuat(transform->rotation);
	}

	if (hasChanged)
	{
		m_body->setGlobalPose(pose);
	}
}

void Rigidbody::ApplyGameObjectTransfromFromBody()
{
	PxTransform pose = m_body->getGlobalPose();

	transform->position = FromPxVec3(pose.p);

	if (!m_isCCTComponent)
	{
		// PhsX::Controller�� Simulation ���Ŀ� ������ ����˴ϴ�.
		// �� ������ �������� �ʱ� �����Դϴ�.
		transform->rotation = FromPxQuat(pose.q);
	}
}

bool Rigidbody::IsWorking() const
{
	return active;
}
