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

    SetState(State::WALK_ANYWHERE);
}

void BossAshi::Update()
{
    Boss::Update();

    WalkDirectionLerp();
    StateUpdate();
    AttackTriggerQuery();

    if (m_manualLook)
    {
        RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 360.0f, system->time->deltaTime);
    }
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
    m_collider->radius = CCT->radius + 0.3f;
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
    m_goSword->transform->localScale = V3::one() * 1.5f;
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
    m_attackTrigger[SWORD_TRIGGER]->enable = false;
    CapsuleCollider* swordTrigger = (CapsuleCollider*)m_attackTrigger[SWORD_TRIGGER];
    swordTrigger->radius = 0.7f;
    swordTrigger->halfHeight = 0.5f;

    GameObject* goRightFootTrigger = CreateGameObjectToChild(m_characterRenderer->transform);
    m_attackTrigger[RIGHT_FOOT_TRIGGER] = goRightFootTrigger->AddComponent<SphereCollider>();
    m_attackTrigger[RIGHT_FOOT_TRIGGER]->isTrigger = true;
    m_attackTrigger[RIGHT_FOOT_TRIGGER]->enable = false;
    SphereCollider* rightFootTrigger = (SphereCollider*)m_attackTrigger[RIGHT_FOOT_TRIGGER];
    rightFootTrigger->radius = 0.6f;
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
        //Q deltaRotation = Q::RightHandedToLeftHanded(m_animator->Layer->deltaRotation);
        //if (deltaRotation.eulerAngles.sqrMagnitude > Epsilon)
        //    transform->localRotation *= Q::RightHandedToLeftHanded(m_animator->Layer->deltaRotation);
    }
}

void BossAshi::UpdateAttachmentObjects()
{
    m_goSword->transform->position = m_R_Hand_Weapon_cnt_tr->position;
    m_goSword->transform->rotation = m_R_Hand_Weapon_cnt_tr->rotation;

    m_attackTrigger[RIGHT_FOOT_TRIGGER]->transform->position = m_RightFoot->position;
    m_attackTrigger[RIGHT_FOOT_TRIGGER]->transform->localScale = V3::one();
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
    if (changing->name.find(TEXT("ATK")) != tstring::npos)
    {
        m_gadableAttack = false;
    }
}

void BossAshi::OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev)
{
    if (prev && prev->name.find(TEXT("ATK")) != tstring::npos &&
        endChanged.GetPointer() == m_animator->BH_IDLE)
    {
        SetState(State::IDLE);
        m_manualLook = false;
    }

    if (prev && prev.GetPointer() == m_animator->BH_BACKJUMP)
    {
        SetState(State::ATK_FAR_RAND);
    }
}

void BossAshi::OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc)
{
    if (desc.ContextInt & BossAshiAnimator::IntContext::SWORD_START)
    {
        m_attackTrigger[SWORD_TRIGGER]->enable = true;
        SetAttackType(desc.ContextUInt);
        ClearHitBuffer();
    }
    if (desc.ContextInt & BossAshiAnimator::IntContext::SWORD_END)
    {
        OffAttackTriggers();
        ClearHitBuffer();
    }
    if (desc.ContextInt & BossAshiAnimator::IntContext::RIGHT_FOOT_START)
    {
        m_attackTrigger[RIGHT_FOOT_TRIGGER]->enable = true;
        SetAttackType(desc.ContextUInt);
        ClearHitBuffer();
    }
    if (desc.ContextInt & BossAshiAnimator::IntContext::RIGHT_FOOT_END)
    {
        OffAttackTriggers();
        ClearHitBuffer();
    }

    if (desc.ContextUInt & BossAshiAnimator::UIntContext::ATK_GADABLE)
    {
        m_gadableAttack = true;
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::START_MANUAL_LOOK)
    {
        m_manualLook = true;
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::END_MANUAL_LOOK)
    {
        m_manualLook = false;
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::ATK_COMBO)
    {
        if (m_numComboAttack > 0)
        {
            --m_numComboAttack;

            State currentAtk = m_state;
            State atk = currentAtk;
            while (currentAtk == atk)
            {
                int numATK = (int)State::__ATK_NEAR_END - (int)State::__ATK_NEAR_BEGIN;
                int indexATK = (int)State::__ATK_NEAR_BEGIN + (rand() % (numATK - 1)) + 1;
                atk = (State)indexATK;
            }
            SetState(atk);
        }
    }
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
    if (state == m_state)
        return;

    State before = m_state;
    State next = state;
    m_state = state;
    StateEnded(before, next);
    StateChanged(before, next);
}

void BossAshi::StateUpdate()
{
    float dt = system->time->deltaTime;

    switch (m_state)
    {
        case State::IDLE:
        {
            if (m_idleLeftCounter > 0.0f)
            {
                m_idleLeftCounter -= dt;
            }
            else
            {
                SetState(State::WALK_ANYWHERE);
            }
        }
        break;
        case State::WALK_ANYWHERE:
        {
            if (m_walkLeftTime > 0.0f)
            {
                m_walkLeftTime -= dt;
            }
            else
            {
                uint randomState = rand() % 2;
                State state;
                switch (randomState)
                {
                    case 0: state = State::WALK_TRACE; break;
                    case 1: state = State::RUN_TRACE; break;
                }
                SetState(state);
            }

            if (m_walkDirectionChangeLeftTime > 0.0f)
            {
                m_walkDirectionChangeLeftTime -= dt;
            }
            else
            {
                m_walkDirectionChangeLeftTime = RandomWalkDirectionChangeTime();
                static const float angles[4] = { 0.0f, 90.0f, 180.0f, 270.0f };
                uint beforeDirection = uint(m_animator->WalkDirectionFProperty->valueAsFloat * 4.0f);
                uint direction = beforeDirection;
                while (direction == beforeDirection)
                {
                    direction = rand() % 4;
                }
                float angle = angles[direction];
                SetWalkDirectionLerp(angle, 1.0f);
            }

            //if (XZDistanceBetweenPlayer() < 5.0f)
            //{
            //    SetState(State::ATK_NEAR_RAND);
            //}

            m_moveAcc += dt;
        }
        break;
        case State::WALK_TRACE:
        {
            if (m_walkLeftTime > 0.0f)
            {
                m_walkLeftTime -= dt;
            }
            else
            {
                SetState(State::RUN_TRACE);
            }

            if (XZDistanceBetweenPlayer() < 5.0f)
            {
                SetState(State::ATK_NEAR_RAND);
            }

            m_moveAcc += dt;
        }
        break;
        case State::RUN_TRACE:
        {
            if (XZDistanceBetweenPlayer() < 3.0f)
            {
                SetState(State::ATK_NEAR_RAND);
            }

            m_moveAcc += dt;
        }
        break;
        case State::ATK_RUSH:
        {
            if (XZDistanceBetweenPlayer() < 5.0f)
            {
                m_animator->ATK_RUSH_End_TProperty->SetTriggerState();
            }
        }
        break;
    }

    switch (m_state)
    {
        case State::TURN:
        case State::WALK_ANYWHERE:
        case State::WALK_TRACE:
        case State::RUN_TRACE:
        {
            RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 180.0f, system->time->deltaTime);
        }
        break;
    }

    if (m_state == State::ATK_RUSH)
    {
        if (!m_animator->Layer->IsPlaying(m_animator->ATK_RUSH_ED))
        {
            RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 180.0f, system->time->deltaTime);
        }
    }
}

void BossAshi::StateChanged(BossAshi::State before, BossAshi::State next)
{
    switch (next)
    {
        case State::IDLE:
        {
            m_idleLeftCounter = 0.3f;
            m_animator->MoveFProperty->valueAsFloat = 0.0f;
            m_animator->TurnBProperty->valueAsBool = false;
        }
        break;
        case State::TURN:
        {
            m_animator->TurnBProperty->valueAsBool = true;
        }
        break;
        case State::WALK_ANYWHERE:
        {
            m_animator->MoveFProperty->valueAsFloat = 1.0f;
            m_animator->WalkDirectionFProperty->valueAsFloat = 0.75f;
            m_walkDirectionChangeLeftTime = RandomWalkDirectionChangeTime();
            m_walkLeftTime = 2.5f + float(rand() % 5);
        }
        break;
        case State::WALK_TRACE:
        {
            m_animator->MoveFProperty->valueAsFloat = 1.0f;
            SetWalkDirectionLerp(0.0f, 1.0f);
            m_walkLeftTime = 2.5f + float(rand() % 3);
        }
        break;
        case State::RUN_TRACE:
        {
            m_animator->MoveFProperty->valueAsFloat = 2.0f;
        }
        break;
        case State::DIE:
        {
            m_animator->DieTProperty->SetTriggerState();
        }
        break;
        case State::BACKJUMP:
        case State::ATK_BACKJUMP:
        {
            m_numComboAttack = 0;
            m_animator->BackjumpTProperty->SetTriggerState();
        }
        break;
        case State::ATK_NEAR_RAND:
        {
            int numATK = (int)State::__ATK_NEAR_END - (int)State::__ATK_NEAR_BEGIN;
            int indexATK = (int)State::__ATK_NEAR_BEGIN + (rand() % (numATK - 1)) + 1;
            State atk = (State)indexATK;
            m_numComboAttack = 1 + (rand() % 5);
            SetState(atk);
        }
        break;
        case State::ATK_FAR_RAND:
        {
            int numATK = (int)State::__ATK_FAR_END - (int)State::__ATK_FAR_BEGIN - 1;

            ++m_farAttackIndex;
            if (m_farAttackIndex >= numATK) m_farAttackIndex = 0;

            int indexATK = (int)State::__ATK_FAR_BEGIN + m_farAttackIndex + 1;
            State atk = (State)indexATK;
            SetState(atk);
        }
        break;
        case State::ATK_DOUBLEHAND_SLASH:
        {
            m_animator->ATK_DOUBLEHAND_SLASH_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_H_SLASH:
        {
            m_animator->ATK_H_SLASH_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_SHOLDER_SLASH:
        {
            m_animator->ATK_SHOLDER_SLASH_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_SLASHUP:
        {
            m_animator->ATK_SLASHUP_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_DROPKICK:
        {
            m_animator->ATK_DROPKICK_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_LEGSTEP:
        {
            m_animator->ATK_LEGSTEP_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_SPINKICK:
        {
            m_animator->ATK_SPINKICK_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_LASER:
        {
            m_animator->ATK_LASER_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_FAR_RAGE:
        {
            m_numComboAttack = 0;
            m_animator->ATK_RAGE_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_NEAR_RAGE:
        {
            m_animator->ATK_ANGRY_TProperty->SetTriggerState();
        }
        break;
        case State::ATK_RUSH:
        {
            m_animator->ATK_RUSH_Start_TProperty->SetTriggerState();
        }
        break;
    }
}

void BossAshi::StateEnded(BossAshi::State before, BossAshi::State current)
{
    switch (before)
    {
        case State::IDLE:
        {
            m_idleLeftCounter = 0.0f;
        }
        break;
        case State::TURN:
        {
            m_animator->TurnBProperty->valueAsBool = false;
        }
        break;
        case State::WALK_ANYWHERE:
        {
            m_walkLeftTime = 0.0f;
        }
        break;
        case State::WALK_TRACE:
        {
            m_walkLeftTime = 0.0f;
        }
        break;
        case State::RUN_TRACE:
        {
            m_walkLeftTime = 0.0f;
        }
        break;
    }
}

void BossAshi::WalkDirectionLerp()
{
    if (m_walkDirectionLerp)
    {
        m_walkDirectionLerpAcc += system->time->deltaTime;

        float percent = m_walkDirectionLerpAcc / m_walkDirectionLerpDuration;
        float angle = LerpAngle(m_startWalkAngle, m_targetWalkAngle, percent);
        float direction = Repeat(angle, 360.0f) / 360.0f;
        m_animator->WalkDirectionFProperty->valueAsFloat = direction;
        if (m_walkDirectionLerpAcc >= m_walkDirectionLerpDuration)
        {
            m_walkDirectionLerpAcc = 0.0f;
            m_walkDirectionLerp = false;
        }
    }
}

void BossAshi::SetWalkDirectionLerp(float angle, float duration)
{
    float curDirection = m_animator->WalkDirectionFProperty->valueAsFloat;
    m_startWalkAngle = Repeat(curDirection * 360.0f, 360.0f);
    m_targetWalkAngle = angle;
    m_walkDirectionLerpDuration = duration;
    m_walkDirectionLerpAcc = 0.0f;
    m_walkDirectionLerp = true;
}

void BossAshi::StopWalkDirectionLerp()
{
    m_walkDirectionLerp = false;
}

float BossAshi::RandomWalkDirectionChangeTime() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rdTime(1.0f, 5.0f);
    return rdTime(gen);
}
