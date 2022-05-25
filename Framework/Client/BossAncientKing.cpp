#include "stdafx.h"
#include "BossAncientKing.h"
#include "Config.h"
#include "BossAncientKingAnimator.h"

void BossAncientKing::Awake()
{
	Boss::Awake();
	SetupCharacterRenderers();
	SetupAnimator();
	SetupHammer();
	SetupAttackTrigger();

	transform->localScale = V3::one() * 3.0f;
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

void BossAncientKing::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = V3(0, -1.0f, 0);
	m_goCharacterRender->transform->localEulerAngles = V3(90, 180, 0);
	m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
	m_characterRenderer->mesh = system->resource->Find(MESH_ANCIENT_KING);
	m_characterRenderer->SetupStandardMaterials();

	m_L_Hand_Weapon_cnt_tr = m_characterRenderer->GetNodeTransformByName(TEXT("L_Hand_Weapon_cnt_tr"));
}

void BossAncientKing::SetupAnimator()
{
	m_animator = m_goCharacterRender->AddComponent<BossAncientKingAnimator>();
	m_animator->OnBeginChanging += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)>(this, &BossAncientKing::OnBeginChanging);
	m_animator->OnEndChanged += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)>(this, &BossAncientKing::OnEndChanged);
	m_animator->OnAnimationEvent += func<void(Ref<AnimatorLayer>, const AnimationEventDesc&)>(this, &BossAncientKing::OnAnimationEvent);
}

void BossAncientKing::SetupHammer()
{
	m_goHammer = CreateGameObjectToChild(transform);
	m_goHammer->transform->localScale = V3::one() * 5.0f;
	m_hammerRenderer = m_goHammer->AddComponent<MeshRenderer>();
	m_hammerRenderer->mesh = system->resource->Find(MESH_ANCIENT_KING_HAMMER);
	m_hammerRenderer->SetupStandardMaterials();

	m_goHammerTrail = CreateGameObjectToChild(m_goHammer->transform);
	m_goHammerTrail->transform->localPosition = V3(0, 0, -0.72f);
	//m_goHammerTrail->transform->localEulerAngles = V3(0, -96.0f, 0);
	m_hammerTrailRenderer = m_goHammerTrail->AddComponent<TrailRenderer>();
	m_hammerTrailRenderer->alignment = TrailRenderer::Alignment::View;
	m_hammerTrailRenderer->shrinkDistance = 20.0f;
	m_hammerTrailRenderer->width = 6.0f;
	m_hammerTrailRenderer->autoTrail = true;
}

void BossAncientKing::SetupAttackTrigger()
{
}

void BossAncientKing::UpdateCCT()
{
}

void BossAncientKing::UpdateAttachmentObjects()
{
	m_goHammer->transform->position = m_L_Hand_Weapon_cnt_tr->position;
	m_goHammer->transform->rotation = m_L_Hand_Weapon_cnt_tr->rotation;
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
	return DamageOutType::FAILED;
}
