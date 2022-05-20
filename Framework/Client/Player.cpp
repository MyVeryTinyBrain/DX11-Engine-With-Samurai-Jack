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
	SetupWeapons();
}

void Player::Update()
{
	V3 translation = GetTranslateDirection();

	RotateToDirection(translation);

	float targetDashState = float(system->input->GetKey(Key::LeftMouse));
	m_animator->DashStateFProperty->valueAsFloat = Lerp(m_animator->DashStateFProperty->valueAsFloat, targetDashState, system->time->deltaTime * 5.0f);

	if (CCT->isGrounded)
	{
		m_animator->MoveStateFProperty->valueAsFloat = translation.magnitude > 0.0f;
	}
	else
	{
		m_animator->MoveStateFProperty->valueAsFloat = 0.0f;
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

	if (system->input->GetKeyDown(Key::LShift))
	{
		m_animator->RollTProperty->SetTriggerState();
	}

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
	m_characterRenderer->mesh = system->resource->Find(TEXT("../Resource/Jack/jack.FBX"));
	m_characterRenderer->SetupStandardMaterials();

	m_rightHandWeaponCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));
	m_scabbardCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("Scabbard_cnt_tr"));

	m_goKatanaSheathRenderer = CreateGameObjectToChild(transform);
	m_katanaSheathRenderer = m_goKatanaSheathRenderer->AddComponent<MeshRenderer>();
	m_katanaSheathRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));
	m_katanaSheathRenderer->SetupStandardMaterials();
}

void Player::SetupAnimator()
{
	m_animator = m_goCharacterRender->AddComponent<PlayerAnimator>();
}

void Player::SetupWeapons()
{
	m_goKatana = CreateGameObjectToChild(transform);

	m_goKatanaRenderer = CreateGameObjectToChild(m_goKatana->transform);
	m_katanaRenderer = m_goKatanaRenderer->AddComponent<MeshRenderer>();
	m_katanaRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/Katana.FBX"));
	m_katanaRenderer->SetupStandardMaterials();

	m_goTrail = CreateGameObjectToChild(m_goKatana->transform);
	m_goTrail->transform->localPosition = V3(0, 0, -1.0f);
	m_goTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_trailRenderer = m_goTrail->AddComponent<TrailRenderer>();
	m_trailRenderer->alignment = TrailRenderer::Alignment::Local;
	m_trailRenderer->shrinkDistance = 30.0f;
	m_trailRenderer->width = 2.0f;
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
}

void Player::RotateToDirection(const V3& delta)
{
	if (delta.magnitude > 0.0f)
	{
		V3 up = V3(0, 1, 0);
		Q q = Q::LookRotation(delta.normalized, up);
		transform->rotation = Q::RotateTowards(transform->rotation, q, 720.0f * system->time->deltaTime);
	}
}

V3 Player::GetTranslateDirection()
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
