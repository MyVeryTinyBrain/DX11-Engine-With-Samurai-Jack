#include "stdafx.h"
#include "Player.h"
#include "TPSCamera.h"
#include "PlayerAnimator.h"

void Player::Awake()
{
	Character::Awake();

	SetupTPSCamera();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupKatanaSheet();
	SetupKatana();
	SetupFootCnt();
	SetupAttackTrigger();
}

void Player::Update()
{
	UpdateKeyTimes();

	V3 translation = GetTranslateDirection();

	RotateOnYAxisToDirection(translation, system->time->deltaTime);

	float targetDashState = float(system->input->GetKey(Key::LShift) && IsLongKeyPressing(m_lShiftPressingTime));
	m_animator->DashStateFProperty->valueAsFloat = Lerp(m_animator->DashStateFProperty->valueAsFloat, targetDashState, system->time->deltaTime * 10.0f);

	if (!m_animator->Layer->IsPlaying(m_animator->BH_RUN_BH_DASH))
		m_animator->DashStateFProperty->valueAsFloat = 0.0f;

	bool isAirAction =
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_X) ||
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_XX) ||
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_XXX) ||
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_START) ||
		m_animator->Layer->IsPlaying(m_animator->ATK_XY);

	CCT->useGravity = !isAirAction;

	if (CCT->isGrounded)
	{
		m_animator->MoveStateFProperty->valueAsFloat = translation.magnitude > 0.0f;
	}
	else if (!isAirAction)
	{
		V3 xzTranslation = translation;
		xzTranslation.y = 0;
		xzTranslation.Normalize();
		CCT->Move(translation * 5.0f * system->time->deltaTime, system->time->deltaTime);
	}

	if (system->input->GetKeyDown(Key::Space))
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

	if (system->input->GetKeyUp(Key::LShift) && IsShortKeyPressed(m_lShiftPressedTime))
	{
		m_animator->RollTProperty->SetTriggerState();
	}

	bool isPlayingJump =
		m_animator->Layer->IsPlaying(m_animator->BH_JUMP) ||
		m_animator->Layer->IsPlaying(m_animator->BH_AIR_JUMP) ||
		m_animator->Layer->IsPlaying(m_animator->BH_LAND);

	bool isPlayingGuard =
		m_animator->Layer->IsPlaying(m_animator->GAD_LOOP) ||
		m_animator->Layer->IsPlaying(m_animator->GAD_START) ||
		m_animator->Layer->IsPlaying(m_animator->GAD_HIT) ||
		m_animator->Layer->IsPlaying(m_animator->GAD_BREAK);

	if (system->input->GetKeyDown(Key::RightMouse))
	{
		m_animator->LastDashStateFProperty->valueAsFloat = m_animator->DashStateFProperty->valueAsFloat > 0.4f ? 1.0f : 0.0f;
		m_animator->DashStateFProperty->valueAsFloat = 0.0f;
	}
	m_animator->GuardStateBProperty->valueAsBool = system->input->GetKey(Key::RightMouse) && !isPlayingJump;
	m_animator->IsPlayingGuardBProperty->valueAsBool = isPlayingGuard;

	if (system->input->GetKeyDown(Key::One))
		m_animator->GuardHitTProperty->SetTriggerState();
	if (system->input->GetKeyDown(Key::Two))
		m_animator->GuardBreakTProperty->SetTriggerState();

	if (system->input->GetKeyDown(Key::Three))
	{
		m_animator->DamageTProperty->SetTriggerState();
		m_animator->DamageTypeIProperty->valueAsInt = 0;
	}
	if (system->input->GetKeyDown(Key::Four))
	{
		m_animator->DamageTProperty->SetTriggerState();
		m_animator->DamageTypeIProperty->valueAsInt = 1;
	}
	if (system->input->GetKeyDown(Key::Five))
	{
		m_animator->DamageTProperty->SetTriggerState();
		m_animator->DamageTypeIProperty->valueAsInt = 2;
	}

	if (m_animator->Layer->IsPlaying(m_animator->DMG_BLOW_LOOP))
	{
		CCT->Move(-transform->forward * 5.0f * system->time->deltaTime, system->time->deltaTime);
	}

		m_animator->LightAttackTProperty->OffTriggerState();
		m_animator->LightAttackComboTProperty->OffTriggerState();
		m_animator->HeavyAttackTProperty->OffTriggerState();
		m_animator->HeavyAttackComboTProperty->OffTriggerState();
		m_animator->LightToHeavtAttackTProperty->OffTriggerState();

		bool isPlayingHeavyAttack =
			m_animator->Layer->IsPlaying(m_animator->ATK_Y) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_YY) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_YYY) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_XY) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_XXXXY) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_XXXXXY) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_START) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_LOOP) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_END) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_ONDASH_UPPER);

	if (system->input->GetKeyUp(Key::LeftMouse) && IsShortKeyPressed(m_leftMousePressedTime) && !isPlayingHeavyAttack)
	{
		bool first =
			!m_animator->Layer->IsPlaying(m_animator->ATK_X) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_XX) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_XXX) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_XXXX) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_XXXXX) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_AIR_X) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_AIR_XX) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_AIR_XXX);

		if (first)
			m_animator->LightAttackTProperty->SetTriggerState();
		else
			m_animator->LightAttackComboTProperty->SetTriggerState();

		m_animator->LastDashStateFProperty->valueAsFloat = m_animator->DashStateFProperty->valueAsFloat > 0.4f ? 1.0f : 0.0f;
	}
	else if (system->input->GetKey(Key::LeftMouse) && IsLongKeyPressing(m_leftMousePressingTime))
	{
		bool first =
			!m_animator->Layer->IsPlaying(m_animator->ATK_Y) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_YY) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_YYY) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_XY) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_XXXXY) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_XXXXXY) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_START) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_LOOP) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_END) &&
			!m_animator->Layer->IsPlaying(m_animator->ATK_ONDASH_UPPER);

		bool fromX =
			m_animator->Layer->IsPlaying(m_animator->ATK_X) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_XX) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_XXXX) ||
			m_animator->Layer->IsPlaying(m_animator->ATK_XXXXX);

		if (first)
			m_animator->HeavyAttackTProperty->SetTriggerState();
		else if (fromX)
			m_animator->LightToHeavtAttackTProperty->SetTriggerState();
		else
			m_animator->HeavyAttackComboTProperty->SetTriggerState();

		m_animator->LastDashStateFProperty->valueAsFloat = m_animator->DashStateFProperty->valueAsFloat > 0.4f ? 1.0f : 0.0f;
	}

	if (m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_LOOP))
	{
		CCT->Move(V3::down() * 40.0f * system->time->deltaTime, system->time->deltaTime);
	}
}

void Player::FixedUpdate()
{
	m_animator->HasGroundBProperty->valueAsBool = CCT->isGrounded;
}

void Player::LateUpdate()
{
	UpdateCCT();
	UpdateAttachmentObjects();
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
	m_animator->OnEndChanged += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)>(this, &Player::OnEndChanged);
	m_animator->OnAnimationEvent += func<void(Ref<AnimatorLayer>, const string&)>(this, &Player::OnAnimationEvent);
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
	m_goAttackTrigger[FORWARD_TRIGGER] = CreateGameObjectToChild(transform);
	m_goAttackTrigger[FORWARD_TRIGGER]->transform->localPosition = V3(0, 0, 1.2f);
	m_attackTrigger[FORWARD_TRIGGER] = m_goAttackTrigger[FORWARD_TRIGGER]->AddComponent<SphereCollider>();
	m_attackTrigger[FORWARD_TRIGGER]->radius = 1.5f;
	m_attackTrigger[FORWARD_TRIGGER]->isTrigger = true;
	m_attackTrigger[FORWARD_TRIGGER]->enable = false;

	m_goAttackTrigger[KATANA_TRIGGER] = CreateGameObjectToChild(m_katanaTrailRenderer->transform);
	m_goAttackTrigger[KATANA_TRIGGER]->transform->localPosition = V3(0/*forward*/, 0, 0);
	m_attackTrigger[KATANA_TRIGGER] = m_goAttackTrigger[KATANA_TRIGGER]->AddComponent<SphereCollider>();
	m_attackTrigger[KATANA_TRIGGER]->radius = 1.5f;
	m_attackTrigger[KATANA_TRIGGER]->isTrigger = true;
	m_attackTrigger[FORWARD_TRIGGER]->enable = false;

	m_goAttackTrigger[FOOT_TRIGGER] = CreateGameObjectToChild(m_footTrailRenderer->transform);
	m_goAttackTrigger[FOOT_TRIGGER]->transform->localPosition = V3(0, 0/*back*/, 0);
	m_attackTrigger[FOOT_TRIGGER] = m_goAttackTrigger[FOOT_TRIGGER]->AddComponent<SphereCollider>();
	m_attackTrigger[FOOT_TRIGGER]->radius = 1.5f;
	m_attackTrigger[FOOT_TRIGGER]->isTrigger = true;
	m_attackTrigger[FORWARD_TRIGGER]->enable = false;
}

void Player::UpdateCCT()
{
	CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
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
}

void Player::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
	if (prev->name.find(TEXT("ATK")) != tstring::npos)
	{
		m_katanaTrailRenderer->autoTrail = false;
		m_footTrailRenderer->autoTrail = false;
	}
}

void Player::OnAnimationEvent(Ref<AnimatorLayer> layer, const string& context)
{
	if (context == ATK_KT_START)
		m_katanaTrailRenderer->autoTrail = true;
	else if (context == ATK_KT_END)
		m_katanaTrailRenderer->autoTrail = false;
	else if (context == ATK_FOOT_START)
		m_footTrailRenderer->autoTrail = true;
	else if (context == ATK_FOOT_END)
		m_footTrailRenderer->autoTrail = false;
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
