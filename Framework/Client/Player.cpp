#include "stdafx.h"
#include "Player.h"
#include "TPSCamera.h"
#include "JackAnimator.h"

void Player::Awake()
{
	SetupTPSCamera();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupPhysics();
	SetupWeapons();
}

void Player::Update()
{
	ImGui::Begin("Angles");
	V3 angles = m_goTrail->transform->localEulerAngles;
	ImGui::SliderFloat3("angles", (float*)&angles, -360, +360);
	m_goTrail->transform->localEulerAngles = angles;
	ImGui::End();

	float dt = system->time->deltaTime;
	auto layer = m_jackAnimator->GetLayerByIndex(0);

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

	if (system->input->GetKeyDown(Key::Space))
	{
		m_jackAnimator->jumpProperty->SetTriggerState();
		m_jackAnimator->hasGroundProperty->valueAsBool = false;
		m_controller->Jump(V3::up() * 11.0f);
	}
	m_jackAnimator->hasGroundProperty->valueAsBool = m_controller->isGrounded;

	m_jackAnimator->moveProperty->valueAsFloat = Clamp01(m_jackAnimator->moveProperty->valueAsFloat - system->time->deltaTime / 0.05f);
	if (layer->currentNode->name.find(TEXT("Atk")) == tstring::npos &&
		layer->currentNode->name.find(TEXT("Land")) == tstring::npos &&
		layer->currentNode->name.find(TEXT("Jump")) == tstring::npos)
	{
		if (translation.sqrMagnitude > 0)
		{
			m_jackAnimator->moveProperty->valueAsFloat = 1.0f;

			V3 up = V3(0, 1, 0);
			Q q = Q::LookRotation(translation.normalized, up);
			transform->rotation = Q::RotateTowards(transform->rotation, q, 720.0f * dt);
		}
	}
	else if (layer->currentNode->name.find(TEXT("Atk")) != tstring::npos)
	{
		if (translation.sqrMagnitude > 0)
		{
			m_jackAnimator->moveProperty->valueAsFloat = 1.0f;

			if (layer->currentNode->normalizedTime < 0.2f)
			{
				V3 up = V3(0, 1, 0);
				Q q = Q::LookRotation(translation.normalized, up);
				transform->rotation = Q::RotateTowards(transform->rotation, q, 720.0f * dt);
			}
		}
	}
	else if (layer->currentNode->name.find(TEXT("Jump")) != tstring::npos)
	{
		if (translation.sqrMagnitude > 0)
		{
			V3 up = V3(0, 1, 0);
			Q q = Q::LookRotation(translation.normalized, up);
			m_controller->Move(translation * 5.0f * system->time->deltaTime, system->time->deltaTime);
			transform->rotation = Q::RotateTowards(transform->rotation, q, 720.0f * dt);
		}
	}
	else if (layer->currentNode->name.find(TEXT("Land")) != tstring::npos)
	{
		if (translation.sqrMagnitude > 0)
		{
			m_jackAnimator->moveProperty->valueAsFloat = 1.0f;
		}
	}

	if (system->input->GetKeyDown(Key::LeftMouse))
	{
		if (layer->currentNode->name.find(TEXT("Atk")) == tstring::npos)
			m_jackAnimator->attackProperty->SetTriggerState();
		else if (
			(layer->currentNode->name == TEXT("AtkX") && layer->currentNode->normalizedTime > 0.15f) || 
			(layer->currentNode->name == TEXT("AtkXX") && layer->currentNode->normalizedTime > 0.3f) || 
			(layer->currentNode->name == TEXT("AtkXXX") && layer->currentNode->normalizedTime > 0.225f))
			m_jackAnimator->comboProperty->SetTriggerState();
	}
}

void Player::LateUpdate()
{
	m_controller->MoveOnGround(m_jackAnimator->GetLayerByIndex(0)->deltaPosition, system->time->deltaTime);

	UpdateAttachmentObjects();
}

void Player::SetupTPSCamera()
{
	m_goTPSCamera = CreateGameObject();
    m_tpsCamera = m_goTPSCamera->AddComponent<TPSCamera>();
	m_tpsCamera->target = transform;
	system->graphic->cameraManager->mainCamera = m_tpsCamera->camera;
}

void Player::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = V3::down();
	m_goCharacterRender->transform->localEulerAngles = V3(90, 180, 0);
	m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
	m_characterRenderer->mesh = system->resource->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));
	//for (auto& m : m_characterRenderer->materials)
	//	m->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Dev/Normal.png")));

	m_rightHandWeaponCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));
	m_scabbardCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("Scabbard_cnt_tr"));

	m_goKatanaSheathRenderer = CreateGameObjectToChild(transform);
	m_katanaSheathRenderer = m_goKatanaSheathRenderer->AddComponent<MeshRenderer>();
	m_katanaSheathRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));
}

void Player::SetupAnimator()
{
	m_jackAnimator = m_goCharacterRender->AddComponent<JackAnimator>();
}

void Player::SetupPhysics()
{
	m_controller = gameObject->AddComponent<CharacterController>();
	m_controller->gravityScale = 2.0f;

	//m_goWeaponTrigger = CreateGameObjectToChild(transform);
	//m_weaponTrigger = m_goWeaponTrigger->AddComponent<SphereCollider>();
	//m_weaponTrigger->isTrigger = true;
}

void Player::SetupWeapons()
{
	m_goKatana = CreateGameObjectToChild(transform);

	m_goKatanaRenderer = CreateGameObjectToChild(m_goKatana->transform);
	m_katanaRenderer = m_goKatanaRenderer->AddComponent<MeshRenderer>();
	m_katanaRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/Katana.FBX"));

	m_goTrail = CreateGameObjectToChild(m_goKatana->transform);
	m_goTrail->transform->localPosition = V3(0, 0, -1.0f);
	m_goTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_trailRenderer = m_goTrail->AddComponent<TrailRenderer>();
	m_trailRenderer->alignment = TrailRenderer::Alignment::Local;
	m_trailRenderer->shrinkDistance = 30.0f;
	m_trailRenderer->width = 2.0f;
}

void Player::UpdateAttachmentObjects()
{
	if (m_goKatanaSheathRenderer->active)
	{
		m_goKatanaSheathRenderer->transform->position = m_scabbardCntNode->position;
		m_goKatanaSheathRenderer->transform->rotation = m_scabbardCntNode->rotation;
	}

	if (m_goWeaponTrigger || m_goKatana->active)
	{
		V3 position = m_rightHandWeaponCntNode->position;
		Q  rotation = m_rightHandWeaponCntNode->rotation;

		if (m_goKatana->active)
		{
			m_goKatana->transform->position = position;
			m_goKatana->transform->rotation = rotation;
		}

		if (m_goWeaponTrigger)
		{
			m_goWeaponTrigger->transform->rotation = rotation;
			m_goWeaponTrigger->transform->position = position - m_goWeaponTrigger->transform->forward * 1.0f;
		}
	}
}
