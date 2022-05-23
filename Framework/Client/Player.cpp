#include "stdafx.h"
#include "Player.h"
#include "TPSCamera.h"
#include "PlayerAnimator.h"
#include "Config.h"
#include "Enemy.h"

#define BLOWUP_VELOCITY		V3(0,17,0)
#define BLOWDOWN_VELOCITY	V3(0,-40,0)

Player* Player::g_player = nullptr;

void Player::Awake()
{
	Character::Awake();

	CCT->capsuleCollider->layerIndex = PhysicsLayer_Player;

	SetupTPSCamera();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupKatanaSheet();
	SetupKatana();
	SetupFootCnt();
	SetupAttackTrigger();

	g_player = this;
}

void Player::Update()
{
	Character::Update();

	UpdateKeyTimes();

	V3 translation = GetTranslateDirection();

	RotateOnYAxisToDirection(translation, 720.0f, system->time->deltaTime);

	float targetDashState = float(system->input->GetKey(Key::LShift) && IsLongKeyPressing(m_lShiftPressingTime));
	m_animator->DashStateFProperty->valueAsFloat = Lerp(m_animator->DashStateFProperty->valueAsFloat, targetDashState, system->time->deltaTime * 10.0f);

	if (!m_animator->Layer->IsPlaying(m_animator->BH_RUN_BH_DASH))
		m_animator->DashStateFProperty->valueAsFloat = 0.0f;

	bool airAction = m_animator->IsPlayingAirAction();

	CCT->useGravity = !airAction;

	if (CCT->isGrounded)
	{
		m_animator->MoveStateFProperty->valueAsFloat = translation.magnitude > 0.0f;
	}
	else if (!airAction)
	{
		V3 xzTranslation = translation;
		xzTranslation.y = 0;
		xzTranslation.Normalize();
		CCT->Move(translation * 5.0f * system->time->deltaTime, system->time->deltaTime);
	}

	JumpInput();

	if (system->input->GetKeyUp(Key::LShift) && IsShortKeyPressed(m_lShiftPressedTime))
	{
		m_animator->RollTProperty->SetTriggerState();
	}


	GuardInput();

	if (m_animator->Layer->IsPlaying(m_animator->DMG_BLOW_LOOP))
	{
		CCT->Move(-transform->forward * 5.0f * system->time->deltaTime, system->time->deltaTime);
	}

	AttackInput();

	if (m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_LOOP))
	{
		CCT->Move(V3::down() * 40.0f * system->time->deltaTime, system->time->deltaTime);
	}

	AttackTriggerQuery();
}

void Player::FixedUpdate()
{
	Character::FixedUpdate();
	m_animator->HasGroundBProperty->valueAsBool = CCT->isGrounded;
}

void Player::LateUpdate()
{
	Character::LateUpdate();
	UpdateCCT();
	UpdateAttachmentObjects();
}

void Player::OnDestroyed()
{
	Character::OnDestroyed();
}

void Player::SetupTPSCamera()
{
	GameObject* goTPSCamera = CreateGameObject();
    m_tpsCamera = goTPSCamera->AddComponent<TPSCamera>();
	m_tpsCamera->target = transform;
	system->graphic->cameraManager->mainCamera = m_tpsCamera->camera;
}

void Player::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = V3(0, -1, 0);
	m_goCharacterRender->transform->localEulerAngles = V3(90, 180, 0);
	m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
	m_characterRenderer->mesh = system->resource->Find(TEXT("../Resource/Jack/Jack.FBX"));
	m_characterRenderer->SetupStandardMaterials();

	m_rightHandWeaponCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));
	m_scabbardCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("Scabbard_cnt_tr"));
	m_leftToeBaseNode = m_characterRenderer->GetNodeTransformByName(TEXT("LeftToeBase"));
}

void Player::SetupAnimator()
{
	m_animator = m_goCharacterRender->AddComponent<PlayerAnimator>();
	m_animator->OnBeginChanging += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)>(this, &Player::OnBeginChanging);
	m_animator->OnEndChanged += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)>(this, &Player::OnEndChanged);
	m_animator->OnAnimationEvent += func<void(Ref<AnimatorLayer>, const AnimationEventDesc&)>(this, &Player::OnAnimationEvent);
}

void Player::SetupKatanaSheet()
{
	m_goKatanaSheathRenderer = CreateGameObjectToChild(transform);
	m_katanaSheathRenderer = m_goKatanaSheathRenderer->AddComponent<MeshRenderer>();
	m_katanaSheathRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));
	m_katanaSheathRenderer->SetupStandardMaterials();
}

void Player::SetupKatana()
{
	m_goKatana = CreateGameObjectToChild(transform);

	m_goKatanaRenderer = CreateGameObjectToChild(m_goKatana->transform);
	m_katanaRenderer = m_goKatanaRenderer->AddComponent<MeshRenderer>();
	m_katanaRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/Katana.FBX"));
	m_katanaRenderer->SetupStandardMaterials();

	m_goKatanaTrail = CreateGameObjectToChild(m_goKatana->transform);
	m_goKatanaTrail->transform->localPosition = V3(0, 0, -1.0f);
	m_goKatanaTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_katanaTrailRenderer = m_goKatanaTrail->AddComponent<TrailRenderer>();
	m_katanaTrailRenderer->alignment = TrailRenderer::Alignment::Local;
	m_katanaTrailRenderer->shrinkDistance = 30.0f;
	m_katanaTrailRenderer->width = 2.0f;
	m_katanaTrailRenderer->autoTrail = false;
}

void Player::SetupFootCnt()
{
	m_goFootCnt = CreateGameObjectToChild(m_goCharacterRender->transform);
	m_footTrailRenderer = m_goFootCnt->AddComponent<TrailRenderer>();
	m_footTrailRenderer->shrinkDistance = 30.0f;
	m_footTrailRenderer->width = 0.5f;
	m_footTrailRenderer->autoTrail = false;
}

void Player::SetupAttackTrigger()
{
	m_goAttackTrigger[KATANA_TRIGGER] = CreateGameObjectToChild(m_katanaTrailRenderer->transform);
	m_goAttackTrigger[KATANA_TRIGGER]->transform->localPosition = V3(0/*forward*/, 0, 0);
	m_attackTrigger[KATANA_TRIGGER] = m_goAttackTrigger[KATANA_TRIGGER]->AddComponent<SphereCollider>();
	m_attackTrigger[KATANA_TRIGGER]->radius = 1.5f;
	m_attackTrigger[KATANA_TRIGGER]->isTrigger = true;
	m_attackTrigger[KATANA_TRIGGER]->enable = false;

	m_goAttackTrigger[FOOT_TRIGGER] = CreateGameObjectToChild(m_footTrailRenderer->transform);
	m_goAttackTrigger[FOOT_TRIGGER]->transform->localPosition = V3(0, 0/*back*/, 0);
	m_attackTrigger[FOOT_TRIGGER] = m_goAttackTrigger[FOOT_TRIGGER]->AddComponent<SphereCollider>();
	m_attackTrigger[FOOT_TRIGGER]->radius = 2.0f;
	m_attackTrigger[FOOT_TRIGGER]->isTrigger = true;
	m_attackTrigger[FOOT_TRIGGER]->enable = false;

	m_goAttackTrigger[KATANA_TRIGGER]->name = TEXT("KATANA");
	m_goAttackTrigger[FOOT_TRIGGER]->name = TEXT("FOOT");
}

void Player::UpdateCCT()
{
	if (CCT->isGrounded)
	{
		CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
	}
}

void Player::UpdateAttachmentObjects()
{
	if (m_goKatanaSheathRenderer->active)
	{
		m_goKatanaSheathRenderer->transform->position = m_scabbardCntNode->position;
		m_goKatanaSheathRenderer->transform->rotation = m_scabbardCntNode->rotation;
	}

	if (m_goKatana->active)
	{
		m_goKatana->transform->position = m_rightHandWeaponCntNode->position;
		m_goKatana->transform->rotation = m_rightHandWeaponCntNode->rotation;
	}

	if (m_goFootCnt->active)
	{
		m_goFootCnt->transform->position = m_leftToeBaseNode->position - m_leftToeBaseNode->up * 1.0f + transform->up * 0.25f;
		m_goFootCnt->transform->rotation = m_leftToeBaseNode->rotation;
	}
}

void Player::JumpInput()
{
	if (system->input->GetKeyDown(Key::Space) && !m_animator->IsPlayingNonJumpableAnimation())
	{
		if (CCT->isGrounded)
		{
			m_animator->JumpTProperty->SetTriggerState();
			CCT->Jump(V3::up() * 11.0f);
		}
		else
		{
			m_animator->AirJumpTProperty->SetTriggerState();
			CCT->Jump(V3::up() * 13.0f);
		}
	}
}

void Player::AttackInput()
{
	m_animator->LightAttackTProperty->OffTriggerState();
	m_animator->HeavyAttackTProperty->OffTriggerState();

	if (!m_animator->IsPlayingNonAttackableAnimation())
	{
		if (system->input->GetKeyUp(Key::LeftMouse) && IsShortKeyPressed(m_leftMousePressedTime))
		{
			m_animator->LightAttackTProperty->SetTriggerState();
			m_animator->LastDashStateFProperty->valueAsFloat = m_animator->DashStateFProperty->valueAsFloat > 0.4f ? 1.0f : 0.0f;
		}
		else if (system->input->GetKey(Key::LeftMouse) && IsLongKeyPressing(m_leftMousePressingTime))
		{
			m_animator->HeavyAttackTProperty->SetTriggerState();
			m_animator->LastDashStateFProperty->valueAsFloat = m_animator->DashStateFProperty->valueAsFloat > 0.4f ? 1.0f : 0.0f;
		}
	}
}

void Player::GuardInput()
{
	if (system->input->GetKeyDown(Key::RightMouse))
	{
		m_animator->DashStateFProperty->valueAsFloat = 0.0f;
	}
	m_animator->GuardStateBProperty->valueAsBool = system->input->GetKey(Key::RightMouse);
}

void Player::AttackTriggerQuery()
{
	for (int i = 0; i < MAX_TRIGGERS; ++i)
	{
		if (!m_attackTrigger[i]->enable)
			continue;

		vector<Collider*> overlaps = system->physics->query->OverlapAll(m_attackTrigger[i], 1 << PhysicsLayer_Enemy, PhysicsQueryType::Collider);
		for (auto& overlap : overlaps)
		{
			Rigidbody* rigidbody = overlap->rigidbody;
			if (!rigidbody) continue;

			Enemy* enemy = rigidbody->gameObject->GetComponent<Enemy>();
			if (!enemy) continue;

			auto result = m_hitBuffer.insert(rigidbody);
			if (!result.second) continue; // 이미 힛 버퍼에 존재합니다.

			DamageDesc desc;
			desc.FromCharacter = this;
			desc.FromDirection = enemy->transform->position - transform->position;
			switch (m_attackType)
			{
				case ANIM_ATK_LIGHT: desc.Type = DamageDesc::Type::LIGHT; break;
				case ANIM_ATK_HEAVY: desc.Type = DamageDesc::Type::HEAVY; break;
				case ANIM_ATK_BLOW: desc.Type = DamageDesc::Type::BLOW; desc.Velocity = transform->forward * 5.0f + V3(0, 5, 0); desc.SetVelocity = true; break;
				case ANIM_ATK_BLOWUP: desc.Type = DamageDesc::Type::BLOWUP; desc.Velocity = BLOWUP_VELOCITY; desc.SetVelocity = true; break;
				case ANIM_ATK_BLOWDOWN: desc.Type = DamageDesc::Type::BLOWDOWN; desc.Velocity = BLOWDOWN_VELOCITY; desc.SetVelocity = true; break;
			}
			enemy->Damage(desc);
		}
	}
}

void Player::OffAttackTriggers()
{
	for (int i = 0; i < MAX_TRIGGERS; ++i)
		m_attackTrigger[i]->enable = false;
}

void Player::ClearHitBuffer()
{
	if (!m_hitBuffer.empty())
		m_hitBuffer.clear();
}

void Player::UpdateKeyTimes()
{
	if (system->input->GetKeyDown(Key::LShift))
	{
		m_lShiftPressedTime = system->time->unscaledAccumulatedSinceStartup;
		m_lShiftPressingTime = 0.0f;
	}
	else if (system->input->GetKeyUp(Key::LShift))
	{
		m_lShiftPressingTime = 0.0f;
	}
	else if (system->input->GetKey(Key::LShift))
	{
		m_lShiftPressingTime += system->time->deltaTime;
	}
	if (system->input->GetKeyDown(Key::LeftMouse))
	{
		m_leftMousePressedTime = system->time->unscaledAccumulatedSinceStartup;
		m_leftMousePressingTime = 0.0f;
	}
	else if (system->input->GetKeyUp(Key::LeftMouse))
	{
		m_leftMousePressingTime = 0.0f;
	}
	else if (system->input->GetKey(Key::LeftMouse))
	{
		m_leftMousePressingTime += system->time->deltaTime;
	}
}

bool Player::IsShortKeyPressed(float pressedTime) const
{
	if (pressedTime == 0.0f)
		return true;

	float delta = system->time->unscaledAccumulatedSinceStartup - pressedTime;
	return delta <= 0.2f;
}

bool Player::IsLongKeyPressing(float pressingTime) const
{
	return pressingTime > 0.2f;
}

void Player::OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing)
{
	if (changing.GetPointer() == m_animator->GAD_START)
		m_animator->LastDashStateFProperty->valueAsFloat = m_animator->DashStateFProperty->valueAsFloat > 0.4f ? 1.0f : 0.0f;
}

void Player::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
	if (prev->name.find(TEXT("ATK")) != tstring::npos)
	{
		m_katanaTrailRenderer->autoTrail = false;
		m_footTrailRenderer->autoTrail = false;
		OffAttackTriggers();
		ClearHitBuffer();
	}
}

void Player::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
	if (desc.ContextInt & ANIM_ATK_KT_START)
	{
		m_katanaTrailRenderer->autoTrail = true;
		m_attackTrigger[KATANA_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	else if (desc.ContextInt & ANIM_ATK_KT_END)
	{
		m_katanaTrailRenderer->autoTrail = false;
		OffAttackTriggers();
		ClearHitBuffer();
	}
	else if (desc.ContextInt & ANIM_ATK_FOOT_START)
	{
		m_footTrailRenderer->autoTrail = true;
		m_attackTrigger[FOOT_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	else if (desc.ContextInt & ANIM_ATK_FOOT_END)
	{
		m_footTrailRenderer->autoTrail = false;
		OffAttackTriggers();
		ClearHitBuffer();
	}
	else if (desc.ContextInt & ANIM_ATK_KT_STING_START)
	{
		m_attackTrigger[KATANA_TRIGGER]->enable = true;
		SetAttackType(desc.ContextInt);
		ClearHitBuffer();
	}
	else if (desc.ContextInt & ANIM_ATK_KT_STING_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}

	if (desc.ContextInt & ANIM_JUMP)
	{
		//CCT->velocity = BLOWUP_VELOCITY;
		CCT->Jump(BLOWUP_VELOCITY);
	}
}

void Player::SetAttackType(int contextInt)
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

bool Player::IsGuarding() const
{
	return m_animator->IsPlayingGuardableAnimation();
}

DamageResult Player::Damage(const DamageDesc& desc)
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
		m_animator->GuardHitTProperty->SetTriggerState();
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
			case DamageDesc::Type::BLOWUP:
				m_animator->DamageTypeIProperty->valueAsInt = 2;
				break;
		}
		return DamageResult::HIT;
	}
}

V3 Player::GetTranslateDirection() const
{
	V3 forward = m_tpsCamera->camera->transform->forward;
	V3 right = m_tpsCamera->camera->transform->right;

	forward.y = right.y = 0;
	forward.Normalize();
	right.Normalize();

	V3 translation = V3::zero();

	if (system->input->GetKey(Key::W))
		translation += forward;
	if (system->input->GetKey(Key::S))
		translation -= forward;
	if (system->input->GetKey(Key::D))
		translation += right;
	if (system->input->GetKey(Key::A))
		translation -= right;

	return translation;
}
