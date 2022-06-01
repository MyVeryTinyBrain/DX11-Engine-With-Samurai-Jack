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
	assert(m_controller != nullptr); // 캐릭터 컨트롤러 생성이 실패했습니다.

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

	// 성능 향상을 위한 컨트롤러 주변의 공간에 대한 값입니다.
	// 1.0보다 커야 하지만 이상적으로는 2.0보다 작은 값이기를 권장합니다.
	desc.volumeGrowth = 1.5f;

	// 컨트롤러에 사용되는 접점 오프셋입니다.
	// 수치적 정밀도 문제를 피하기 위해 사용되며 0이 아닌 작은 양수값이어야 합니다.
	// 이 값은 지면에서 캡슐이 더 높이 뜨도록 합니다.
	// 만약 이 값이 0.1이라면 0.1만큼 더 떠있게 됩니다.
	desc.contactOffset = 0.01f;

	// Kinematic body의 스케일 계수입니다.
	// 1.0보다 약간 작은 숫자여야 합니다.
	desc.scaleCoeff = 0.8f;

	// Kinematic body의 밀도입니다.
	desc.density = 10.0f;

	// 캡슐의 모양 설정입니다.
	desc.radius = 0.5f;
	desc.height = 1.0f;

	// EASY: 캡슐이 노멀에 따라 표면위로 올라갑니다.
	// CONSTRAINED: stepOffset에 따라 표면을 올라갑니다.
	desc.climbingMode = PxCapsuleClimbingMode::eEASY;

	// climbingMode가 CONSTRANED일떄 올라갈수 있는 장애물의 최대 높이입니다.
	desc.stepOffset = 0.0f;

	// 캐릭터가 걸을 수 있는 최대 경사입니다.
	// 원하는 한계 각도의 코사인 값을 사용합니다.
	desc.slopeLimit = cos(46.0f * Deg2Rad);

	// 걸을수 없는 삼각형 주위에 생성된 보이지 않는 벽의 높이입니다.
	// 0.0이면 추가 삼각형이 생성되지 않습니다.
	// slopeLimit로 정의된 보행불가 삼각형 주위에 보이지 않는 벽을 자동으로 생성할수 있습니다.
	desc.invisibleWallHeight = 0.0f;

	// 점프하는 캐릭터가 도달할수 있는 최대 높이입니다.
	// 이것은 보이지 않는 벽이 생성된 경우에만 사용됩니다.
	desc.maxJumpHeight = 0.0f;

	// ePREVENT_CLIMBING: 캐릭터가 걸을 수 없는 경사면을 오르지 못하도록 막습니다.
	// ePREVENT_CLIMBING_AND_FORCE_SLIDING: 캐릭터가 걸을 수 없는 경사면을 오르지 못하도록 하며, 미끄러지도록 합니다.
	// slopeLimit이 0이 아닌 경우에만 사용됩니다.
	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING;

	desc.material = physics->createMaterial(1.0f, 1.0f, 0.0f);

	return desc;
}

void CharacterController::UpdateController(float deltaTime)
{
	if (!IsValid())
		return;

	// 컨트롤러의 Capsule 동기화
	V3 position = transform->position;
	V3 up = transform->up;
	V3 absScale = V3::Abs(transform->lossyScale);
	float biggestElementOfXZ = absScale.x > absScale.z ? absScale.x : absScale.z;
	m_controller->setPosition(PxExtendedVec3(float(position.x), float(position.y), float(position.z)));
	m_controller->setRadius(radius * biggestElementOfXZ);
	m_controller->setHeight(height * absScale.y);
	m_controller->setUpDirection(ToPxVec3(up));

	// 중력 적용
	if(m_useGravity && !m_groundHit)
	{
		m_velocity += system->physics->gravity * deltaTime * m_gravityScale;
	}

	// 속도 적용
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
