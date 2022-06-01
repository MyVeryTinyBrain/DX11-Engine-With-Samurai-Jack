#include "EnginePCH.h"
#include "CharacterController.h"
#include "Transform.h"
#include "System.h"
#include "PhysicsSystem.h"
#include "GameObject.h"
#include "Rigidbody.h"
#include "CapsuleCollider.h"
#include "CCTCallback.h"
#include "CCTInterpolate.h"
#include "CCTExtrapolate.h"
#include "Times.h"
#include "PhysicsQuery.h"

void CharacterController::Awake()
{
	IPhysicsSystem* iPhysicsSystem = system->physics;
	PxPhysics* physics = iPhysicsSystem->GetPhysics();
	iPhysicsSystem->RegistPhysicsObject(this);

	m_callback = new CCTCallback;

	PxCapsuleControllerDesc desc = CreateInitDesc(physics);

	V3 position = transform->position;
	desc.position.x = position.x;
	desc.position.y = position.y;
	desc.position.z = position.z;

	V3 up = transform->up;
	desc.upDirection.x = up.x;
	desc.upDirection.y = up.y;
	desc.upDirection.z = up.z;

	desc.reportCallback = m_callback;
	desc.behaviorCallback = m_callback;
	desc.userData = this;

	m_controller = (PxCapsuleController*)iPhysicsSystem->GetControllerManager()->createController(desc);
	assert(m_controller != nullptr); // ĳ���� ��Ʈ�ѷ� ������ �����߽��ϴ�.

	m_actor = m_controller->getActor();
	m_shape = nullptr;
	m_actor->getShapes(&m_shape, 1, 0);

	m_callback->Initialize(this);

	m_filters.mFilterData = 0;
	m_filters.mFilterFlags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;
	m_filters.mFilterCallback = m_callback;
	m_filters.mCCTFilterCallback = m_callback;

	m_collider = gameObject->AddComponent<CapsuleCollider>(m_shape);
	m_rigidbody = gameObject->AddComponent<Rigidbody>(m_actor);

	m_interpolater = new CCTInterpolate(system->time, this);
	m_extrapolater = new CCTExtrapolate(system->time, this);

	SetInterpolateMode(CharacterController::Interpolate::Interpolate);
}

void CharacterController::BeforePhysicsSimulation()
{
	if (m_fixedUpdate && m_currentInterpolate)
	{
		m_currentInterpolate->RollbackPose();
	}

	if (m_fixedUpdate)
	{
		UpdateController(system->time->fixedDeltaTime);
	}
}

void CharacterController::AfterPhysicsSimulation()
{
	ApplyPositionFromCCT();

	if (m_fixedUpdate && m_currentInterpolate)
	{
		m_currentInterpolate->BackupPose();
	}

	if (m_fixedUpdate)
	{
		m_prevGroundHit = m_groundHit;
		if (m_groundHit)
		{
			m_groundHit = false;
		}
	}

	NotifyCollisions();
	ClearCollisions();
}

void CharacterController::BeforePhysicsSimulationOnce()
{
}

void CharacterController::AfterPhysicsSimulationOnce()
{
	//m_prevGroundHit = m_groundHit;
	//if (m_groundHit)
	//{
	//	m_groundHit = false;
	//}
}

void CharacterController::PreUpdate()
{
	if (m_fixedUpdate && m_currentInterpolate)
	{
		m_currentInterpolate->InterpolatePose();
	}

	if (!m_fixedUpdate)
	{
		UpdateController(system->time->deltaTime);
	}

	if (!m_fixedUpdate)
	{
		m_prevGroundHit = m_groundHit;
		if (m_groundHit)
		{
			m_groundHit = false;
		}
	}
}

void CharacterController::Update()
{
//#include "Input.h"
	//V3 dir;
	//if (system->input->GetKey(Key::Num4))
	//	dir.x += -1;
	//if (system->input->GetKey(Key::Num6))
	//	dir.x += +1;
	//if (system->input->GetKey(Key::Num8))
	//	dir.z += +1;
	//if (system->input->GetKey(Key::Num2))
	//	dir.z += -1;
	//if (system->input->GetKeyDown(Key::NumSubtract))
	//	m_velocity += V3::up() * 8.0f;
	//if (system->input->GetKey(Key::NumDivide))
	//	transform->localScale += V3::one() * 0.1f * system->time->deltaTime;
	//if (system->input->GetKey(Key::NumMultiply))
	//	transform->localScale -= V3::one() * 0.1f * system->time->deltaTime;

	//if (system->input->GetKeyDown(Key::Zero))
	//	transform->position += V3::forward();

	////Move(dir * 10.0f * system->time->deltaTime, system->time->deltaTime);
	//if (dir.magnitude != 0)
	//{
	//	//m_velocity.x = dir.x;
	//	//m_velocity.z = dir.z;
	//	MoveOnGround(dir * 5.0f * system->time->deltaTime, system->time->deltaTime);
	//}
}

void CharacterController::FixedUpdate()
{
}

void CharacterController::PostUpdate()
{
	if (m_fixedUpdate && m_currentInterpolate)
	{
		m_currentInterpolate->CheckPoseChange();
	}
}

void CharacterController::OnDestroyed()
{
	m_currentInterpolate = nullptr;
	SafeDelete(m_interpolater);
	SafeDelete(m_extrapolater);

	IPhysicsSystem* iPhysicsSystem = system->physics;
	iPhysicsSystem->UnregistPhysicsObject(this);

	if (m_rigidbody)
	{
		m_rigidbody->Destroy();
		m_rigidbody = nullptr;
	}
	if (m_collider)
	{
		m_collider->Destroy();
		m_collider = nullptr;
	}

	PxRelease(m_controller);

	SafeDelete(m_callback);
}

bool CharacterController::IsWorking() const
{
	return active;
}

Ref<Rigidbody> CharacterController::GetRigidbody() const
{
	return m_rigidbody;
}

Ref<CapsuleCollider> CharacterController::GetCapsuleCollider() const
{
	return m_collider;
}

float CharacterController::GetContactOffset() const
{
	return m_controller->getContactOffset();
}

void CharacterController::SetContactOffset(float value)
{
	m_controller->setContactOffset(value);
}

float CharacterController::GetRadius() const
{
	return m_collider->radius;
}

void CharacterController::SetRadius(float value)
{
	m_collider->radius = value;
}

float CharacterController::GetHeight() const
{
	return 2.0f * m_collider->halfHeight;
}

void CharacterController::SetHeight(float value)
{
	m_collider->halfHeight = value * 0.5f;
}

bool CharacterController::IsAutoClimbMode() const
{
	PxCapsuleClimbingMode::Enum mode = m_controller->getClimbingMode();
	return mode == PxCapsuleClimbingMode::eEASY;
}

void CharacterController::SetAutoClimbMode(bool value)
{
	if (value)
	{
		m_controller->setClimbingMode(PxCapsuleClimbingMode::eEASY);
	}
	else
	{
		m_controller->setClimbingMode(PxCapsuleClimbingMode::eCONSTRAINED);
	}
}

float CharacterController::GetStepOffset() const
{
	return m_controller->getStepOffset();
}

void CharacterController::SetStepOffset(float value)
{
	m_controller->setStepOffset(value);
}

float CharacterController::GetSlopeLimit() const
{
	return m_controller->getSlopeLimit();
}

void CharacterController::SetSlopeLimit(float cosine)
{
	m_controller->setSlopeLimit(cosine);
}

float CharacterController::GetSlopeLimitAsAngle() const
{
	float cosine = GetSlopeLimit();
	float radian = Acos(cosine);
	return radian * Rad2Deg;
}

void CharacterController::SetSlopeLimitAsAngle(float angle)
{
	SetSlopeLimit(Cos(angle * Deg2Rad));
}

bool CharacterController::IsSlideMode() const
{
	PxControllerNonWalkableMode::Enum mode = m_controller->getNonWalkableMode();
	return mode == PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
}

void CharacterController::SetSlideMode(bool value)
{
	if (value)
	{
		m_controller->setNonWalkableMode(PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING);
	}
	else
	{
		m_controller->setNonWalkableMode(PxControllerNonWalkableMode::ePREVENT_CLIMBING);
	}
}

V3 CharacterController::GetFootPosition() const
{
	PxExtendedVec3 pxFootPosition = m_controller->getFootPosition();
	return V3(float(pxFootPosition.x), float(pxFootPosition.y), float(pxFootPosition.z));
}

void CharacterController::SetFootPosition(const V3& value)
{
	if (!IsValid())
		return;

	PxExtendedVec3 pxFootPosition = PxExtendedVec3(value.x, value.y, value.z);
	m_controller->setFootPosition(pxFootPosition);

	ApplyPositionFromCCT();
}

V3 CharacterController::GetUpDirection() const
{
	PxVec3 pxUp = m_controller->getUpDirection();
	return FromPxVec3(pxUp);
}

void CharacterController::Move(const V3& delta, float deltaTime)
{
	if (!IsValid())
		return;

	CCTMove(delta, deltaTime);
}

void CharacterController::MoveOnGround(const V3& delta, float deltaTime)
{
	if (!IsValid())
		return;

	if (delta.magnitude == 0)
		return;

	PhysicsHit hit;
	PhysicsRay ray;
	ray.Point = footPosition;
	ray.Direction = -transform->up;
	ray.Length = contactOffset + radius;

	if (system->physics->query->Raycast(hit, ray, ~m_collider->ignoreLayerBits, PhysicsQueryType::Collider, m_rigidbody))
	{
		Q q = Q::FromToRotation(V3::up(), hit.Normal);
		V3 newDelta = q.MultiplyVector(delta);
		Move(newDelta, deltaTime);
	}
	else
	{
		Move(delta, deltaTime);
	}
}

void CharacterController::Teleport(const V3& position)
{
	if (!IsValid())
		return;

	PxExtendedVec3 pxPosition;
	pxPosition.x = double(position.x);
	pxPosition.y = double(position.y);
	pxPosition.z = double(position.z);
	m_controller->setPosition(pxPosition);
	
	ApplyPositionFromCCT();
}

void CharacterController::Jump(const V3& velocity)
{
	if (!IsValid())
		return;

	m_isJump = true;
	m_isJumpFrame = true;
	m_groundHit = false;
	m_prevGroundHit = false;
	m_velocity = velocity;

	Move(velocity * system->time->fixedDeltaTime, system->time->fixedDeltaTime);
}

CharacterController::Interpolate CharacterController::GetInterpolateMode() const
{
	if (!m_currentInterpolate)
	{
		return CharacterController::Interpolate::None;
	}
	else if (m_currentInterpolate == m_interpolater)
	{
		return CharacterController::Interpolate::Interpolate;
	}
	else if (m_currentInterpolate == m_extrapolater)
	{
		return CharacterController::Interpolate::Extrapolate;
	}

	return CharacterController::Interpolate::None;
}

void CharacterController::SetInterpolateMode(CharacterController::Interpolate mode)
{
	if (mode == GetInterpolateMode())
	{
		return;
	}

	switch (mode)
	{
		case CharacterController::Interpolate::Interpolate:
			m_currentInterpolate = m_interpolater;
			m_currentInterpolate->OnEnable();
			break;
		case CharacterController::Interpolate::Extrapolate:
			m_currentInterpolate = m_extrapolater;
			m_currentInterpolate->OnEnable();
			break;
		default:
			m_currentInterpolate = nullptr;
			break;
	}
}

void CharacterController::OnHitShape(const PxControllerShapeHit& hit)
{
	if (!IsValid())
		return;

	if (OnCollision.Empty())
		return;

	CCTCollision collision = {};
	collision.HitCollider = (Collider*)hit.shape->userData;
	collision.point = V3(float(hit.worldPos.x), float(hit.worldPos.y), float(hit.worldPos.z));
	collision.normal = FromPxVec3(hit.worldNormal);
	collision.IsCCT = false;
	m_collisions.push_back(collision);
}

void CharacterController::OnHitCCT(const PxControllersHit& hit)
{
	if (!IsValid())
		return;

	if (OnCollision.Empty())
		return;

	CCTCollision collision;
	CharacterController* collisionCCT = (CharacterController*)hit.other->getUserData();
	if (!collisionCCT)
		return;
	collision.HitCollider = collisionCCT->m_collider;
	collision.point = V3(float(hit.worldPos.x), float(hit.worldPos.y), float(hit.worldPos.z));
	collision.normal = FromPxVec3(hit.worldNormal);
	collision.IsCCT = true;
	m_collisions.push_back(collision);
}

void CharacterController::OnGroundHit(const V3& normal)
{
	if (!IsValid())
		return;

	//Plane p(transform->up, V3::zero());
	//m_velocity = p.ClosetPoint(m_velocity);

	if (m_isJumpFrame)
	{
		m_isJumpFrame = false;
		return;
	}

	m_isJump = false;

	V3 up = transform->up;
	m_velocity = m_velocity - up * V3::Dot(up, m_velocity);

	if (!m_prevGroundHit)
	{
		OnLanded();
	}

	m_groundHit = true;
	m_prevGroundHit = m_groundHit;
}

void CharacterController::NotifyCollisions()
{
	for (auto& collision : m_collisions)
	{
		OnCollision(collision);
	}
}

void CharacterController::ClearCollisions()
{
	m_collisions.clear();
}

PxCapsuleControllerDesc CharacterController::CreateInitDesc(PxPhysics* physics)
{
	PxCapsuleControllerDesc desc;
	desc.setToDefault();

	// ���� ����� ���� ��Ʈ�ѷ� �ֺ��� ������ ���� ���Դϴ�.
	// 1.0���� Ŀ�� ������ �̻������δ� 2.0���� ���� ���̱⸦ �����մϴ�.
	desc.volumeGrowth = 1.5f;

	// ��Ʈ�ѷ��� ���Ǵ� ���� �������Դϴ�.
	// ��ġ�� ���е� ������ ���ϱ� ���� ���Ǹ� 0�� �ƴ� ���� ������̾�� �մϴ�.
	// �� ���� ���鿡�� ĸ���� �� ���� �ߵ��� �մϴ�.
	// ���� �� ���� 0.1�̶�� 0.1��ŭ �� ���ְ� �˴ϴ�.
	desc.contactOffset = 0.01f;

	// Kinematic body�� ������ ����Դϴ�.
	// 1.0���� �ణ ���� ���ڿ��� �մϴ�.
	desc.scaleCoeff = 0.8f;

	// Kinematic body�� �е��Դϴ�.
	desc.density = 10.0f;

	// ĸ���� ��� �����Դϴ�.
	desc.radius = 0.5f;
	desc.height = 1.0f;

	// EASY: ĸ���� ��ֿ� ���� ǥ������ �ö󰩴ϴ�.
	// CONSTRAINED: stepOffset�� ���� ǥ���� �ö󰩴ϴ�.
	desc.climbingMode = PxCapsuleClimbingMode::eEASY;

	// climbingMode�� CONSTRANED�ϋ� �ö󰥼� �ִ� ��ֹ��� �ִ� �����Դϴ�.
	desc.stepOffset = 0.0f;

	// ĳ���Ͱ� ���� �� �ִ� �ִ� ����Դϴ�.
	// ���ϴ� �Ѱ� ������ �ڻ��� ���� ����մϴ�.
	desc.slopeLimit = cos(46.0f * Deg2Rad);

	// ������ ���� �ﰢ�� ������ ������ ������ �ʴ� ���� �����Դϴ�.
	// 0.0�̸� �߰� �ﰢ���� �������� �ʽ��ϴ�.
	// slopeLimit�� ���ǵ� ����Ұ� �ﰢ�� ������ ������ �ʴ� ���� �ڵ����� �����Ҽ� �ֽ��ϴ�.
	desc.invisibleWallHeight = 0.0f;

	// �����ϴ� ĳ���Ͱ� �����Ҽ� �ִ� �ִ� �����Դϴ�.
	// �̰��� ������ �ʴ� ���� ������ ��쿡�� ���˴ϴ�.
	desc.maxJumpHeight = 0.0f;

	// ePREVENT_CLIMBING: ĳ���Ͱ� ���� �� ���� ������ ������ ���ϵ��� �����ϴ�.
	// ePREVENT_CLIMBING_AND_FORCE_SLIDING: ĳ���Ͱ� ���� �� ���� ������ ������ ���ϵ��� �ϸ�, �̲��������� �մϴ�.
	// slopeLimit�� 0�� �ƴ� ��쿡�� ���˴ϴ�.
	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING;

	desc.material = physics->createMaterial(1.0f, 1.0f, 0.0f);

	return desc;
}

void CharacterController::UpdateController(float deltaTime)
{
	if (!IsValid())
		return;

	// ��Ʈ�ѷ��� Capsule ����ȭ
	V3 position = transform->position;
	V3 up = transform->up;
	V3 absScale = V3::Abs(transform->lossyScale);
	float biggestElementOfXZ = absScale.x > absScale.z ? absScale.x : absScale.z;
	m_controller->setPosition(PxExtendedVec3(float(position.x), float(position.y), float(position.z)));
	m_controller->setRadius(radius * biggestElementOfXZ);
	m_controller->setHeight(height * absScale.y);
	m_controller->setUpDirection(ToPxVec3(up));

	// �߷� ����
	if(m_useGravity && !m_groundHit)
	{
		m_velocity += system->physics->gravity * deltaTime * m_gravityScale;
	}

	// �ӵ� ����
	CCTMove(m_velocity * deltaTime, deltaTime);
	m_velocity = V3::Lerp(m_velocity, V3::zero(), deltaTime * m_damping);
}

void CharacterController::ApplyPositionFromCCT()
{
	if (!IsValid())
		return;

	PxExtendedVec3 pxPosition = m_controller->getPosition();
	transform->position = V3(float(pxPosition.x), float(pxPosition.y), float(pxPosition.z));
}

void CharacterController::CCTMove(const V3& delta, float deltaTime)
{
	if (!IsValid())
		return;

	PxVec3 pxDelta = ToPxVec3(delta);
	m_controller->move(pxDelta, 0.0f, deltaTime, m_filters);

	ApplyPositionFromCCT();
}

bool CharacterController::IsValid() const
{
	if (!m_rigidbody || !m_collider)
		return false;

	if (!m_rigidbody->active || !m_collider->active)
		return false;

	return true;
}
