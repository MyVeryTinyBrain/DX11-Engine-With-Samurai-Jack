#include "stdafx.h"
#include "EnemyBeetleDrone.h"
#include "EnemyBeetleDroneAnimator.h"
#include "Player.h"

void EnemyBeetleDrone::Awake()
{
	Enemy::Awake();
	SetupCharacterRenderers();
	SetupAnimator();

	CCT->radius = 0.8f;
	CCT->height = 0.675f;
	//CCT->height = 0.01f; // Bug mode
}

void EnemyBeetleDrone::Update()
{
	Enemy::Update();

	bool airAction = m_animator->IsPlayingAirAction();

	CCT->useGravity = !airAction;

	//m_animator->MoveBProperty->valueAsBool = true;
	//RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 90.0f, system->time->deltaTime);

	//if (DistanceBetweenPlayer() < 2.0f)
	//	m_animator->ATKTProperty->SetTriggerState();
}

void EnemyBeetleDrone::LateUpdate()
{
	Enemy::LateUpdate();
	UpdateCCT();
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
	m_characterRenderer->mesh = system->resource->Find(TEXT("../Resource/BeetleDrone/BeetleDrone.FBX"));
	m_characterRenderer->SetupStandardMaterials();
}

void EnemyBeetleDrone::SetupAnimator()
{
	m_animator = m_goCharacterRender->AddComponent<EnemyBeetleDroneAnimator>();
	m_animator->OnBeginChanging += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)>(this, &EnemyBeetleDrone::OnBeginChanging);
	m_animator->OnEndChanged += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)>(this, &EnemyBeetleDrone::OnEndChanged);
	m_animator->OnAnimationEvent += func<void(Ref<AnimatorLayer>, const AnimationEventDesc&)>(this, &EnemyBeetleDrone::OnAnimationEvent);
}

void EnemyBeetleDrone::UpdateCCT()
{
	if (CCT->isGrounded)
	{
		CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
	}
}

void EnemyBeetleDrone::OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing)
{
	if (changing.GetPointer() == m_animator->DMG_STD_AIR)
		CCT->velocity = V3::up() * 5.0f;
}

void EnemyBeetleDrone::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
}

void EnemyBeetleDrone::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
}

DamageResult EnemyBeetleDrone::Damage(const DamageDesc& desc)
{
	bool backAttack = false;
	bool guarded = false;

	float cosine = V3::Dot(transform->forward, -desc.FromDirection);

	if (cosine < 0.0f)
	{
		backAttack = true;
	}
	
	if (desc.Guardable && !backAttack)
	{
		if (isGuarding)
		{
			guarded = true;
		}
	}

	if (guarded)
	{
		//m_animator->GuardHitTProperty->SetTriggerState();
		return DamageResult::GUARDED;
	}
	else
	{
		m_animator->DamageTProperty->SetTriggerState();
		m_animator->DamageDirectionFProperty->valueAsFloat = backAttack ? 1.0f : 0.0f;
		switch (desc.Type)
		{
			case DamageDesc::Type::LIGHT:
				m_animator->DamageTypeIProperty->valueAsInt = 0;
				break;
			case DamageDesc::Type::HEAVY:
				m_animator->DamageTypeIProperty->valueAsInt = 1;
				break;
			case DamageDesc::Type::BLOW:
				m_animator->DamageTypeIProperty->valueAsInt = 2;
				break;
			case DamageDesc::Type::BLOWUP:
				m_animator->DamageTypeIProperty->valueAsInt = 3;
				m_animator->DamageDirectionFProperty->valueAsFloat = 0.0f;
				break;
			case DamageDesc::Type::BLOWDOWN:
				m_animator->DamageTypeIProperty->valueAsInt = 2;
				m_animator->DamageDirectionFProperty->valueAsFloat = 0.0f;
				break;
		}
		if (desc.SetVelocity)
		{
			//CCT->velocity = desc.Velocity;
			CCT->Jump(desc.Velocity);
		}
		return DamageResult::HIT;
	}
}
