#include "stdafx.h"
#include "BossAshi.h"
#include "Config.h"
#include "BossAshiAnimator.h"
#include "Player.h"

void BossAshi::Awake()
{
    Boss::Awake();

    CCT->radius = 0.5f;
    CCT->height = 2.2f;
    CCT->capsuleCollider->layerIndex = PhysicsLayer_Enemy;
    CCT->collisionWithCCT = false;
    CCT->OnCollision += func<void(const CCTCollision&)>(this, &BossAshi::OnCCTCollision);
    CCT->enable = false;

    SetupCollider();
    SetupCharacterRenderer();
    SetupAnimator();
    SetupSword();
    SetupAttackTrigger();
    SetupAudioSource();

    SetState(State::NONE);
    m_idleLeftCounter = 0.1f;
}

void BossAshi::Start()
{
    Boss::Start();

    m_animator->MoveFProperty->valueAsFloat = 1.0f;
    m_animator->WalkDirectionFProperty->valueAsFloat = 0.0f;
}

void BossAshi::Update()
{
    Boss::Update();
}

void BossAshi::LateUpdate()
{
    Boss::LateUpdate();

    UpdateCCT();
    UpdateAttachmentObjects();
}

void BossAshi::FixedUpdate()
{
    Boss::FixedUpdate();

    CCT->enable = true;
}

void BossAshi::SetupCollider()
{
    GameObject* goCollider = CreateGameObjectToChild(transform);
    m_collider = goCollider->AddComponent<CapsuleCollider>();
    m_collider->layerIndex = PhysicsLayer_Enemy;
    m_collider->radius = CCT->radius + 0.2f;
    m_collider->halfHeight = CCT->height * 0.5f + 0.2f;
}

void BossAshi::SetupCharacterRenderer()
{
    m_goCharacterRender = CreateGameObjectToChild(transform);
    m_goCharacterRender->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
    m_goCharacterRender->transform->localPosition = ADJUST_ASHI_LOCALPOSITION;
    m_goCharacterRender->transform->localScale = ADJUST_ASHI_LOCALSCALE;

    m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
    m_characterRenderer->mesh = system->resource->Find(MESH_ASHI);
    m_characterRenderer->SetupStandardMaterials();

    m_R_Hand_Weapon_cnt_tr = m_characterRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));
    m_RightFoot = m_characterRenderer->GetNodeTransformByName(TEXT("RightFoot"));
}

void BossAshi::SetupAnimator()
{
    m_animator = m_goCharacterRender->AddComponent<BossAshiAnimator>();
    m_animator->OnBeginChanging += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)>(this, &BossAshi::OnBeginChanging);
    m_animator->OnEndChanged += func<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)>(this, &BossAshi::OnEndChanged);
    m_animator->OnAnimationEvent += func<void(Ref<AnimatorLayer>, const AnimationEventDesc&)>(this, &BossAshi::OnAnimationEvent);
}

void BossAshi::SetupSword()
{
    m_goSword = CreateGameObjectToChild(m_goCharacterRender->transform);
    m_goSword->transform->localScale = V3::one() * 1.0f;
    m_swordRenderer = m_goSword->AddComponent<MeshRenderer>();
    m_swordRenderer->mesh = system->resource->Find(MESH_ASHI_SWORD);
    m_swordRenderer->SetupStandardMaterials();

    m_goSwordTip = CreateGameObjectToChild(m_goSword->transform);
    m_goSwordTip->transform->localEulerAngles = V3(90.0f, 0.0f, 0.0f);
    m_goSwordTip->transform->localPosition = V3(0.0f, 0.0f, -1.0f);
}

void BossAshi::SetupAttackTrigger()
{
    m_attackTrigger[SWORD_TRIGGER] = m_goSwordTip->AddComponent<CapsuleCollider>();
    m_attackTrigger[SWORD_TRIGGER]->isTrigger = true;
    m_attackTrigger[SWORD_TRIGGER]->debugRenderMode = Collider::DebugRenderMode::Fill;

    GameObject* goRightFootTrigger = CreateGameObjectToChild(m_characterRenderer->transform);
    m_attackTrigger[RIGHT_FOOT_TRIGGER] = goRightFootTrigger->AddComponent<SphereCollider>();
    m_attackTrigger[RIGHT_FOOT_TRIGGER]->isTrigger = true;
    m_attackTrigger[RIGHT_FOOT_TRIGGER]->debugRenderMode = Collider::DebugRenderMode::Fill;
}

void BossAshi::SetupAudioSource()
{
    m_audioSource = gameObject->AddComponent<AudioSource>();
    m_audioSource->minDistance = 8.0f;
    m_audioSource->maxDistance = m_audioSource->minDistance + 30.0f;
}

void BossAshi::UpdateCCT()
{
    if (CCT->isGrounded)
    {
        CCT->MoveOnGround(m_animator->Layer->deltaPosition, system->time->deltaTime);
        Q deltaRotation = Q::RightHandedToLeftHanded(m_animator->Layer->deltaRotation);
        if (deltaRotation.eulerAngles.sqrMagnitude > Epsilon)
            transform->localRotation *= Q::RightHandedToLeftHanded(m_animator->Layer->deltaRotation);
    }
}

void BossAshi::UpdateAttachmentObjects()
{
    m_goSword->transform->position = m_R_Hand_Weapon_cnt_tr->position;
    m_goSword->transform->rotation = m_R_Hand_Weapon_cnt_tr->rotation;

    m_attackTrigger[RIGHT_FOOT_TRIGGER]->transform->position = m_RightFoot->position;
}

void BossAshi::AttackTriggerQuery()
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
                case BossAshiAnimator::UIntContext::ATK_LIGHT:
                    damage.Type = DamageInType::LIGHT;
                    damage.Damage = 1.0f;
                    break;
                case BossAshiAnimator::UIntContext::ATK_HEAVY:
                    damage.Type = DamageInType::HEAVY;
                    damage.Damage = 2.5f;
                    break;
                case BossAshiAnimator::UIntContext::ATK_BLOW:
                    damage.Type = DamageInType::BLOW;
                    damage.Damage = 3.0f;
                    break;
                case BossAshiAnimator::UIntContext::ATK_BLOWUP:
                    damage.Damage = 2.0f;
                    damage.Type = DamageInType::BLOWUP;
                    break;
                case BossAshiAnimator::UIntContext::ATK_BLOWDOWN:
                    damage.Damage = 3.0f;
                    damage.Type = DamageInType::BLOWDOWN;
                    break;
            }
            player->Damage(damage);
        }
    }
}

void BossAshi::OffAttackTriggers()
{
    for (int i = 0; i < MAX_TRIGGERS; ++i)
        m_attackTrigger[i]->enable = false;
}

void BossAshi::ClearHitBuffer()
{
    if (!m_hitBuffer.empty())
        m_hitBuffer.clear();
}

void BossAshi::OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing)
{
}

void BossAshi::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
}

void BossAshi::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
}

void BossAshi::SetAttackType(uint contextUInt)
{
    m_attackType = BossAshiAnimator::UIntContext::ATK_LIGHT;

    if (contextUInt & BossAshiAnimator::UIntContext::ATK_LIGHT)
        m_attackType = BossAshiAnimator::UIntContext::ATK_LIGHT;
    if (contextUInt & BossAshiAnimator::UIntContext::ATK_HEAVY)
        m_attackType = BossAshiAnimator::UIntContext::ATK_HEAVY;
    if (contextUInt & BossAshiAnimator::UIntContext::ATK_BLOW)
        m_attackType = BossAshiAnimator::UIntContext::ATK_BLOW;
    if (contextUInt & BossAshiAnimator::UIntContext::ATK_BLOWUP)
        m_attackType = BossAshiAnimator::UIntContext::ATK_BLOWUP;
    if (contextUInt & BossAshiAnimator::UIntContext::ATK_BLOWDOWN)
        m_attackType = BossAshiAnimator::UIntContext::ATK_BLOWDOWN;
}

float BossAshi::GetHP() const
{
    return m_hp;
}

void BossAshi::SetHP(float value)
{
    m_hp = value;
}

V3 BossAshi::GetDirection() const
{
    return transform->forward;
}

void BossAshi::SetDirection(const V3& direction)
{
    transform->forward = direction;
}

bool BossAshi::IsGuarding() const
{
    return false;
}

bool BossAshi::IsGuardBreakableByBackattack() const
{
    return false;
}

bool BossAshi::IsInvisible() const
{
    return false;
}

bool BossAshi::IsSuperarmor() const
{
    return false;
}

DamageOutType BossAshi::OnDamage(const DamageOut& out)
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
            else
            {
                m_animator->AdditiveDamageTProperty->SetTriggerState();
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

void BossAshi::OnCCTCollision(const CCTCollision& collision)
{
}

void BossAshi::SetState(BossAshi::State state)
{
}

void BossAshi::StateUpdate()
{
}

void BossAshi::StateChanged(BossAshi::State before, BossAshi::State next)
{
}

void BossAshi::StateEnded(BossAshi::State before, BossAshi::State current)
{
}
