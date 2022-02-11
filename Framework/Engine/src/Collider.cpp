#include "EnginePCH.h"
#include "Collider.h"
#include "System.h"
#include "PhysicsSystem.h"
#include "PhysicsLayerManager.h"
#include "Rigidbody.h"
#include "GameObject.h"
#include "Transform.h"

void Collider::Awake()
{
	IPhysicsSystem* iPhysicsSystem = system->physicsSystem;
	PxPhysics* physics = iPhysicsSystem->GetPhysics();

	iPhysicsSystem->RegistPhysicsObject(this);
	
	m_material = physics->createMaterial(1.0f, 1.0f, 0.0f);
	m_material->setFrictionCombineMode(PxCombineMode::eMIN);
	m_material->setRestitutionCombineMode(PxCombineMode::eMIN);
	m_material->setFlag(PxMaterialFlag::eDISABLE_FRICTION, false);
	m_material->setFlag(PxMaterialFlag::eDISABLE_STRONG_FRICTION, false);
	m_material->setFlag(PxMaterialFlag::eIMPROVED_PATCH_FRICTION, true);
	
	ResetShape();

	m_beforeLocalPosition = V3::zero();
	m_beforeLocalRotation = Q::identity();
	m_beforeWorldScale = V3::one();

	FindRigidbodyAndAttach();
	ApplyFlags();
	ApplyPose(true);
	ApplyScale(true);
	ApplyLayer();
}

void Collider::BeforePhysicsSimulation()
{
	ApplyPose(false);
	ApplyScale(false);
}

void Collider::OnEnable()
{
	if (!m_shape)
		return;

	ApplyFlags();

	Rigidbody* ridigBody = GetRigidbody();
	if (rigidbody)
	{
		rigidbody->WakeUp();
	}
}

void Collider::OnDisable()
{
	if (!m_shape)
		return;

	ApplyFlags();
}

void Collider::OnDestroyed()
{
	IPhysicsSystem* iPhysicsSystem = system->physicsSystem;
	iPhysicsSystem->UnregistPhysicsObject(this);

	PxActor* actor = m_shape->getActor();
	PxRigidBody* rigidBody = static_cast<PxRigidBody*>(actor);
	if (rigidBody)
	{
		rigidBody->detachShape(*m_shape);
	}
}

Collider::~Collider()
{
	PxRelease(m_material);
	PxRelease(m_shape);
}

void Collider::SetTrigger(bool value)
{
	if (m_isTrigger == value)
		return;

	m_isTrigger = value;
	ApplyFlags();
}

void Collider::SetFriction(float value)
{
	m_material->setStaticFriction(value);
	m_material->setDynamicFriction(value);
}

void Collider::SetIngoreLayerBits(uint32 value)
{
	m_ignoreLayerBits = value;
	ApplyLayer();
}

bool Collider::IsIgnoreLayerIndex(uint8 layerIndex) const
{
	// 레이어 인덱스가 범위를 초과했습니다.
	assert(layerIndex < PhysicsLayerManager::PhysicsLayerMax);

	return m_ignoreLayerBits & (1 << layerIndex);
}

void Collider::SetIgnoreLayerIndex(uint layerIndex, bool ignore)
{
	// 레이어 인덱스가 범위를 초과했습니다.
	assert(layerIndex < PhysicsLayerManager::PhysicsLayerMax);

	if (ignore)
		m_ignoreLayerBits |= (1 << layerIndex);
	else
		m_ignoreLayerBits &= ~(1 << layerIndex);
	ApplyLayer();
}

void Collider::SetLayerIndex(uint8 layerIndex)
{
	// 레이어 인덱스가 범위를 초과했습니다.
	assert(layerIndex < PhysicsLayerManager::PhysicsLayerMax);

	m_layerIndex = layerIndex;
	ApplyLayer();
}

Rigidbody* Collider::GetRigidbody() const
{
	PxActor* actor = m_shape->getActor();

	if (!actor)
		return nullptr;

	return (Rigidbody*)actor->userData;
}

void Collider::ResetShape()
{
	IPhysicsSystem* iPhysicsSystem = system->physicsSystem;
	PxPhysics* physics = iPhysicsSystem->GetPhysics();

	bool invalid = false;
	PxGeometryHolder pxGeometry = CreatePxGeometry(invalid);

	if (!invalid)
	{
		if (!m_shape)
		{
			m_shape = physics->createShape(pxGeometry.any(), *m_material, true);
			m_shape->userData = this;
		}
		else
		{
			m_shape->setGeometry(pxGeometry.any());
			m_valid = true;
		}
	}
	else
	{
		if (!m_shape)
		{
			PxSphereGeometry pxTempGeometry;
			m_shape = physics->createShape(pxTempGeometry, *m_material, true);
			m_shape->userData = this;
		}
		m_valid = false;
	}

	PxActor* actor = m_shape->getActor();
	PxRigidBody* rigidBody = static_cast<PxRigidBody*>(actor);
	if (rigidBody)
	{
		PxRigidBodyExt::setMassAndUpdateInertia(*rigidBody, rigidBody->getMass());
	}

	ApplyFlags();
}

void Collider::FindRigidbodyAndAttach()
{
	if (GetRigidbody())
		return;

	Transform* t = transform;

	// 이 트랜스폼부터 루트 트랜스폼까지의 가장 가까운 Rigidbody 컴포넌트를 검색합니다.
	// Rigidbody 컴포넌트가 발견되면 이 콜라이더를 부착합니다.

	while (t)
	{
		Rigidbody* rigidBody = t->gameObject->GetComponent<Rigidbody>();
		if (rigidBody)
		{
			IRigidbody* iRigidBody = rigidBody;
			PxRigidDynamic* pxRigidDynamic = iRigidBody->GetPxRigidDynamic();
			if (pxRigidDynamic)
			{
				pxRigidDynamic->attachShape(*m_shape);
				ApplyPose(false);
			}
			return;
		}
		t = t->parent;
	}
}

void Collider::ApplyFlags()
{
	// 트리거 플래그: 트리거 모드이고 컴포넌트가 활성화 되어있으면 켜집니다.
	// 시뮬레이션 플래그: 트리거 모드가 비활성화이어야 하며 컴포넌트가 활성화 되어있으면 켜집니다.
	// 쿼리 플래그: 컴포넌트가 활성화 상태라면 언제나 켜져 있습니다.
	// 비주얼라이즈 플래그: 컴포넌트가 활성화 상태라면 언제나 켜져 있습니다.

	bool comActive = active && m_valid;
	bool triggerFlag = m_isTrigger && comActive;
	bool simulationFlag = !m_isTrigger && comActive;
	bool queryFlag = comActive;
	bool visualizationFlag = comActive;

	// eTRIGGER_SHAPE 플래그를 활성화 하기 전에는 반드시 eSIMULATION_SHAPE 플래그가 비활성화 상태여야 합니다.
	// 때문에 아래와 같은 순서로 플래그를 설정합니다.

	m_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, simulationFlag);
	m_shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, triggerFlag);
	m_shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, queryFlag);
	m_shape->setFlag(PxShapeFlag::eVISUALIZATION, visualizationFlag);
}

void Collider::ApplyPose(bool unconditionally)
{
	Rigidbody* rigidBody = GetRigidbody();

	V3 localPositionFromBody;
	Q  localRotationFromBody;

	if (rigidBody)
	{
		localPositionFromBody = rigidBody->transform->worldToLocalMatrix.MultiplyPoint(transform->position);
		localRotationFromBody = rigidBody->transform->rotation.inversed * transform->rotation;
	}
	else
	{
		localPositionFromBody = V3::zero();
		localRotationFromBody = Q::identity();
	}

	float distance = V3::Distance(m_beforeLocalPosition, localPositionFromBody);
	float radian = Q::Radian(m_beforeLocalRotation, localRotationFromBody);

	// Shape의 로컬 변환과 트랜스폼의 로컬 변환이 지정한 값보다 차이난다면
	// 트랜스폼의 로컬 변환을 Shape에 설정합니다.

	if (unconditionally ||
		distance > Epsilon ||
		radian > Epsilon)
	{
		m_beforeLocalPosition = localPositionFromBody;
		m_beforeLocalRotation = localRotationFromBody;

		Q localRotationFromBody_WithDefaultRotation = m_defaultRotation * localRotationFromBody;

		PxTransform pose;
		pose.p = ToPxVec3(localPositionFromBody);
		pose.q = ToPxQuat(localRotationFromBody_WithDefaultRotation);
		m_shape->setLocalPose(pose);
	}
}

void Collider::ApplyScale(bool unconditionally)
{
	V3 worldScale = transform->lossyScale;

	float distance = V3::Distance(worldScale, m_beforeWorldScale);

	if (unconditionally ||
		distance > 0.001f)
	{
		m_beforeWorldScale = worldScale;
		ResetShape();
	}
}

void Collider::ApplyLayer()
{
	IPhysicsSystem* iPhysicsSystem = system->physicsSystem;
	PxPhysics* physics = iPhysicsSystem->GetPhysics();

	PxFilterData simulationFilterData = {};					// 시뮬레이션에 사용되는 필터 데이터입니다.
	simulationFilterData.word0 = (1 << m_layerIndex);		// 이 콜라이더의 레이어 비트입니다.
	simulationFilterData.word1 = ~m_ignoreLayerBits;		// 이 콜라이더의 충돌 가능한 레이어 비트들입니다.

	PxFilterData queryFilterData = {};						// 쿼리에 사용되는 필터 데이터입니다.
	queryFilterData.word0 = (1 << m_layerIndex);			// 이 콜라이더의 레이어 비트입니다.
	
	m_shape->setSimulationFilterData(simulationFilterData);
	m_shape->setQueryFilterData(queryFilterData);
}

bool Collider::IsWorking()
{
	return active;
}
