#include "stdafx.h"
#include "BossAncientKing.h"
#include "Config.h"
#include "BossAncientKingAnimator.h"

#include "Player.h"
#include "TPSCamera.h"
#include "EffectShockwave.h"
#include "ProjectileInstanceWaveBeam.h"
#include "EffectRing01.h"
#include "EffectElectricBeam.h"
#include "EffectGroundImapct.h"
#include "ParticleDust01.h"
#include "EventSystem.h"

#define ATK_NEAR_MAX_DIST 6.0f
#define ATK_NEAR_MAX_ANGLE 20.0f
#define ATK_FAR_MAX_DIST 15.0f
#define ATK_FAR_MAX_ANGLE 30.0f
#define ATK_KEEP_MAX_ANGLE 35.0f
#define LOOK_STOP_MIN_ANGLE 5.0f
#define LOOK_START_ANGLE 100.0f
#define IDLE_TIME 2.0f
#define WAIT_DEFAULT_TIME 0.1f
#define TRACE_SPIN_ANGLE_PER_SEC 50.0f
#define LOOK_SPIN_ANGLE_PER_SEC 140.0f
#define TRACE_TO_LOOK_TIME 5.0f
#define SPIN_ON_ATK_NEAR_ANGLE_PER_SEC 15.0f

#define RUSH_START_ACCTIME 12.0f
#define RUSH_MIN_ANGLE 4.0f
#define RUSH_SPIN_ANGLE_PER_SEC 20.0f

#define ATK_FAR_DELAY 7.0f

#define MANUAL_LOOK_SPIN_PER_SEC 180.0f

void BossAncientKing::Awake()
{
	Boss::Awake();

	CCT->radius = 3.0f;
	CCT->height = 0.01f;
	CCT->capsuleCollider->layerIndex = PhysicsLayer_Enemy;
	CCT->collisionWithCCT = false;
	CCT->OnCollision += func<void(const CCTCollision&)>(this, &BossAncientKing::OnCCTCollision);

	SetupCollider();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupLight();
	SetupHammer();
	SetupAttackTrigger();

	SetState(State::NONE);
	m_idleLeftCounter = 0.1f;
}

void BossAncientKing::Start()
{
	Boss::Start();

	m_toPlayerDirection = transform->forward;
}

void BossAncientKing::Update()
{
	Boss::Update();

	if (m_state == State::NONE)
	{
		if (XZDistanceBetweenPlayer() < 10.0f)
		{
			EventSystem::Notify(EVENT_FIGHT_START, this);
			SetState(State::RAGE);
		}
	}

	StateUpdate();
	AttackTriggerQuery();

	m_farATKCounter -= system->time->deltaTime;

	if (m_electricBeam->active)
	{
		m_electricBeam->transform->forward = (m_aimPosition - m_electricBeam->transform->position).normalized;
	}

	if (m_manualLook)
	{
		RotateOnYAxisToDirection(ToPlayerDirectionXZ(), MANUAL_LOOK_SPIN_PER_SEC, system->time->deltaTime);
	}
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
	m_collider->radius = CCT->radius + 0.1f;
	m_collider->halfHeight = 5.0f;
}

void BossAncientKing::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = ADJUST_ANCIENTKING_LOCALPOSITION;
	m_goCharacterRender->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
	m_goCharacterRender->transform->localScale = ADJUST_ANCIENTKING_LOCALSCALE;
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

	m_goHammerTip = CreateGameObjectToChild(m_goHammer->transform);
	m_goHammerTip->transform->localPosition = V3(0.0f, 0.0f, -0.9f);
}

void BossAncientKing::SetupAttackTrigger()
{
	m_goAttackTrigger[HAMMER_TRIGGER] = CreateGameObjectToChild(m_goHammer->transform);
	m_goAttackTrigger[HAMMER_TRIGGER]->transform->localPosition = V3(0.0f, 0.0f, -0.4f);
	m_goAttackTrigger[HAMMER_TRIGGER]->transform->localEulerAngles = V3(90.0f, 0.0f, 0.0f);
	m_attackTrigger[HAMMER_TRIGGER] = m_goAttackTrigger[HAMMER_TRIGGER]->AddComponent<CapsuleCollider>();
	CapsuleCollider* hammer_trigger = (CapsuleCollider*)m_attackTrigger[HAMMER_TRIGGER];
	hammer_trigger->radius = 0.5f;
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

	m_attackTrigger[HAMMER_TIP_TRIGGER] = m_goHammerTip->AddComponent<SphereCollider>();
	SphereCollider* hammerTipTrigger = (SphereCollider*)m_attackTrigger[HAMMER_TIP_TRIGGER];
	hammerTipTrigger->radius = 1.0f;
	hammerTipTrigger->isTrigger = true;
	hammerTipTrigger->enable = false;

	GameObject* goElectricBeam = CreateGameObjectToChild(transform);
	m_electricBeam = goElectricBeam->AddComponent<EffectElectricBeam>();
	m_electricBeam->enable = false;
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

	// Look at player
	float angle = V3::Angle(transform->forward, ToPlayerDirection());
	if (angle < 70.0f)
	{
		V3 toPlayerDirection = ToPlayerDirection();
		m_toPlayerDirection = V3::Lerp(m_toPlayerDirection, toPlayerDirection, system->time->deltaTime * 5.0f).normalized;
	}
	else
	{
		m_toPlayerDirection = V3::Lerp(m_toPlayerDirection, transform->forward, system->time->deltaTime * 5.0f).normalized;
	}
	Q lookRotation = Q::FromToRotation(transform->forward, m_toPlayerDirection);
	m_Head->SetRotationWithCurrentRotation(lookRotation);

	m_goLight->transform->position = m_Head->position;
	m_goLight->transform->rotation = m_Head->rotation;

	m_goAttackTrigger[FOOT_L_TRIGGER]->transform->position = m_LeftFoot->position;
	m_goAttackTrigger[FOOT_L_TRIGGER]->transform->rotation = m_LeftFoot->rotation;

	m_goAttackTrigger[FOOT_R_TRIGGER]->transform->position = m_RightFoot->position;
	m_goAttackTrigger[FOOT_R_TRIGGER]->transform->rotation = m_RightFoot->rotation;

	m_electricBeam->transform->position = m_R_Hand_Weapon_cnt_tr->position;
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
				case BossAncientKingAnimator::UIntContext::ATK_LIGHT:
					damage.Type = DamageInType::LIGHT;
					damage.Damage = 1.0f;
					break;
				case BossAncientKingAnimator::UIntContext::ATK_HEAVY:
					damage.Type = DamageInType::HEAVY;
					damage.Damage = 2.5f;
					break;
				case BossAncientKingAnimator::UIntContext::ATK_BLOW:
					damage.Type = DamageInType::BLOW;
					damage.Damage = 3.0f;
					break;
				case BossAncientKingAnimator::UIntContext::ATK_BLOWUP:
					damage.Damage = 2.0f;
					damage.Type = DamageInType::BLOWUP;
					break;
				case BossAncientKingAnimator::UIntContext::ATK_BLOWDOWN:
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
		OffAttackTriggers();
		ClearHitBuffer();
		m_electricBeam->enable = false;
		m_manualLook = false;
	}

	if (prev.GetPointer() == m_animator->DMG_GROGY_ED)
	{
		SetState(State::WAIT);
	}

	if (prev.GetPointer() == m_animator->ETC_RAGE)
	{
		SetState(State::ATK_NEAR_RAND);
	}
}

void BossAncientKing::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
	if (desc.ContextByte & BossAncientKingAnimator::ByteContext::BACKJUMP_END)
	{
		uint randomFarAttack = rand() % 2;
		if (randomFarAttack == 0)
			SetState(BossAncientKing::State::ATK_BEAM);
		else
			SetState(BossAncientKing::State::ATK_ELECTRIC);
	}
	if (desc.ContextByte & BossAncientKingAnimator::ByteContext::CAM_SHAKE)
	{
		float shakeValue = desc.ContextFloat;
		Player* player = Player::GetInstance();
		if (player)
		{
			TPSCamera* camera = Player::GetInstance()->GetTPSCamera();
			camera->Shake(V3::up(), shakeValue * 0.1f, 1.1f, 0.15f, 4.0f / 0.15f);
		}
	}
	if (desc.ContextByte & BossAncientKingAnimator::ByteContext::LF_SHOCKWAVE)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_attackTrigger[FOOT_L_TRIGGER]->transform->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = m_attackTrigger[FOOT_L_TRIGGER]->transform->position + V3::down() * 0.5f;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		EffectShockwave::Create(
			gameObject->regionScene,
			position,
			1.0f,
			2.0f,
			1.0f, 9.0f
		);
	}
	if (desc.ContextByte & BossAncientKingAnimator::ByteContext::RF_SHOCKWAVE)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_attackTrigger[FOOT_R_TRIGGER]->transform->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = m_attackTrigger[FOOT_R_TRIGGER]->transform->position + V3::down() * 0.5f;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		EffectShockwave::Create(
			gameObject->regionScene,
			position,
			1.0f,
			2.0f,
			1.0f, 9.0f
		);
	}
	if (desc.ContextByte & BossAncientKingAnimator::ByteContext::HAMMER_SHOCKWAVE)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_goHammerTip->transform->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = m_goHammerTip->transform->position;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		EffectShockwave::Create(
			gameObject->regionScene,
			position,
			1.2f,
			3.0f,
			1.0f, 12.0f
		);
	}

	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::ATK_GADABLE)
	{
		m_gadableAttack = true;
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::SET_AIM_ON_HEAD)
	{
		V3 aimPosition = player->transform->position;
		V3 aimDelta = aimPosition - m_Head->position;
		V3 aimDirection = aimDelta.normalized;
		float angle = V3::Angle(transform->forward, aimDirection);
		if (angle < 80.0f)
		{
			m_aimPosition = aimPosition;
		}
		else
		{
			m_aimPosition = CCT->footPosition + transform->forward * 10.0f;
		}
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::SET_AIM_ON_RH)
	{
		V3 aimPosition = player->transform->position;
		V3 aimDelta = aimPosition - m_R_Hand_Weapon_cnt_tr->position;
		V3 aimDirection = aimDelta.normalized;
		float angle = V3::Angle(transform->forward, aimDirection);
		if (angle < 30.0f)
		{
			m_aimPosition = aimPosition;
		}
		else
		{
			m_aimPosition = CCT->footPosition + transform->forward * 10.0f;
		}
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::ATK_SHOOT_BEAM)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> rdYAngle(-10.0f, +10.0f);
		std::uniform_real_distribution<float> rdXZAngle(-10.0f, +10.0f);
		V3 aimDelta = m_aimPosition - m_Head->position;
		V3 aimDirection = aimDelta.normalized;
		Q rdRotate = Q::Euler(rdXZAngle(gen), rdYAngle(gen), rdXZAngle(gen));
		V3 direction = rdRotate.MultiplyVector(aimDirection).normalized;

		GameObject* goBeam = CreateGameObject();
		goBeam->transform->position = m_Head->position;
		goBeam->transform->forward = direction;
		ProjectileInstanceWaveBeam* beam = goBeam->AddComponent<ProjectileInstanceWaveBeam>();

		EffectRing01::Create(
			gameObject->regionScene,
			m_Head->position,
			0.5f,
			0.5f,
			500.0f,
			2.0f, 5.0f,
			Color(1.0f, 0.9764f, 0.466f, 1.0f)
		);
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::ATK_ELECTRIC_START)
	{
		m_electricBeam->enable = true;
		m_electricBeam->transform->localScale = V3(1.0f, 1.0f, 20.0f);
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::ATK_ELECTRIC_END)
	{
		m_electricBeam->enable = false;
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::MANUALLOOK_START)
	{
		m_manualLook = true;
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::MANUALLOOK_END)
	{
		m_manualLook = false; 
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::HAMMER_GROUND_IMPACT)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_goHammerTip->transform->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = CCT->footPosition;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		EffectGroundImapct::Create(
			gameObject->regionScene,
			position + V3::up() * 1.0f,
			1.0f,
			0.1f, 80.0f, 0.8f,
			1000.0f, Color::red(), 0.5f
		);
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::LF_GROUND_IMPACT)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_LeftFoot->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = CCT->footPosition;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		EffectGroundImapct::Create(
			gameObject->regionScene,
			position + V3::up() * 1.0f,
			1.0f,
			0.1f, 80.0f, 0.8f,
			1000.0f, Color::red(), 0.5f
		);
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::RF_GROUND_IMPACT)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_RightFoot->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = CCT->footPosition;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		EffectGroundImapct::Create(
			gameObject->regionScene,
			position + V3::up() * 1.0f,
			1.0f,
			0.1f, 80.0f, 0.8f,
			1000.0f, Color::red(), 0.5f
		);
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::HAMMER_DUST)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_goHammerTip->transform->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = m_goHammerTip->transform->position;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		ParticleDust01::CreateAroundAxis(
			gameObject->regionScene,
			position + V3::up() * 1.0f,
			V3::up(), 1.0f,
			1.0f, 6.0f,
			2.5f, 5.0f,
			2.0f, 10.0f, 0.5f,
			Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
			50);
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::RF_DUST)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_attackTrigger[FOOT_R_TRIGGER]->transform->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = m_attackTrigger[FOOT_R_TRIGGER]->transform->position + V3::down() * 0.5f;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		ParticleDust01::CreateAroundAxis(
			gameObject->regionScene,
			position,
			V3::up(), 1.0f,
			0.2f, 3.0f,
			2.0f, 4.5f,
			2.0f, 10.0f, 0.5f,
			Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
			50);
	}
	if (desc.ContextUInt & BossAncientKingAnimator::UIntContext::LF_DUST)
	{
		PhysicsHit hit;
		PhysicsRay ray;
		ray.Point = m_attackTrigger[FOOT_L_TRIGGER]->transform->position;
		ray.Point.y = CCT->footPosition.y + 0.1f;
		ray.Direction = V3::down();
		ray.Length = 100.0f;
		V3 position = m_attackTrigger[FOOT_L_TRIGGER]->transform->position + V3::down() * 0.5f;
		if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
		{
			position = hit.Point;
		}

		ParticleDust01::CreateAroundAxis(
			gameObject->regionScene,
			position,
			V3::up(), 1.0f,
			0.2f, 3.0f,
			2.0f, 4.5f,
			2.0f, 10.0f, 0.5f,
			Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
			50);
	}

	if (desc.ContextInt & BossAncientKingAnimator::IntContext::HAMMER_START)
	{
		m_attackTrigger[HAMMER_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & BossAncientKingAnimator::IntContext::HAMMER_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & BossAncientKingAnimator::IntContext::LF_START)
	{
		m_attackTrigger[FOOT_L_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & BossAncientKingAnimator::IntContext::LF_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & BossAncientKingAnimator::IntContext::RF_START)
	{
		m_attackTrigger[FOOT_R_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & BossAncientKingAnimator::IntContext::RF_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
	if (desc.ContextInt & BossAncientKingAnimator::IntContext::HAMMER_TIP_START)
	{
		m_attackTrigger[HAMMER_TIP_TRIGGER]->enable = true;
		SetAttackType(desc.ContextUInt);
		ClearHitBuffer();
	}
	if (desc.ContextInt & BossAncientKingAnimator::IntContext::HAMMER_TIP_END)
	{
		OffAttackTriggers();
		ClearHitBuffer();
	}
}

void BossAncientKing::SetAttackType(uint contextUInt)
{
	m_attackType = BossAncientKingAnimator::UIntContext::ATK_LIGHT;

	if (contextUInt & BossAncientKingAnimator::UIntContext::ATK_LIGHT)
		m_attackType = BossAncientKingAnimator::UIntContext::ATK_LIGHT;
	if (contextUInt & BossAncientKingAnimator::UIntContext::ATK_HEAVY)
		m_attackType = BossAncientKingAnimator::UIntContext::ATK_HEAVY;
	if (contextUInt & BossAncientKingAnimator::UIntContext::ATK_BLOW)
		m_attackType = BossAncientKingAnimator::UIntContext::ATK_BLOW;
	if (contextUInt & BossAncientKingAnimator::UIntContext::ATK_BLOWUP)
		m_attackType = BossAncientKingAnimator::UIntContext::ATK_BLOWUP;
	if (contextUInt & BossAncientKingAnimator::UIntContext::ATK_BLOWDOWN)
		m_attackType = BossAncientKingAnimator::UIntContext::ATK_BLOWDOWN;
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
			m_hp -= out.In.Damage;

			if (m_hp <= 0.0f)
			{
				SetState(State::DIE);
			}
			else if (m_hitCount > 40)
			{
				m_hitCount = 0;
				SetState(State::GROGY);
			}
			else
			{
				m_animator->AdditiveDamageTProperty->SetTriggerState();
				
				if (m_state != State::GROGY)
				{
					++m_hitCount;
				}
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

void BossAncientKing::OnCCTCollision(const CCTCollision& collision)
{
	if (m_state == State::ATK_RUSH)
	{
		if (collision.HitCollider->layerIndex == PhysicsLayer_Default)
		{
			float angle = V3::Angle(collision.normal, -transform->forward);
			if (angle < 70.0f)
			{
				m_animator->KeepAttackBProperty->valueAsBool = false;
			}
		}
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

			if (system->physics->query->SweepTest(m_collider, transform->forward, 0.5f, 1 << PhysicsLayer_Player, PhysicsQueryType::Collider))
			{
				if (m_rushDamageReady &&
					m_animator->Layer->IsPlaying(m_animator->ATK_RUSH_LP))
				{
					DamageIn damage = {};
					damage.FromComponent = this;
					damage.FromDirection = player->transform->position - transform->position;
					damage.Guardable = m_gadableAttack;
					damage.Type = DamageInType::BLOW;
					damage.Damage = 3.0f;
					player->Damage(damage);
					m_rushDamageReady = false;
					m_animator->KeepAttackBProperty->valueAsBool = false;
				}
			}

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
		case State::ATK_STEPON:
		case State::ATK_JUMP:
		case State::ATK_DOWNSTRIKE:
		{
			RotateOnYAxisToDirection(ToPlayerDirectionXZ(), SPIN_ON_ATK_NEAR_ANGLE_PER_SEC, system->time->deltaTime);
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

			if (atk != State::ATK_BACKJUMP)
			{
				--m_backJumpLeftCount;

				if (m_backJumpLeftCount <= 0)
				{
					atk = State::ATK_BACKJUMP;
					m_backJumpLeftCount = 5;
				}
			}

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
			m_animator->KeepAttackBProperty->valueAsBool = true;
			m_rushDamageReady = true;
		}
		break;
		case State::ATK_JUMP:
		{
			m_animator->ATK_JUMP_TProperty->SetTriggerState();
		}
		break;
		case State::ATK_ELECTRIC:
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
		case State::ATK_BACKJUMP:
		{
			m_animator->ATK_BACKJUMP_TProperty->SetTriggerState();
		}
		break;
		case State::GROGY:
		{
			m_animator->GROGY_TProperty->SetTriggerState();
		}
		break;
		case State::RAGE:
		{
			m_animator->RAGE_TProperty->SetTriggerState();
		}
		break;
		case State::DIE:
		{
			EventSystem::Notify(EVENT_ENEMY_DIE, this);
			EventSystem::Notify(EVENT_FIGHT_END, this);
			Enemy::UnregistEnemy(this);
			m_animator->Layer->OffAllTriggers();
			m_animator->DIE_TProperty->SetTriggerState();
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
		case State::ATK_RUSH:
		case State::ATK_ELECTRIC:
		case State::ATK_BEAM:
		{
			m_farATKCounter = ATK_FAR_DELAY;
			m_rushDamageReady = false;
		}
	}
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
