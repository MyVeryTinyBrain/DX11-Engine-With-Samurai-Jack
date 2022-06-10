#include "stdafx.h"
#include "Player.h"
#include "TPSCamera.h"
#include "PlayerAnimator.h"
#include "Config.h"
#include "Enemy.h"

#include "ParticleInstanceSpark.h"
#include "EffectShockwave.h"
#include "EffectRing01.h"
#include "ParticleDust01.h"
#include "EffectGlow.h"

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

void Player::Start()
{
	Character::Start();

	m_tpsCamera->transform->position = transform->position;
	m_tpsCamera->transform->forward = transform->forward;
}

#include "BossAncientKing.h"
#include "EnemyBeetleDrone.h"
void Player::Update()
{
	Character::Update();

	SecondGroundCheck();

	UpdateKeyTimes();
	AttackTriggerQuery();

	XZInput();
	DashInput();
	JumpInput();
	RollInput();
	GuardInput();
	AttackInput();

	if (system->input->GetKeyDown(Key::WhileMouse))
	{
		if (!m_tpsCamera->lookTarget)
			m_tpsCamera->LookNearEnemy();
		else
			m_tpsCamera->Unlook();
	}

	ImGui::Begin("Monster");
	if (ImGui::Button("Beetle"))
	{
		GameObject* goEnemy = CreateGameObject();
		goEnemy->transform->position = transform->position + transform->forward * 3.0f + V3::up() * 5.0f;
		goEnemy->AddComponent<EnemyBeetleDrone>();
	}
	if (ImGui::Button("AncientKing"))
	{
		GameObject* goEnemy = CreateGameObject();
		goEnemy->transform->position = transform->position + transform->forward * 6.0f + V3::up() * 8.0f;
		goEnemy->AddComponent<BossAncientKing>();
	}
	if (ImGui::Button("Destroy"))
	{
		while (Enemy::GetEnemyCount() > 0)
		{
			Enemy::GetEnemyByIndex(0)->gameObject->Destroy();
		}
	}
	ImGui::End();
}

void Player::FixedUpdate()
{
	Character::FixedUpdate();

	m_animator->GroundStateBProperty->valueAsBool = m_groundCheck;
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
	m_tpsCamera->traceTarget = transform;
	system->graphic->cameraManager->mainCamera = m_tpsCamera->camera;
}

void Player::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = ADJUST_PLAYER_LOCALPOSITION;
	m_goCharacterRender->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
	m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
	m_characterRenderer->mesh = system->resource->Find(MESH_JACK);
	m_characterRenderer->SetupStandardMaterials();

	m_RightHandWeaponCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));
	m_ScabbardCntNode = m_characterRenderer->GetNodeTransformByName(TEXT("Scabbard_cnt_tr"));
	m_LeftToeBaseNode = m_characterRenderer->GetNodeTransformByName(TEXT("LeftToeBase"));
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
	m_katanaSheathRenderer->mesh = system->resource->Find(MESH_KATANA_SHEATH);
	m_katanaSheathRenderer->SetupStandardMaterials();
}

void Player::SetupKatana()
{
	m_goKatana = CreateGameObjectToChild(transform);
	m_katanaRenderer = m_goKatana->AddComponent<MeshRenderer>();
	m_katanaRenderer->mesh = system->resource->Find(MESH_KATANA);
	m_katanaRenderer->SetupStandardMaterials();

	m_goKatanaTrail = CreateGameObjectToChild(m_goKatana->transform);
	m_goKatanaTrail->transform->localPosition = V3(0.0f, 0.0f, -1.1f);
	m_goKatanaTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_katanaTrailRenderer = m_goKatanaTrail->AddComponent<TrailRenderer>();
	m_katanaTrailRenderer->alignment = TrailRenderer::Alignment::Local;
	m_katanaTrailRenderer->shrinkDistance = 30.0f;
	m_katanaTrailRenderer->width = 2.0f;
	m_katanaTrailRenderer->autoTrail = false;
	m_katanaTrailRenderer->interpolateStep = 30;
	//m_katanaTrailRenderer->fitVToLength = false;

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_TRAIL);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	material->SetTexture("_GradientTexture", system->resource->Find(TEX_GRADIENT_TO_RIGHT));
	//material->SetTexture("_DistortionTexture", system->resource->Find(TEX_EFFECT_ELECTRIC_V));
	m_katanaTrailRenderer->material = material;
}

void Player::SetupFootCnt()
{
	m_goFootCnt = CreateGameObjectToChild(m_goCharacterRender->transform);
	m_footTrailRenderer = m_goFootCnt->AddComponent<TrailRenderer>();
	m_footTrailRenderer->shrinkDistance = 30.0f;
	m_footTrailRenderer->width = 2.0f;
	m_footTrailRenderer->autoTrail = false;
	m_footTrailRenderer->alignment = TrailRenderer::Alignment::Local;
	//m_footTrailRenderer->fitVToLength = false;

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_TRAIL);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	material->SetTexture("_GradientTexture", system->resource->Find(TEX_GRADIENT_CENTER));
	//material->SetTexture("_DistortionTexture", system->resource->Find(TEX_EFFECT_ELECTRIC_V));
	material->SetFloat("_SideAttenFactor", 1.5f);
	m_footTrailRenderer->material = material;
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
}

void Player::UpdateCCT()
{
	CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
}

void Player::UpdateAttachmentObjects()
{
	m_goKatanaSheathRenderer->transform->position = m_ScabbardCntNode->position;
	m_goKatanaSheathRenderer->transform->rotation = m_ScabbardCntNode->rotation;

	m_goKatana->transform->position = m_RightHandWeaponCntNode->position;
	m_goKatana->transform->rotation = m_RightHandWeaponCntNode->rotation;

	m_goFootCnt->transform->position = m_LeftToeBaseNode->position - m_LeftToeBaseNode->up * 1.0f + transform->up * 0.25f;
	m_goFootCnt->transform->rotation = m_LeftToeBaseNode->rotation;
	V3 footCntEulerAngles = m_goFootCnt->transform->eulerAngles;
	footCntEulerAngles.x = 0;
	footCntEulerAngles.z = 0;
	m_goFootCnt->transform->eulerAngles = footCntEulerAngles;
}

void Player::AttackTriggerQuery()
{
	for (int i = 0; i < MAX_TRIGGERS; ++i)
	{
		if (!m_attackTrigger[i]->enable)
			continue;

		uint numHit = 0;
		vector<Collider*> overlaps = system->physics->query->OverlapAll(m_attackTrigger[i], 1 << PhysicsLayer_Enemy, PhysicsQueryType::Collider);
		for (auto& overlap : overlaps)
		{
			Rigidbody* rigidbody = overlap->rigidbody;
			if (!rigidbody) continue;

			Enemy* enemy = rigidbody->gameObject->GetComponent<Enemy>();
			if (!enemy) continue;

			auto result = m_hitBuffer.insert(rigidbody);
			if (!result.second) continue; // 이미 힛 버퍼에 존재합니다.

			DamageIn damage = {};
			damage.FromComponent = this;
			damage.FromDirection = enemy->transform->position - transform->position;
			damage.Guardable = true;
			switch (m_attackType)
			{
				case PlayerAnimator::UIntContext::ATK_LIGHT:
					damage.Type = DamageInType::LIGHT;
					damage.Damage = 1.0f;
					break;
				case PlayerAnimator::UIntContext::ATK_HEAVY:
					damage.Type = DamageInType::HEAVY;
					damage.Damage = 2.5f;
					break;
				case PlayerAnimator::UIntContext::ATK_BLOW:
					damage.Type = DamageInType::BLOW;
					damage.Damage = 3.0f;
					damage.Velocity = transform->forward * 5.0f + V3(0, 5, 0);
					damage.SetVelocity = true;
					break;
				case PlayerAnimator::UIntContext::ATK_BLOWUP:
					damage.Damage = 2.0f;
					damage.Type = DamageInType::BLOWUP;
					damage.Velocity = BLOWUP_VELOCITY;
					damage.SetVelocity = true;
					break;
				case PlayerAnimator::UIntContext::ATK_BLOWDOWN:
					damage.Damage = 3.0f;
					damage.Type = DamageInType::BLOWDOWN;
					damage.Velocity = BLOWDOWN_VELOCITY;
					damage.SetVelocity = true;
					break;
			}
			DamageOutType damaged = enemy->Damage(damage);

			if (damaged != DamageOutType::IGNORED)
			{
				++numHit;

				V3 katanaPos = m_goAttackTrigger[KATANA_TRIGGER]->transform->position + transform->forward * 2.5f;
				V3 effectPos = (katanaPos + overlap->transform->position) * 0.5f;

				ParticleInstanceSpark::Create(
					gameObject->regionScene,
					effectPos,
					-transform->forward,
					0.0f, 45.0f,
					1.0f, 5.0f,
					2.0f, 9.0f,
					0.1f, 0.355f,
					0.5f, 2.0f,
					50);

				ParticleDust01::CreateAroundAxis(
					gameObject->regionScene,
					effectPos,
					V3::up(), 1.0f,
					0.5f, 1.5f,
					0.5f, 1.0f,
					1.0f, 2.0f, 0.5f,
					Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
					10);

				EffectGlow::Create(
					gameObject->regionScene,
					effectPos,
					0.5f,
					V2(20.0f, 0.5f), V2(50.0f, 0.1f), 1.0f,
					Color::white(), Color(1.0f, 1.0f, 1.0f, 0.0f), 1.0f
				);
			}
		}

		if (numHit > 0)
		{
			Q q = Q::AxisAngle(transform->forward, float(rand() % 361));
			m_tpsCamera->Shake(q.MultiplyVector(V3::up()), 0.025f, 1.1f, 0.1f, 2.0f / 0.1f);
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

void Player::XZInput()
{
	V3 translation = GetTranslateDirection();
	m_animator->MoveStateFProperty->valueAsFloat = translation.magnitude > 0.0f;

	bool atk_or_gad =
		m_animator->Layer->IsContains(TEXT("ATK")) ||
		m_animator->Layer->IsContains(TEXT("GAD"));

	bool damaged = m_animator->Layer->IsContains(TEXT("DMG"));

	if (!damaged)
	{
		if (m_tpsCamera->lookTarget && atk_or_gad)
		{
			V3 look = m_tpsCamera->lookTarget->position - transform->position;
			look.y = 0.0f;
			look.Normalize();
			RotateOnYAxisToDirection(look, 720.0f, system->time->deltaTime);
		}
		else
		{
			RotateOnYAxisToDirection(translation, 900.0f, system->time->deltaTime);
		}
	}
	else if (damaged && m_damagedDirection.magnitude > 0.01f)
	{
		RotateOnYAxisToDirection(-m_damagedDirection, 900.0f, system->time->deltaTime);
	}


	bool airAction = 
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_X) ||
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_XX) ||
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_XXX) ||
		m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_START);

	CCT->useGravity = !airAction;

	if (m_groundCheck)
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
}

void Player::DashInput()
{
	float targetDashState = float(system->input->GetKey(Key::LShift) && IsLongKeyPressing(m_lShiftPressingTime));
	m_animator->DashStateFProperty->valueAsFloat = Lerp(m_animator->DashStateFProperty->valueAsFloat, targetDashState, system->time->deltaTime * 10.0f);
}

void Player::JumpInput()
{
	if (system->input->GetKeyDown(Key::Space))
	{
		if (m_groundCheck)
		{
			m_animator->JumpTProperty->SetTriggerState();
		}
		else
		{
			m_animator->AirJumpTProperty->SetTriggerState();
		}
	}
}

void Player::RollInput()
{
	if (system->input->GetKeyUp(Key::LShift) && IsShortKeyPressed(m_lShiftPressedTime))
	{
		m_animator->RollTProperty->SetTriggerState();
	}
}

void Player::GuardInput()
{
	if (system->input->GetKeyDown(Key::RightMouse))
	{
		m_animator->LastDashStateFProperty->valueAsFloat = m_animator->DashStateFProperty->valueAsFloat;
		m_animator->DashStateFProperty->valueAsFloat = 0.0f;
	}
	m_animator->GuardBProperty->valueAsBool = system->input->GetKey(Key::RightMouse);
}

void Player::AttackInput()
{
	bool damaged = m_animator->Layer->IsContains(TEXT("DMG"));		

	m_animator->LightAttackTProperty->OffTriggerState();
	m_animator->HeavyAttackTProperty->OffTriggerState();

	if (!damaged)
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

	if (m_animator->Layer->IsPlaying(m_animator->ATK_AIR_Y_LOOP))
	{
		CCT->Move(V3::down() * 40.0f * system->time->deltaTime, system->time->deltaTime);
		CCT->collisionWithCCT = false;
		CCT->capsuleCollider->SetIgnoreLayerIndex(PhysicsLayer_Enemy, true);
	}
	else
	{
		CCT->collisionWithCCT = true;
		CCT->capsuleCollider->SetIgnoreLayerIndex(PhysicsLayer_Enemy, false);
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
	return delta <= 0.25f;
}

bool Player::IsLongKeyPressing(float pressingTime) const
{
	return pressingTime > 0.25f;
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
		OffAttackTriggers();
		ClearHitBuffer();
	}
}

void Player::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
	if (desc.ContextInt & PlayerAnimator::IntContext::KT_START)
	{
		m_katanaTrailRenderer->autoTrail = true;
		m_attackTrigger[KATANA_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & PlayerAnimator::IntContext::KT_END)
	{
		m_katanaTrailRenderer->autoTrail = false;
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & PlayerAnimator::IntContext::FOOT_START)
	{
		m_footTrailRenderer->autoTrail = true;
		m_attackTrigger[FOOT_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & PlayerAnimator::IntContext::FOOT_END)
	{
		m_footTrailRenderer->autoTrail = false;
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & PlayerAnimator::IntContext::KT_STING_START)
	{
		m_attackTrigger[KATANA_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & PlayerAnimator::IntContext::KT_STING_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextByte & PlayerAnimator::ByteContext::JUMP)
	{
		V3 jump;
		jump.y = desc.ContextFloat;
		CCT->Jump(jump);

		EffectShockwave::Create(
			gameObject->regionScene,
			CCT->footPosition,
			0.25f,
			0.5f,
			0.2f, 2.5f
		);
	}
}

void Player::SetAttackType(uint contextUInt)
{
	m_attackType = PlayerAnimator::UIntContext::ATK_LIGHT;

	if (contextUInt & PlayerAnimator::UIntContext::ATK_LIGHT)
		m_attackType = PlayerAnimator::UIntContext::ATK_LIGHT;
	if (contextUInt & PlayerAnimator::UIntContext::ATK_HEAVY)
		m_attackType = PlayerAnimator::UIntContext::ATK_HEAVY;
	if (contextUInt & PlayerAnimator::UIntContext::ATK_BLOW)
		m_attackType = PlayerAnimator::UIntContext::ATK_BLOW;
	if (contextUInt & PlayerAnimator::UIntContext::ATK_BLOWUP)
		m_attackType = PlayerAnimator::UIntContext::ATK_BLOWUP;
	if (contextUInt & PlayerAnimator::UIntContext::ATK_BLOWDOWN)
		m_attackType = PlayerAnimator::UIntContext::ATK_BLOWDOWN;
}

float Player::GetHP() const
{
	return 0.0f;
}

void Player::SetHP(float value)
{
}

V3 Player::GetDirection() const
{
	return transform->forward;
}

void Player::SetDirection(const V3& direction)
{
	transform->forward = direction;
}

bool Player::IsGuarding() const
{
	if (m_animator->Layer->IsPlaying(m_animator->GAD_START) ||
		m_animator->Layer->IsPlaying(m_animator->GAD_LOOP) ||
		m_animator->Layer->IsPlaying(m_animator->GAD_HIT))
		return true;

	return false;
}

bool Player::IsGuardBreakableByBackattack() const
{
	return true;
}

bool Player::IsInvisible() const
{
	return false;
}

bool Player::IsSuperarmor() const
{
	return false;
}

DamageOutType Player::OnDamage(const DamageOut& out)
{
	switch (out.Type) 
	{
		case DamageOutType::GUARDED:
		{
			m_damagedDirection = out.In.FromDirection;
			m_damagedDirection.y = 0;
			m_damagedDirection.Normalize();

			ParticleInstanceSpark::Create(
				gameObject->regionScene,
				m_goAttackTrigger[KATANA_TRIGGER]->transform->position,
				-transform->forward,
				0.0f, 45.0f,
				1.0f, 5.0f,
				2.0f, 9.0f,
				0.1f, 0.5f,
				0.5f, 2.0f,
				50);

			EffectRing01::Create(
				gameObject->regionScene,
				transform->position,
				0.5f,
				0.5f,
				500.0f,
				0.5f, 2.0f,
				Color::RGBA255(255, 142, 51, 255)
			);

			EffectGlow::Create(
				gameObject->regionScene,
				m_goAttackTrigger[KATANA_TRIGGER]->transform->position,
				0.5f,
				V2(20.0f, 0.5f), V2(50.0f, 0.1f), 1.0f,
				Color::white(), Color(1.0f, 1.0f, 1.0f, 0.0f), 1.0f
			);

			Q q = Q::AxisAngle(transform->forward, float(rand() % 361));
			m_tpsCamera->Shake(q.MultiplyVector(V3::up()), 0.025f, 1.1f, 0.1f, 2.0f / 0.1f);

			m_animator->GuardHitTProperty->SetTriggerState();
			return DamageOutType::GUARDED;
		}
		break;
		case DamageOutType::GUARD_BREAKED:
		{
			m_damagedDirection = out.In.FromDirection;
			m_damagedDirection.y = 0;
			m_damagedDirection.Normalize();

			ParticleInstanceSpark::Create(
				gameObject->regionScene,
				m_goAttackTrigger[KATANA_TRIGGER]->transform->position,
				-transform->forward,
				0.0f, 45.0f,
				1.0f, 5.0f,
				2.0f, 9.0f,
				0.1f, 0.5f,
				0.5f, 2.0f,
				50);

			EffectRing01::Create(
				gameObject->regionScene,
				transform->position,
				0.5f,
				0.5f,
				500.0f,
				0.5f, 2.0f,
				Color::RGBA255(255, 71, 51, 255)
			);

			EffectGlow::Create(
				gameObject->regionScene,
				m_goAttackTrigger[KATANA_TRIGGER]->transform->position,
				0.5f,
				V2(20.0f, 0.5f), V2(50.0f, 0.1f), 1.0f,
				Color::white(), Color(1.0f, 1.0f, 1.0f, 0.0f), 1.0f
			);

			Q q = Q::AxisAngle(transform->forward, float(rand() % 361));
			m_tpsCamera->Shake(q.MultiplyVector(V3::up()), 0.025f, 1.1f, 0.1f, 2.0f / 0.1f);

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
				{
					m_animator->DamageTypeIProperty->valueAsInt = 0;

					if (!out.Backattack)
					{
						m_damagedDirection = out.In.FromDirection;
						m_damagedDirection.y = 0;
						m_damagedDirection.Normalize();
					}
					else
					{
						m_damagedDirection = {};
					}
				}
				break;
				case DamageInType::HEAVY:
				{
					m_animator->DamageTypeIProperty->valueAsInt = 1;

					if (!out.Backattack)
					{
						m_damagedDirection = out.In.FromDirection;
						m_damagedDirection.y = 0;
						m_damagedDirection.Normalize();
					}
					else
					{
						m_damagedDirection = {};
					}
				}
				break;
				case DamageInType::BLOW:
				case DamageInType::BLOWUP:
				{
					m_animator->DamageTypeIProperty->valueAsInt = 2;

					m_damagedDirection = out.In.FromDirection;
					m_damagedDirection.y = 0;
					m_damagedDirection.Normalize();
				}
				break;
			}

			if (!CCT->isGrounded)
			{
				m_animator->DamageTypeIProperty->valueAsInt = 2;
			}
			if (out.In.SetVelocity)
			{
				CCT->Jump(out.In.Velocity);
			}

			ParticleDust01::CreateWithNormal(
				gameObject->regionScene,
				transform->position,
				-out.In.FromDirection, 30.0f, 30.0f, 1.0f,
				0.5f, 2.0f,
				0.5f, 2.0f,
				1.0f, 3.0f, 0.5f,
				Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
				10);

			Q q = Q::AxisAngle(transform->forward, float(rand() % 361));
			m_tpsCamera->Shake(q.MultiplyVector(V3::up()), 0.035f, 1.1f, 0.1f, 2.0f / 0.1f);
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

void Player::SecondGroundCheck()
{
	// 너무 정확한 CCT->isGrounded는 작은 떨림도 공중에 뜲으로 인식합니다.
	// 약간 부정확한 2차 검사를 통해 작은 떨림을 무시합니다.

	if (!CCT->isGrounded)
	{
		const static float adjustHeight = 0.2f;

		PhysicsRay ray;
		bool hit = system->physics->query->SweepCapsuleTest(
			CCT->capsuleCollider->transform->position,
			Q::identity(),
			CCT->capsuleCollider->radius, CCT->capsuleCollider->halfHeight,
			V3::down(),
			adjustHeight,
			1 << PhysicsLayer_Default,
			PhysicsQueryType::Collider,
			CCT->rigidbody
		);
		m_groundCheck = hit;
	}
	else
	{
		m_groundCheck = true;
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
