#include "stdafx.h"
#include "BossAncientKing.h"
#include "Config.h"
#include "BossAncientKingAnimator.h"

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
}

void BossAncientKing::Start()
{
	Boss::Start();
}

void BossAncientKing::Update()
{
	Boss::Update();

	if (system->input->GetKeyDown(Key::One))
		m_animator->AdditiveDamageTProperty->SetTriggerState();
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
	m_collider->halfHeight = CCT->height * 0.5f;
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
	m_goHammer->transform->localScale = V3::one() * 5.0f;
	m_hammerRenderer = m_goHammer->AddComponent<MeshRenderer>();
	m_hammerRenderer->mesh = system->resource->Find(MESH_ANCIENT_KING_HAMMER);
	m_hammerRenderer->SetupStandardMaterials();

	m_goHammerTrail = CreateGameObjectToChild(m_goHammer->transform);
	m_goHammerTrail->transform->localPosition = V3(0, 0, -0.72f);
	//m_goHammerTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_hammerTrailRenderer = m_goHammerTrail->AddComponent<TrailRenderer>();
	m_hammerTrailRenderer->alignment = TrailRenderer::Alignment::View;
	m_hammerTrailRenderer->shrinkDistance = 50.0f;
	m_hammerTrailRenderer->width = 4.0f;
	m_hammerTrailRenderer->autoTrail = true;
}

void BossAncientKing::SetupAttackTrigger()
{
}

void BossAncientKing::UpdateCCT()
{
	if (CCT->isGrounded)
	{
		CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
	}
}

void BossAncientKing::UpdateAttachmentObjects()
{
	m_goHammer->transform->position = m_L_Hand_Weapon_cnt_tr->position;
	m_goHammer->transform->rotation = m_L_Hand_Weapon_cnt_tr->rotation;

	m_goLight->transform->position = m_Head->position;
	m_goLight->transform->rotation = m_Head->rotation;
}

void BossAncientKing::AttackTriggerQuery()
{
}

void BossAncientKing::OffAttackTriggers()
{
}

void BossAncientKing::ClearHitBuffer()
{
}

void BossAncientKing::OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing)
{
}

void BossAncientKing::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
}

void BossAncientKing::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
}

void BossAncientKing::SetAttackType(int contextInt)
{
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
