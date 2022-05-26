#include "stdafx.h"
#include "EnemyBeetleDrone.h"
#include "EnemyBeetleDroneAnimator.h"
#include "Player.h"
#include "Config.h"

void EnemyBeetleDrone::Awake()
{
	Enemy::Awake();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupAttackTrigger();

	CCT->radius = 0.8f;
	CCT->height = 0.675f;
	//CCT->height = 0.01f; // Bug mode

	m_characterRenderer->enable = false;
	CCT->rigidbody->enable = false;
}

void EnemyBeetleDrone::Start()
{
	Enemy::Start();
	//DoAppear();
}

void EnemyBeetleDrone::Update()
{
	Enemy::Update();

	if (DistanceBetweenPlayer() < 5.0f)
		DoAppear();

	if (!m_appeared)
		return;

	bool airAction = m_animator->IsPlayingAirAction();

	CCT->useGravity = !airAction;

	if (!isDead)
	{
		m_animator->MoveBProperty->valueAsBool = true;
		RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 90.0f, system->time->deltaTime);

		if (DistanceBetweenPlayer() < 3.0f)
		{
			m_animator->ATKTProperty->SetTriggerState();
			RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 90.0f, system->time->deltaTime);
		}
	}

	if (isDead && CCT->isGrounded)
	{
		CCT->enable = false;
		CCT->rigidbody->enable = false;
	}

	if (m_animator->DamageDirectionFProperty->valueAsFloat == 0.0f && m_animator->IsPlayingDamageDuringLookPlayerAnimation())
	{
		RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 180.0f, system->time->deltaTime);
	}

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
	m_goCharacterRender->transform->localPosition = V3(0, -1.0f, 0);
	m_goCharacterRender->transform->localEulerAngles = V3(90, 180, 0);
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
					damage.Velocity = transform->forward * 5.0f + V3(0, 5, 0);
					damage.SetVelocity = true;
					break;
				case ANIM_ATK_BLOWUP:
					damage.Damage = 2.0f;
					damage.Type = DamageInType::BLOWUP;
					damage.Velocity = BLOWUP_VELOCITY;
					damage.SetVelocity = true;
					break;
				case ANIM_ATK_BLOWDOWN:
					damage.Damage = 3.0f;
					damage.Type = DamageInType::BLOWDOWN;
					damage.Velocity = BLOWDOWN_VELOCITY;
					damage.SetVelocity = true;
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
		CCT->velocity = V3::up() * 5.0f;
}

void EnemyBeetleDrone::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
	if (prev->name.find(TEXT("ATK")) != tstring::npos)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}

	if (endChanged.GetPointer() == m_animator->STD_ATK3A ||
		endChanged.GetPointer() == m_animator->STD_ATK3B)
	{
		m_animator->ATK3TypeIProperty->valueAsInt = 1 - m_animator->ATK3TypeIProperty->valueAsInt;
	}
}

void EnemyBeetleDrone::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
	if (desc.ContextInt & ANIM_ATK_RH_START)
	{
		m_attackTrigger[RH_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	else if (desc.ContextInt & ANIM_ATK_RH_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & ANIM_ATK_LH_START)
	{
		m_attackTrigger[LH_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	else if (desc.ContextInt & ANIM_ATK_LH_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
}

void EnemyBeetleDrone::SetAttackType(int contextInt)
{
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
	else
		m_attackType = ANIM_ATK_LIGHT;
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
	if (isDead)
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
				//CCT->velocity = desc.Velocity;
				CCT->Jump(out.In.Velocity);
			}
			hp -= out.In.Damage;
			m_animator->HPFProperty->valueAsFloat = hp;
			if (hp <= 0.0f)
			{
				m_animator->DamageTypeIProperty->valueAsInt = 2;
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

void EnemyBeetleDrone::DoAppear()
{
	if (m_appeared)
		return;

	m_appeared = true;
	m_characterRenderer->enable = true;
	CCT->rigidbody->enable = true;
	PhysicsRay ray;
	ray.Direction = V3::down();
	ray.Length = 100.0f;
	ray.Point = CCT->footPosition;
	PhysicsHit hit;
	if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider, CCT->rigidbody))
	{
		CCT->footPosition = hit.Point;
	}
	m_animator->Layer->Play(m_animator->ETC_APPEAR);
}
