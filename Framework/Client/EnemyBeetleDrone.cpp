#include "stdafx.h"
#include "EnemyBeetleDrone.h"
#include "EnemyBeetleDroneAnimator.h"
#include "Player.h"
#include "Config.h"
#include "ParticleDust01.h"
#include "EventSystem.h"

void EnemyBeetleDrone::Awake()
{
	Enemy::Awake();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupAttackTrigger();
	SetupAudioSource();

	UnregistEnemy(this);
	CCT->radius = 0.8f;
	CCT->height = 0.675f;
	//CCT->height = 0.01f; // Bug mode

	SetState(State::HIDE);
}

void EnemyBeetleDrone::Start()
{
	Enemy::Start();
}

void EnemyBeetleDrone::Update()
{
	Enemy::Update();

	if (!m_appeared && DistanceBetweenPlayer() < 5.0f)
	{
		SetState(State::APPEAR);
		m_appeared = true;
	}

	if (!m_appeared)
		return;

	bool airAction = m_animator->IsPlayingAirAction();
	CCT->useGravity = !airAction;
	if (m_antiGravityCounter > 0.0f)
	{
		m_antiGravityCounter -= system->time->deltaTime;
		CCT->useGravity = false;
	}

	StateUpdate();

	AttackTriggerQuery();
}

void EnemyBeetleDrone::LateUpdate()
{
	Enemy::LateUpdate();
	UpdateCCT();
	UpdateAttachmentObjects();
}

void EnemyBeetleDrone::FixedUpdate()
{
	Enemy::FixedUpdate();
	m_animator->HasGroundBProperty->valueAsBool = CCT->isGrounded;
}

void EnemyBeetleDrone::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = ADJUST_BEETLEDRONE_LOCALPOSITION;
	m_goCharacterRender->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
	m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
	m_characterRenderer->mesh = system->resource->Find(MESH_BEETLE_DRONE);
	m_characterRenderer->SetupStandardMaterials();

	m_rightFoot1Node = m_characterRenderer->GetNodeTransformByName(TEXT("RightFoot1"));
	m_leftFoot1Node = m_characterRenderer->GetNodeTransformByName(TEXT("LeftFoot1"));
}

void EnemyBeetleDrone::SetupAnimator()
{
	m_animator = m_goCharacterRender->AddComponent<EnemyBeetleDroneAnimator>();
	m_animator->OnBeginChanging += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)>(this, &EnemyBeetleDrone::OnBeginChanging);
	m_animator->OnEndChanged += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)>(this, &EnemyBeetleDrone::OnEndChanged);
	m_animator->OnAnimationEvent += func<void(Ref<AnimatorLayer>, const AnimationEventDesc&)>(this, &EnemyBeetleDrone::OnAnimationEvent);
}

void EnemyBeetleDrone::SetupAttackTrigger()
{
	m_goAttackTrigger[RH_TRIGGER] = CreateGameObjectToChild(transform);
	m_goAttackTrigger[RH_TRIGGER]->transform->localPosition = V3(0, 0, 0);
	m_attackTrigger[RH_TRIGGER] = m_goAttackTrigger[RH_TRIGGER]->AddComponent<SphereCollider>();
	m_attackTrigger[RH_TRIGGER]->radius = 1.0f;
	m_attackTrigger[RH_TRIGGER]->isTrigger = true;
	m_attackTrigger[RH_TRIGGER]->enable = false;

	m_goAttackTrigger[LH_TRIGGER] = CreateGameObjectToChild(transform);
	m_goAttackTrigger[LH_TRIGGER]->transform->localPosition = V3(0, 0, 0);
	m_attackTrigger[LH_TRIGGER] = m_goAttackTrigger[LH_TRIGGER]->AddComponent<SphereCollider>();
	m_attackTrigger[LH_TRIGGER]->radius = 1.0f;
	m_attackTrigger[LH_TRIGGER]->isTrigger = true;
	m_attackTrigger[LH_TRIGGER]->enable = false;
}

void EnemyBeetleDrone::SetupAudioSource()
{
	m_audioSource = gameObject->AddComponent<AudioSource>();
	m_audioSource->minDistance = 3.0f;
	m_audioSource->maxDistance = 30.0f;
}

void EnemyBeetleDrone::UpdateCCT()
{
	if (CCT->isGrounded)
	{
		CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
	}
}

void EnemyBeetleDrone::UpdateAttachmentObjects()
{
	m_goAttackTrigger[RH_TRIGGER]->transform->position = m_rightFoot1Node->position;
	m_goAttackTrigger[RH_TRIGGER]->transform->rotation = m_rightFoot1Node->rotation;

	m_goAttackTrigger[LH_TRIGGER]->transform->position = m_leftFoot1Node->position;
	m_goAttackTrigger[LH_TRIGGER]->transform->rotation = m_leftFoot1Node->rotation;
}

void EnemyBeetleDrone::AttackTriggerQuery()
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
			damage.Guardable = true;
			switch (m_attackType)
			{
				case EnemyBeetleDroneAnimator::UIntContext::ATK_LIGHT:
					damage.Type = DamageInType::LIGHT;
					damage.Damage = 1.0f;
					break;
				case EnemyBeetleDroneAnimator::UIntContext::ATK_HEAVY:
					damage.Type = DamageInType::HEAVY;
					damage.Damage = 2.5f;
					break;
				case EnemyBeetleDroneAnimator::UIntContext::ATK_BLOW:
					damage.Type = DamageInType::BLOW;
					damage.Damage = 3.0f;
					break;
				case EnemyBeetleDroneAnimator::UIntContext::ATK_BLOWUP:
					damage.Damage = 2.0f;
					damage.Type = DamageInType::BLOWUP;
					break;
				case EnemyBeetleDroneAnimator::UIntContext::ATK_BLOWDOWN:
					damage.Damage = 3.0f;
					damage.Type = DamageInType::BLOWDOWN;
					break;
			}
			player->Damage(damage);
		}
	}
}

void EnemyBeetleDrone::OffAttackTriggers()
{
	for (int i = 0; i < MAX_TRIGGERS; ++i)
		m_attackTrigger[i]->enable = false;
}

void EnemyBeetleDrone::ClearHitBuffer()
{
	if (!m_hitBuffer.empty())
		m_hitBuffer.clear();
}

void EnemyBeetleDrone::OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing)
{
	if (changing.GetPointer() == m_animator->DMG_STD_AIR)
	{
		//CCT->velocity = V3::up() * 3.15f * CCT->gravityScale;
		CCT->velocity = V3::zero();
		m_antiGravityCounter = 0.7f;
	}
}

void EnemyBeetleDrone::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
	if (prev->name.find(TEXT("ATK")) != tstring::npos)
	{
		OffAttackTriggers();
		ClearHitBuffer();

		if (endChanged.GetPointer() == m_animator->BH_STD_IDLE)
		{
			SetState(State::IDLE);
		}
	}

	if (prev->name.find(TEXT("DMG")) != tstring::npos)
	{
		if (endChanged.GetPointer() == m_animator->BH_STD_IDLE)
		{
			SetState(State::IDLE);
		}
	}

	if (prev.GetPointer() == m_animator->ETC_APPEAR)
	{
		SetState(State::IDLE);
	}

	if (endChanged.GetPointer() == m_animator->STD_ATK3A ||
		endChanged.GetPointer() == m_animator->STD_ATK3B)
	{
		m_animator->ATK3TypeIProperty->valueAsInt = 1 - m_animator->ATK3TypeIProperty->valueAsInt;
	}
}

void EnemyBeetleDrone::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
	if (desc.ContextInt & EnemyBeetleDroneAnimator::IntContext::RH_START)
	{
		m_attackTrigger[RH_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & EnemyBeetleDroneAnimator::IntContext::RH_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & EnemyBeetleDroneAnimator::IntContext::LH_START)
	{
		m_attackTrigger[LH_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & EnemyBeetleDroneAnimator::IntContext::LH_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}

	if (desc.ContextByte & EnemyBeetleDroneAnimator::ByteContext::FOOT_DUST)
	{
		ParticleDust01::CreateWithNormal(
			gameObject->regionScene,
			CCT->footPosition - transform->forward,
			V3::up(), 0.0f, 30.0f, 1.0f,
			4.0f, 8.0f,
			0.5f, 2.0f,
			0.5f, 6.0f, 0.5f,
			Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
			30);
	}
	if (desc.ContextByte & EnemyBeetleDroneAnimator::ByteContext::PLAY_SOUND)
	{
		ResourceRef<AudioClip> audioClip = system->resource->Find(desc.ContextTStr);
		m_audioSource->PlayOneshot(audioClip, desc.ContextFloat);
	}
}

void EnemyBeetleDrone::SetAttackType(uint contextUInt)
{
	m_attackType = EnemyBeetleDroneAnimator::UIntContext::ATK_LIGHT;

	if (contextUInt & EnemyBeetleDroneAnimator::UIntContext::ATK_LIGHT)
		m_attackType = EnemyBeetleDroneAnimator::UIntContext::ATK_LIGHT;
	if (contextUInt & EnemyBeetleDroneAnimator::UIntContext::ATK_HEAVY)
		m_attackType = EnemyBeetleDroneAnimator::UIntContext::ATK_HEAVY;
	if (contextUInt & EnemyBeetleDroneAnimator::UIntContext::ATK_BLOW)
		m_attackType = EnemyBeetleDroneAnimator::UIntContext::ATK_BLOW;
	if (contextUInt & EnemyBeetleDroneAnimator::UIntContext::ATK_BLOWUP)
		m_attackType = EnemyBeetleDroneAnimator::UIntContext::ATK_BLOWUP;
	if (contextUInt & EnemyBeetleDroneAnimator::UIntContext::ATK_BLOWDOWN)
		m_attackType = EnemyBeetleDroneAnimator::UIntContext::ATK_BLOWDOWN;
}

void EnemyBeetleDrone::SetState(EnemyBeetleDrone::State state)
{
	if (state == m_state)
		return;

	State before = m_state;
	State next = state;
	m_state = state;
	StateEnded(before, next);
	StateChanged(before, next);
}

void EnemyBeetleDrone::StateUpdate()
{
	switch (m_state)
	{
		case State::IDLE:
		{
			m_idleLeftCount -= system->time->deltaTime;
			if (m_idleLeftCount <= 0.0f)
			{
				uint r = rand() % 2;
				if (r == 0)
					SetState(State::TRACE);
				else
					SetState(State::MOVEAROUND);
			}

			if (DistanceBetweenPlayer() < 4.0f)
			{
				SetState(State::ATK);
			}
		}
		break;
		case State::TRACE:
		{
			m_moveLeftCount -= system->time->deltaTime;
			if (m_moveLeftCount <= 0.0f)
			{
				SetState(State::IDLE);
			}

			RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 90.0f, system->time->deltaTime);

			if (DistanceBetweenPlayer() < 4.0f)
			{
				SetState(State::ATK);
			}
		}
		break;
		case State::MOVEAROUND:
		{
			m_moveLeftCount -= system->time->deltaTime;
			if (m_moveLeftCount <= 0.0f)
			{
				SetState(State::IDLE);
			}

			float moveAroundRadian = m_moveAroundAngle * Deg2Rad;
			V3 toMoveAroundDir = V3(Cos(moveAroundRadian), 0.0f, Sin(moveAroundRadian));
			RotateOnYAxisToDirection(toMoveAroundDir, 90.0f, system->time->deltaTime);

			if (DistanceBetweenPlayer() < 4.0f)
			{
				SetState(State::ATK);
			}
		}
		break;
		case State::ATK:
		{
			RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 90.0f, system->time->deltaTime);

			m_animator->KeepATKBProperty->valueAsBool = DistanceBetweenPlayer() < 5.0f;
		}
		case State::DMG:
		{
			if (m_animator->DamageDirectionFProperty->valueAsFloat == 0.0f && m_animator->IsPlayingDamageDuringLookPlayerAnimation())
			{
				RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 180.0f, system->time->deltaTime);
			}
		}
		break;
	}
}

void EnemyBeetleDrone::StateChanged(EnemyBeetleDrone::State before, EnemyBeetleDrone::State next)
{
	switch (next)
	{
		case State::HIDE:
		{
			m_characterRenderer->enable = false;
		}
		break;
		case State::APPEAR:
		{
			m_characterRenderer->enable = true;

			PhysicsRay ray;
			ray.Direction = V3::down();
			ray.Length = 100.0f;
			ray.Point = CCT->footPosition + V3::up() * 0.1f;
			PhysicsHit hit;
			if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider, CCT->rigidbody))
			{
				CCT->footPosition = hit.Point;
			}
			m_animator->Layer->Play(m_animator->ETC_APPEAR);

			if (player)
			{
				transform->forward = ToPlayerDirectionXZ();
			}

			RegistEnemy(this);
		}
		break;
		case State::IDLE:
		{
			m_idleLeftCount = 1.5f;
		}
		break;
		case State::TRACE:
		{
			m_moveLeftCount = 5.0f;
			m_animator->MoveBProperty->valueAsBool = true;
		}
		break;
		case State::MOVEAROUND:
		{
			m_moveAroundAngle = float(rand() % 361);
			m_moveLeftCount = 1.0f;
			m_animator->MoveBProperty->valueAsBool = true;
		}
		break;
		case State::ATK:
		{
			m_animator->ATKTProperty->SetTriggerState();
		}
		break;
		case State::DMG:
		{

		}
		break;
		case State::DIE:
		{
			m_animator->Layer->OffAllTriggers();
			m_animator->DamageTypeIProperty->valueAsInt = 2;
			m_animator->DamageTProperty->SetTriggerState();

			CCT->collisionWithCCT = false;
			CCT->capsuleCollider->SetIgnoreLayerIndex(PhysicsLayer_Player, true);
			CCT->capsuleCollider->SetIgnoreLayerIndex(PhysicsLayer_Enemy, true);
			EventSystem::Notify(EVENT_ENEMY_DIE, this);
			Enemy::UnregistEnemy(this);
		}
		break;
		case State::DIE_END:
		{
			CCT->enable = false;
			CCT->rigidbody->enable = false;
		}
		break;
	}
}

void EnemyBeetleDrone::StateEnded(EnemyBeetleDrone::State before, EnemyBeetleDrone::State current)
{
	switch (before)
	{
		case State::TRACE:
		case State::MOVEAROUND:
		{
			m_moveLeftCount = 0.0f;
			m_animator->MoveBProperty->valueAsBool = false;
		}
		break;
		case State::ATK:
		{
			m_animator->KeepATKBProperty->valueAsBool = false;
		}
		break;
	}
}

float EnemyBeetleDrone::GetMaxHP() const
{
	return 11.0f;
}

float EnemyBeetleDrone::GetHP() const
{
	return m_hp;
}

void EnemyBeetleDrone::SetHP(float value)
{
	m_hp = value;
}

V3 EnemyBeetleDrone::GetDirection() const
{
	return transform->forward;
}

void EnemyBeetleDrone::SetDirection(const V3& direction)
{
	transform->forward = direction;
}

bool EnemyBeetleDrone::IsGuarding() const
{
	return m_animator->IsPlayingGuardableAnimation();
}

bool EnemyBeetleDrone::IsGuardBreakableByBackattack() const
{
	return true;
}

bool EnemyBeetleDrone::IsInvisible() const
{
	return false;
}

bool EnemyBeetleDrone::IsSuperarmor() const
{
	return false;
}

DamageOutType EnemyBeetleDrone::OnDamage(const DamageOut& out)
{
	if (m_state == State::DIE || m_state == State::DIE_END || m_state == State::NONE)
		return DamageOutType::IGNORED;

	switch (out.Type)
	{
		case DamageOutType::GUARDED:
		{
			m_animator->GuardHitTProperty->SetTriggerState();
			return DamageOutType::GUARDED;
		}
		break;
		case DamageOutType::GUARD_BREAKED:
		{
			m_animator->GuardBreakTProperty->SetTriggerState();
			return DamageOutType::GUARD_BREAKED;
		}
		break;
		case DamageOutType::HIT:
		{
			m_animator->DamageTProperty->SetTriggerState();
			m_animator->DamageDirectionFProperty->valueAsFloat = out.Backattack ? 1.0f : 0.0f;
			switch (out.In.Type)
			{
				case DamageInType::LIGHT:
					m_animator->DamageTypeIProperty->valueAsInt = 0;
					break;
				case DamageInType::HEAVY:
					m_animator->DamageTypeIProperty->valueAsInt = 1;
					break;
				case DamageInType::BLOW:
					m_animator->DamageTypeIProperty->valueAsInt = 2;
					break;
				case DamageInType::BLOWUP:
					m_animator->DamageTypeIProperty->valueAsInt = 3;
					m_animator->DamageDirectionFProperty->valueAsFloat = 0.0f;
					break;
				case DamageInType::BLOWDOWN:
					m_animator->DamageTypeIProperty->valueAsInt = 2;
					m_animator->DamageDirectionFProperty->valueAsFloat = 0.0f;
					break;
			}
			if (out.In.SetVelocity)
			{
				CCT->Jump(out.In.Velocity);
			}
			hp -= out.In.Damage;
			m_animator->HPFProperty->valueAsFloat = hp;
			if (hp <= 0.0f)
			{
				SetState(State::DIE);
			}
			else
			{
				SetState(State::DMG);
			}
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

void EnemyBeetleDrone::Appear()
{
	SetState(State::APPEAR);
	m_appeared = true;
}
