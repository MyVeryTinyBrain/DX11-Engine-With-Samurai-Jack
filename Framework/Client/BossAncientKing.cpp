#include "stdafx.h"
#include "BossAncientKing.h"
#include "Config.h"
#include "BossAncientKingAnimator.h"

#define ATK_MAX_DIST 5.0f
#define ATK_MAX_ANGLE 20.0f
#define LOOK_STOP_MIN_ANGLE 5.0f
#define LOOK_START_ANGLE 100.0f
#define IDLE_TIME 2.0f
#define WAIT_DEFAULT_TIME 1.0f
#define TRACE_SPIN_ANGLE_PER_SEC 50.0f
#define LOOK_SPIN_ANGLE_PER_SEC 140.0f
#define TRACE_TO_LOOK_TIME 5.0f

#define RUSH_START_ACCTIME 8.0f
#define RUSH_MIN_ANGLE 4.0f
#define RUSH_SPIN_ANGLE_PER_SEC 20.0f

void BossAncientKing::Awake()
{
	Boss::Awake();

	CCT->radius = 2.85f;
	CCT->height = 0.01f;
	CCT->capsuleCollider->layerIndex = PhysicsLayer_VirtualEnemy;

	SetupCollider();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupLight();
	SetupHammer();
	SetupAttackTrigger();

	SetState(State::IDLE);
	m_idleLeftCounter = 2.0f;
}

void BossAncientKing::Start()
{
	Boss::Start();
}

void BossAncientKing::Update()
{
	Boss::Update();

	StateUpdate();
}

void BossAncientKing::LateUpdate()
{
	Boss::LateUpdate();
	UpdateCCT();
	UpdateAttachmentObjects();
}

void BossAncientKing::FixedUpdate()
{
	Boss::FixedUpdate();
}

void BossAncientKing::SetupCollider()
{
	GameObject* goCollider = CreateGameObjectToChild(transform);
	m_collider = goCollider->AddComponent<CapsuleCollider>();
	m_collider->layerIndex = PhysicsLayer_Enemy;
	m_collider->radius = CCT->radius;
	m_collider->halfHeight = CCT->height * 0.5f + 2.0f;
}

void BossAncientKing::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = V3(0, -3.05f, 0);
	m_goCharacterRender->transform->localEulerAngles = V3(90, 180, 0);
	m_goCharacterRender->transform->localScale = V3::one() * 1.5f;
	m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
	m_characterRenderer->mesh = system->resource->Find(MESH_ANCIENT_KING);
	m_characterRenderer->SetupStandardMaterials();

	m_L_Hand_Weapon_cnt_tr = m_characterRenderer->GetNodeTransformByName(TEXT("L_Hand_Weapon_cnt_tr"));
	m_R_Hand_Weapon_cnt_tr = m_characterRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));
	m_Head = m_characterRenderer->GetNodeTransformByName(TEXT("Head"));
	m_RightFoot = m_characterRenderer->GetNodeTransformByName(TEXT("RightFoot"));
	m_LeftFoot = m_characterRenderer->GetNodeTransformByName(TEXT("LeftFoot"));
	m_RightLeg = m_characterRenderer->GetNodeTransformByName(TEXT("RightLeg"));
	m_LeftLeg = m_characterRenderer->GetNodeTransformByName(TEXT("LeftLeg"));
	m_Spine1 = m_characterRenderer->GetNodeTransformByName(TEXT("Spine1"));
	m_RightArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("RightArmRoll"));
	m_LeftArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("LeftArmRoll"));
	m_RightForeArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("RightForeArmRoll"));
	m_LeftForeArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("LeftForeArmRoll"));
}

void BossAncientKing::SetupAnimator()
{
	m_animator = m_goCharacterRender->AddComponent<BossAncientKingAnimator>();
	m_animator->OnBeginChanging += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)>(this, &BossAncientKing::OnBeginChanging);
	m_animator->OnEndChanged += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)>(this, &BossAncientKing::OnEndChanged);
	m_animator->OnAnimationEvent += func<void(Ref<AnimatorLayer>, const AnimationEventDesc&)>(this, &BossAncientKing::OnAnimationEvent);
}

void BossAncientKing::SetupLight()
{
	m_goLight = CreateGameObjectToChild(transform);
	m_pointLight = m_goLight->AddComponent<PointLight>();
	m_pointLight->diffuse = Color::red();
	m_pointLight->ambient = Color::red() * 0.1f;
	m_pointLight->range = 15.0f;
	m_pointLight->intensity = 1.0f;
}

void BossAncientKing::SetupHammer()
{
	m_goHammer = CreateGameObjectToChild(m_goCharacterRender->transform);
	m_goHammer->transform->localScale = V3::one() * 5.0f;
	m_hammerRenderer = m_goHammer->AddComponent<MeshRenderer>();
	m_hammerRenderer->mesh = system->resource->Find(MESH_ANCIENT_KING_HAMMER);
	m_hammerRenderer->SetupStandardMaterials();

	m_goHammerTrail = CreateGameObjectToChild(m_goHammer->transform);
	m_goHammerTrail->transform->localPosition = V3(0, 0, -0.72f);
	//m_goHammerTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_hammerTrailRenderer = m_goHammerTrail->AddComponent<TrailRenderer>();
	m_hammerTrailRenderer->alignment = TrailRenderer::Alignment::View;
	m_hammerTrailRenderer->shrinkDistance = 50.0f;
	m_hammerTrailRenderer->width = 4.0f;
	m_hammerTrailRenderer->autoTrail = true;
}

void BossAncientKing::SetupAttackTrigger()
{
}

void BossAncientKing::UpdateCCT()
{
	if (CCT->isGrounded)
	{
		CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
		Q deltaRotation = Q::RightHandedToLeftHanded(m_animator->Layer->deltaRotation);
		if(deltaRotation.eulerAngles.sqrMagnitude > Epsilon)
			transform->localRotation *= Q::RightHandedToLeftHanded(m_animator->Layer->deltaRotation);
	}
}

void BossAncientKing::UpdateAttachmentObjects()
{
	m_goHammer->transform->position = m_L_Hand_Weapon_cnt_tr->position;
	m_goHammer->transform->rotation = m_L_Hand_Weapon_cnt_tr->rotation;

	m_goLight->transform->position = m_Head->position;
	m_goLight->transform->rotation = m_Head->rotation;
}

void BossAncientKing::AttackTriggerQuery()
{
}

void BossAncientKing::OffAttackTriggers()
{
}

void BossAncientKing::ClearHitBuffer()
{
}

void BossAncientKing::OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing)
{
}

void BossAncientKing::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
	if (prev && prev->name.find(TEXT("ATK")) != tstring::npos &&
		endChanged.GetPointer() == m_animator->BH_IDLE)
	{
		SetState(State::WAIT);
	}

	if (prev && prev->name.find(TEXT("ATK")) != tstring::npos && 
		prev.GetPointer() != m_animator->ATK_TURN)
	{
		m_usedSATK_TURN = false;
	}
}

void BossAncientKing::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
}

void BossAncientKing::SetAttackType(int contextInt)
{
}

float BossAncientKing::GetHP() const
{
	return m_hp;
}

void BossAncientKing::SetHP(float value)
{
	m_hp = value;
}

V3 BossAncientKing::GetDirection() const
{
	return transform->forward;
}

void BossAncientKing::SetDirection(const V3& direction)
{
	transform->forward = direction;
}

bool BossAncientKing::IsGuarding() const
{
	return false;
}

bool BossAncientKing::IsGuardBreakableByBackattack() const
{
	return false;
}

bool BossAncientKing::IsInvisible() const
{
	return false;
}

bool BossAncientKing::IsSuperarmor() const
{
	return false;
}

DamageOutType BossAncientKing::OnDamage(const DamageOut& out)
{
	if (isDead)
		return DamageOutType::IGNORED;

	switch (out.Type)
	{
		case DamageOutType::GUARDED:
		case DamageOutType::GUARD_BREAKED:
		case DamageOutType::HIT:
		{
			m_animator->AdditiveDamageTProperty->SetTriggerState();
			return DamageOutType::HIT;
		}
		break;
		default:
		{
			return DamageOutType::IGNORED;
		}
		break;
	}
}

void BossAncientKing::SetState(BossAncientKing::State state)
{
	if (state == m_state)
		return;
	
	State before = m_state;
	State next = state;
	m_state = state;
	StateEnded(before, next);
	StateChanged(before, next);
}

void BossAncientKing::StateUpdate()
{
	switch (m_state)
	{
		case State::IDLE:
		{
			if (m_idleLeftCounter <= 0.0f)
			{
				SetState(State::TRACE);
			}
			else
			{
				m_idleLeftCounter -= system->time->deltaTime;
			}
		}
		break;
		case State::WAIT:
		{
			if (m_waitLeftCounter <= 0.0f)
			{
				SetState(State::TRACE);
			}
			else
			{
				m_waitLeftCounter -= system->time->deltaTime;
			}
		}
		break;
		case State::TRACE:
		{
			m_traceOrLookAccCounter += system->time->deltaTime;
			
			// 시야각 내에 없다면 누적합니다.
			if (XZAngleBetweenPlayer() > LOOK_START_ANGLE)
				m_traceOutAngleAccCounter += system->time->deltaTime;

			// 걷기 애니메이션 중에 회전합니다.
			RotateOnYAxisToDirection(ToPlayerDirectionXZ(), TRACE_SPIN_ANGLE_PER_SEC, system->time->deltaTime);

			if (IsATKCondition())
			{
				SetState(State::ATK_RAND);
			}
			else if (IsSATKTurnCondition() && XZAngleBetweenPlayer() > LOOK_START_ANGLE && XZDistanceBetweenPlayer() < ATK_MAX_DIST)
			{
				SetState(State::SATK_TURN);
			}
			else if (m_traceOutAngleAccCounter > TRACE_TO_LOOK_TIME)
			{
				// 일정 시간동안 시야각 내에 없었다면 회전을 시작합니다.
				SetState(State::LOOK);
			}
			else if (m_traceOrLookAccCounter > RUSH_START_ACCTIME && XZAngleBetweenPlayer() < RUSH_MIN_ANGLE)
			{
				SetState(State::ATK_RUSH);
			}
		}
		break;
		case State::LOOK:
		{
			m_traceOrLookAccCounter += system->time->deltaTime;

			// 회전 애니메이션 중에 회전합니다.
			if (m_animator->Layer->IsPlaying(m_animator->BH_TURN_ROTATE))
				RotateOnYAxisToDirection(ToPlayerDirectionXZ(), LOOK_SPIN_ANGLE_PER_SEC, system->time->deltaTime);

			if (IsATKCondition())
			{
				SetState(State::ATK_RAND);
			}
			else if (IsSATKTurnCondition() && XZAngleBetweenPlayer() > LOOK_START_ANGLE && XZDistanceBetweenPlayer() < ATK_MAX_DIST)
			{
				SetState(State::SATK_TURN);
			}
			else if (XZAngleBetweenPlayer() < LOOK_STOP_MIN_ANGLE)
			{
				// 플레이어를 요구 각도 이내로 쳐다본다면 추적을 시작합니다.
				SetState(State::TRACE);
			}
		}
		break;
		case State::ATK_SWING_H:
		case State::ATK_STEPON:
		case State::ATK_DOWNSTRIKE:
		{
			m_animator->KeepAttackBProperty->valueAsBool = XZAngleBetweenPlayer() < ATK_MAX_ANGLE;
		}
		break;
		case State::ATK_RUSH:
		{
			if (m_animator->Layer->IsPlaying(m_animator->ATK_RUSH_ST) ||
				m_animator->Layer->IsPlaying(m_animator->ATK_RUSH_LP))
			{
				RotateOnYAxisToDirection(ToPlayerDirectionXZ(), RUSH_SPIN_ANGLE_PER_SEC, system->time->deltaTime);
			}

			m_animator->KeepAttackBProperty->valueAsBool =
				!system->physics->query->SweepSphereTest(
					CCT->capsuleCollider->transform->position,
					CCT->capsuleCollider->radius,
					transform->forward,
					m_collider->radius + 1.0f,
					1 << PhysicsLayer_Default,
					PhysicsQueryType::Collider,
					CCT->rigidbody);
			if (m_animator->Layer->IsPlaying(m_animator->ATK_RUSH_ED))
				m_animator->KeepAttackBProperty->valueAsBool = false;
		}
		break;
	}
}

void BossAncientKing::StateChanged(BossAncientKing::State before, BossAncientKing::State next)
{
	switch (next)
	{
		case State::IDLE:
		{
			m_animator->WalkBProperty->valueAsBool = false;
			m_idleLeftCounter = IDLE_TIME;
		}
		break;
		case State::WAIT:
		{
			m_animator->WalkBProperty->valueAsBool = false;

			// 실수로 설정 안했을 경우에는 기본값을 사용합니다.
			if(m_waitLeftCounter <= 0.0f)
				m_waitLeftCounter = WAIT_DEFAULT_TIME;
		}
		break;
		case State::TRACE:
		{
			if (before != State::LOOK &&
				next != State::LOOK)
				m_traceOrLookAccCounter = 0.0f;
			m_traceOutAngleAccCounter = 0.0f;

			m_animator->WalkBProperty->valueAsBool = true;
			if (XZAngleBetweenPlayer() > LOOK_START_ANGLE)
			{
				SetState(State::LOOK);
			}
			else if (IsATKCondition())
			{
				SetState(State::ATK_RAND);
			}
		}
		break;
		case State::LOOK:
		{
			if (before != State::TRACE &&
				next != State::TRACE)
				m_traceOrLookAccCounter = 0.0f;

			m_animator->TurnBProperty->valueAsBool = true;
			if (IsATKCondition())
			{
				SetState(State::ATK_RAND);
			}
		}
		break;
		case State::ATK_RAND:
		{
			if (!IsATKCondition()) cout << "Is not ATK Condition" << endl;
			int numATK = (int)State::ATK_END - (int)State::ATK_BEGIN;
			int indexATK = (int)State::ATK_BEGIN + rand() % numATK + 1;
			State atk = (State)indexATK;
			SetState(atk);
		}
		break;
		case State::SATK_TURN:
		{
			m_usedSATK_TURN = true;
			m_animator->ATK_TURN_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_SWING_H:
		{
			m_animator->ATK_SWING_H_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_SWING_V:
		{
			m_animator->ATK_SWING_V_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_STOMP:
		{
			m_animator->ATK_STOMP_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_STEPON:
		{
			m_animator->ATK_STEPON_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_RUSH:
		{
			m_animator->ATK_RUSH_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_JUMP:
		{
			m_animator->ATK_JUMP_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_ELECTRIC:
		{
			m_animator->ATK_ELECTRIC_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_DOWNSTRIKE:
		{
			m_animator->ATK_DOWNSTRIKE_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_BEAM:
		{
			m_animator->ATK_BEAM_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_BEGIN:
		case State::ATK_END:
		{
			SetState(State::ATK_RUSH);
		}
		break;
	}
}

void BossAncientKing::StateEnded(BossAncientKing::State before, BossAncientKing::State current)
{
	switch (before)
	{
		case State::IDLE:
		{
			m_idleLeftCounter = 0.0f;
		}
		break;
		case State::WAIT:
		{
			m_waitLeftCounter = 0.0f;
		}
		break;
		case State::TRACE:
		{
			if (before != State::LOOK)
				m_traceOrLookAccCounter = 0.0f;
			m_traceOutAngleAccCounter = 0.0f;

			m_animator->WalkBProperty->valueAsBool = false;
		}
		break;
		case State::LOOK:
		{
			if (before != State::TRACE)
				m_traceOrLookAccCounter = 0.0f;

			m_animator->TurnBProperty->valueAsBool = false;
		}
		break;
	}
}

bool BossAncientKing::RaycastToForwardInStage(float length) const
{
	PhysicsHit hit;
	PhysicsRay ray;
	ray.Direction = transform->forward;
	ray.Length = length;
	ray.Point = transform->position;

	if (system->physics->query->RaycastTest(ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
	{
		return true;
	}

	return false;
}

bool BossAncientKing::IsATKCondition() const
{
	if (XZDistanceBetweenPlayer() > ATK_MAX_DIST)
		return false;

	if (XZAngleBetweenPlayer() > ATK_MAX_ANGLE)
		return false;

	return true;
}

bool BossAncientKing::IsSATKTurnCondition() const
{
	return !m_usedSATK_TURN;
}
