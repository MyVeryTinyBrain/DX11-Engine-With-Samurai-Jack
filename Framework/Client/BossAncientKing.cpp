#include "stdafx.h"
#include "BossAncientKing.h"
#include "Config.h"
#include "BossAncientKingAnimator.h"

#include "Player.h"
#include "TPSCamera.h"
#include "EffectShockwave.h"
#include "ProjectileWaveBeam.h"

#define ATK_NEAR_MAX_DIST 6.0f
#define ATK_NEAR_MAX_ANGLE 20.0f
#define ATK_FAR_MAX_DIST 15.0f
#define ATK_FAR_MAX_ANGLE 30.0f
#define ATK_KEEP_MAX_ANGLE 35.0f
#define LOOK_STOP_MIN_ANGLE 5.0f
#define LOOK_START_ANGLE 100.0f
#define IDLE_TIME 2.0f
#define WAIT_DEFAULT_TIME 0.2f
#define TRACE_SPIN_ANGLE_PER_SEC 50.0f
#define LOOK_SPIN_ANGLE_PER_SEC 140.0f
#define TRACE_TO_LOOK_TIME 5.0f
#define SPIN_ON_ATK_NEAR_ANGLE_PER_SEC 19.0f
#define SPIN_ON_ATK_FAR_ANGLE_PER_SEC 25.0f

#define RUSH_START_ACCTIME 12.0f
#define RUSH_MIN_ANGLE 4.0f
#define RUSH_SPIN_ANGLE_PER_SEC 20.0f

#define ATK_FAR_DELAY 7.0f

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
	AttackTriggerQuery();

	m_farATKCounter -= system->time->deltaTime;
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
	m_goHammer->transform->localScale = V3::one() * 4.0f;
	m_hammerRenderer = m_goHammer->AddComponent<MeshRenderer>();
	m_hammerRenderer->mesh = system->resource->Find(MESH_ANCIENT_KING_HAMMER);
	m_hammerRenderer->SetupStandardMaterials();

	m_goHammerTrail = CreateGameObjectToChild(m_goHammer->transform);
	m_goHammerTrail->transform->localPosition = V3(0.0f, 0.0f, -0.9f);
	//m_goHammerTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_hammerTrailRenderer = m_goHammerTrail->AddComponent<TrailRenderer>();
	m_hammerTrailRenderer->alignment = TrailRenderer::Alignment::View;
	m_hammerTrailRenderer->shrinkDistance = 30.0f;
	m_hammerTrailRenderer->width = 4.0f;
	m_hammerTrailRenderer->autoTrail = false;
	m_hammerTrailRenderer->interpolateStep = 30;
	m_hammerTrailRenderer->enable = false;
}

void BossAncientKing::SetupAttackTrigger()
{
	m_goAttackTrigger[HAMMER_TRIGGER] = CreateGameObjectToChild(m_goHammer->transform);
	m_goAttackTrigger[HAMMER_TRIGGER]->transform->localPosition = V3(0.0f, 0.0f, -0.7f);
	m_goAttackTrigger[HAMMER_TRIGGER]->transform->localEulerAngles = V3(90.0f, 0.0f, 0.0f);
	m_attackTrigger[HAMMER_TRIGGER] = m_goAttackTrigger[HAMMER_TRIGGER]->AddComponent<CapsuleCollider>();
	CapsuleCollider* hammer_trigger = (CapsuleCollider*)m_attackTrigger[HAMMER_TRIGGER];
	hammer_trigger->radius = 0.7f;
	hammer_trigger->halfHeight = 0.4f;
	hammer_trigger->isTrigger = true;
	hammer_trigger->enable = false;

	m_goAttackTrigger[FOOT_L_TRIGGER] = CreateGameObjectToChild(transform);
	m_goAttackTrigger[FOOT_L_TRIGGER]->transform->localPosition = V3(0, 0, 0);
	m_attackTrigger[FOOT_L_TRIGGER] = m_goAttackTrigger[FOOT_L_TRIGGER]->AddComponent<SphereCollider>();
	SphereCollider* foot_l_trigger = (SphereCollider*)m_attackTrigger[FOOT_L_TRIGGER];
	foot_l_trigger->radius = 4.5f;
	foot_l_trigger->isTrigger = true;
	foot_l_trigger->enable = false;

	m_goAttackTrigger[FOOT_R_TRIGGER] = CreateGameObjectToChild(transform);
	m_goAttackTrigger[FOOT_R_TRIGGER]->transform->localPosition = V3(0, 0, 0);
	m_attackTrigger[FOOT_R_TRIGGER] = m_goAttackTrigger[FOOT_R_TRIGGER]->AddComponent<SphereCollider>();
	SphereCollider* foot_r_trigger = (SphereCollider*)m_attackTrigger[FOOT_R_TRIGGER];
	foot_r_trigger->radius = 4.5f;
	foot_r_trigger->isTrigger = true;
	foot_r_trigger->enable = false;
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

	m_goAttackTrigger[FOOT_L_TRIGGER]->transform->position = m_LeftFoot->position;
	m_goAttackTrigger[FOOT_L_TRIGGER]->transform->rotation = m_LeftFoot->rotation;

	m_goAttackTrigger[FOOT_R_TRIGGER]->transform->position = m_RightFoot->position;
	m_goAttackTrigger[FOOT_R_TRIGGER]->transform->rotation = m_RightFoot->rotation;
}

void BossAncientKing::AttackTriggerQuery()
{
	for (int i = 0; i < MAX_TRIGGERS; ++i)
	{
		if (!m_attackTrigger[i]->enable)
			continue;

		vector<Collider*> overlaps = system->physics->query->OverlapAll(m_attackTrigger[i], 1 << PhysicsLayer_Player, PhysicsQueryType::Collider);
		for (auto& overlap : overlaps)
		{
			Rigidbody* rigidbody = overlap->rigidbody;
			if (!rigidbody) continue;

			Player* player = rigidbody->gameObject->GetComponent<Player>();
			if (!player) continue;

			auto result = m_hitBuffer.insert(rigidbody);
			if (!result.second) continue; // 이미 힛 버퍼에 존재합니다.

			DamageIn damage = {};
			damage.FromComponent = this;
			damage.FromDirection = player->transform->position - transform->position;
			damage.Guardable = m_gadableAttack;
			switch (m_attackType)
			{
				case ANIM_ATK_LIGHT:
					damage.Type = DamageInType::LIGHT;
					damage.Damage = 1.0f;
					break;
				case ANIM_ATK_HEAVY:
					damage.Type = DamageInType::HEAVY;
					damage.Damage = 2.5f;
					break;
				case ANIM_ATK_BLOW:
					damage.Type = DamageInType::BLOW;
					damage.Damage = 3.0f;
					break;
				case ANIM_ATK_BLOWUP:
					damage.Damage = 2.0f;
					damage.Type = DamageInType::BLOWUP;
					break;
				case ANIM_ATK_BLOWDOWN:
					damage.Damage = 3.0f;
					damage.Type = DamageInType::BLOWDOWN;
					break;
			}
			player->Damage(damage);
		}
	}
}

void BossAncientKing::OffAttackTriggers()
{
	for (int i = 0; i < MAX_TRIGGERS; ++i)
		m_attackTrigger[i]->enable = false;
}

void BossAncientKing::ClearHitBuffer()
{
	if (!m_hitBuffer.empty())
		m_hitBuffer.clear();
}

void BossAncientKing::OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing)
{
	if (changing->name.find(TEXT("ATK")) != tstring::npos)
	{
		m_gadableAttack = false;
	}
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

	if (prev->name.find(TEXT("ATK")) != tstring::npos)
	{
		m_hammerTrailRenderer->autoTrail = false;
		OffAttackTriggers();
		ClearHitBuffer();
	}
}

void BossAncientKing::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
	if (desc.ContextByte & ANIM_CAM_SHAKE)
	{
		float shakeValue = desc.ContextFloat;
		Player* player = Player::GetInstance();
		if (player)
		{
			TPSCamera* camera = Player::GetInstance()->GetTPSCamera();
			camera->Shake(V3::up(), shakeValue * 0.1f, 1.1f, 0.15f, 4.0f / 0.15f);
		}
	}

	if (desc.ContextInt & ANIM_ATK_GADABLE)
	{
		m_gadableAttack = true;
	}

	if (desc.ContextByte & ANIM_LF_SHOCKWAVE)
	{
		EffectShockwave::Create(
			gameObject->regionScene,
			m_attackTrigger[FOOT_L_TRIGGER]->transform->position + V3::down() * 0.5f,
			1.0f,
			2.0f,
			1.0f, 9.0f
		);
	}
	if (desc.ContextByte & ANIM_RF_SHOCKWAVE)
	{
		EffectShockwave::Create(
			gameObject->regionScene,
			m_attackTrigger[FOOT_R_TRIGGER]->transform->position + V3::down() * 0.5f,
			1.0f,
			2.0f,
			1.0f, 9.0f
		);
	}

	if (desc.ContextInt & ANIM_ATK_HAMMER_START)
	{
		m_hammerTrailRenderer->autoTrail = true;
		m_attackTrigger[HAMMER_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & ANIM_ATK_HAMMER_END)
	{
		m_hammerTrailRenderer->autoTrail = false;
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & ANIM_ATK_LF_START)
	{
		m_attackTrigger[FOOT_L_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & ANIM_ATK_LF_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & ANIM_ATK_RF_START)
	{
		m_attackTrigger[FOOT_R_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & ANIM_ATK_RF_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}

	if (desc.ContextByte & ANIM_AIM_BEGIN)
	{
		m_aimPosition = player->transform->position;
	}
	if (desc.ContextInt & ANIM_ATK_BEAM)
	{
		GameObject* goBeam = CreateGameObject();
		goBeam->transform->position = m_Head->position;
		goBeam->transform->forward = (m_aimPosition - m_Head->position).normalized;
		ProjectileWaveBeam* beam = goBeam->AddComponent<ProjectileWaveBeam>();
	}
}

void BossAncientKing::SetAttackType(int contextInt)
{
	m_attackType = ANIM_ATK_LIGHT;

	if (contextInt & ANIM_ATK_LIGHT)
		m_attackType = ANIM_ATK_LIGHT;
	else if (contextInt & ANIM_ATK_HEAVY)
		m_attackType = ANIM_ATK_HEAVY;
	else if (contextInt & ANIM_ATK_BLOW)
		m_attackType = ANIM_ATK_BLOW;
	else if (contextInt & ANIM_ATK_BLOWUP)
		m_attackType = ANIM_ATK_BLOWUP;
	else if (contextInt & ANIM_ATK_BLOWDOWN)
		m_attackType = ANIM_ATK_BLOWDOWN;
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

			if (IsNearATKCondition())
			{
				SetState(State::ATK_NEAR_RAND);
			}
			else if (IsSATKTurnCondition() && XZAngleBetweenPlayer() > LOOK_START_ANGLE && XZDistanceBetweenPlayer() < ATK_NEAR_MAX_DIST)
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
			else if (IsFarATKCondition())
			{
				SetState(State::ATK_FAR_RAND);
			}
		}
		break;
		case State::LOOK:
		{
			m_traceOrLookAccCounter += system->time->deltaTime;

			// 회전 애니메이션 중에 회전합니다.
			if (m_animator->Layer->IsPlaying(m_animator->BH_TURN_ROTATE))
				RotateOnYAxisToDirection(ToPlayerDirectionXZ(), LOOK_SPIN_ANGLE_PER_SEC, system->time->deltaTime);

			if (IsNearATKCondition())
			{
				SetState(State::ATK_NEAR_RAND);
			}
			else if (IsSATKTurnCondition() && XZAngleBetweenPlayer() > LOOK_START_ANGLE && XZDistanceBetweenPlayer() < ATK_NEAR_MAX_DIST)
			{
				SetState(State::SATK_TURN);
			}
			else if (XZAngleBetweenPlayer() < LOOK_STOP_MIN_ANGLE)
			{
				// 플레이어를 요구 각도 이내로 쳐다본다면 추적을 시작합니다.
				SetState(State::TRACE);
			}
			else if (IsFarATKCondition())
			{
				SetState(State::ATK_FAR_RAND);
			}
		}
		break;
		case State::ATK_SWING_H:
		case State::ATK_STEPON:
		case State::ATK_DOWNSTRIKE:
		{
			m_animator->KeepAttackBProperty->valueAsBool = XZAngleBetweenPlayer() < ATK_KEEP_MAX_ANGLE;
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
					CCT->capsuleCollider->radius * 0.95f,
					transform->forward,
					m_collider->radius + 1.5f,
					1 << PhysicsLayer_Default,
					PhysicsQueryType::Collider,
					CCT->rigidbody);
			if (m_animator->Layer->IsPlaying(m_animator->ATK_RUSH_ED))
				m_animator->KeepAttackBProperty->valueAsBool = false;
		}
		break;
	}

	// Spin when attack
	switch (m_state)
	{
		case State::ATK_SWING_H:
		case State::ATK_SWING_V:
		case State::ATK_STOMP:
		case State::ATK_STEPON:
		case State::ATK_JUMP:
		case State::ATK_DOWNSTRIKE:
			RotateOnYAxisToDirection(ToPlayerDirectionXZ(), SPIN_ON_ATK_NEAR_ANGLE_PER_SEC, system->time->deltaTime);
			break;
		case State::ATK_ELECTRIC:
		case State::ATK_BEAM:
			RotateOnYAxisToDirection(ToPlayerDirectionXZ(), SPIN_ON_ATK_FAR_ANGLE_PER_SEC, system->time->deltaTime);
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
			else if (IsNearATKCondition())
			{
				SetState(State::ATK_NEAR_RAND);
			}
			else if (IsFarATKCondition())
			{
				SetState(State::ATK_FAR_RAND);
			}
		}
		break;
		case State::LOOK:
		{
			if (before != State::TRACE &&
				next != State::TRACE)
				m_traceOrLookAccCounter = 0.0f;

			m_animator->TurnBProperty->valueAsBool = true;
			if (IsNearATKCondition())
			{
				SetState(State::ATK_NEAR_RAND);
			}
			else if (IsFarATKCondition())
			{
				SetState(State::ATK_FAR_RAND);
			}
		}
		break;
		case State::ATK_FAR_RAND:
		{
			int numATK = (int)State::__ATK_FAR_END - (int)State::__ATK_FAR_BEGIN;
			int indexATK = (int)State::__ATK_FAR_BEGIN + (rand() % (numATK - 1)) + 1;
			State atk = (State)indexATK;
			SetState(atk);
		}
		break;
		case State::ATK_NEAR_RAND:
		{
			int numATK = (int)State::__ATK_NEAR_END - (int)State::__ATK_NEAR_BEGIN;
			int indexATK = (int)State::__ATK_NEAR_BEGIN + (rand() % (numATK - 1)) + 1;
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
		case State::ATK_NEAR_RUSH:
		{
			m_state = State::ATK_RUSH;
			m_animator->ATK_RUSH_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_JUMP:
		{
			m_animator->ATK_JUMP_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_ELECTRIC:
		case State::ATK_NEAR_ELECTRIC:
		{
			m_state = State::ATK_ELECTRIC;
			m_animator->ATK_ELECTRIC_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_DOWNSTRIKE:
		{
			m_animator->ATK_DOWNSTRIKE_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_BEAM:
		case State::ATK_NEAR_BEAM:
		{
			m_state = State::ATK_BEAM;
			m_animator->ATK_BEAM_TProperty->SetTriggerState();
		}
		break;
		//case State::__ATK_FAR_BEGIN:
		//case State::__ATK_FAR_END:
		//case State::__ATK_NEAR_BEGIN:
		//case State::__ATK_NEAR_END:
		//{
		//	SetState(State::ATK_RUSH);
		//}
		//break;
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
		case State::ATK_RUSH:
		case State::ATK_ELECTRIC:
		case State::ATK_BEAM:
		{
			m_farATKCounter = ATK_FAR_DELAY;
		}
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

bool BossAncientKing::IsFarATKCondition() const
{
	if (m_farATKCounter > 0.0f)
		return false;

	if (XZDistanceBetweenPlayer() < ATK_NEAR_MAX_DIST)
		return false;

	if (XZAngleBetweenPlayer() > ATK_FAR_MAX_ANGLE)
		return false;

	return true;
}

bool BossAncientKing::IsNearATKCondition() const
{
	if (XZDistanceBetweenPlayer() > ATK_NEAR_MAX_DIST)
		return false;

	if (XZAngleBetweenPlayer() > ATK_NEAR_MAX_ANGLE)
		return false;

	return true;
}

bool BossAncientKing::IsSATKTurnCondition() const
{
	return !m_usedSATK_TURN;
}
