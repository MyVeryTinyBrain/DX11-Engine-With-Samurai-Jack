#include "stdafx.h"
#include "BossAshi.h"
#include "Config.h"
#include "BossAshiAnimator.h"
#include "Player.h"
#include "ProjectileWaveBeam.h"
#include "EffectRing01.h"
#include "EffectGroundImapct.h"
#include "ParticleSpark.h"
#include "TPSCamera.h"
#include "ParticleDust01.h"
#include "GameSystem.h"
#include <MaterialUtil.h>

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

    m_toPlayerDirection = transform->forward;

    GameSystem::GetInstance()->PlayMusic(system->resource->Find(SOUND_MUSIC_02), 0.3f, 3.0f);
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

    for (uint i = 0; i < m_characterRenderer->materialCount; ++i)
    {
        m_characterRenderer->materials[i]->SetFloat("_DistortionPercent", m_distortionPercent);
    }
    for (uint i = 0; i < m_swordRenderer->materialCount; ++i)
    {
        m_swordRenderer->materials[i]->SetFloat("_DistortionPercent", m_distortionPercent);
    }
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
    m_collider->radius = CCT->radius + 0.4f;
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
    MaterialUtil::SetupPBRMaterials(m_characterRenderer, system->resource->FindBinrayShader(SHADER_STANDARD_DISTORTION), 0);
    for (uint i = 0; i < m_characterRenderer->materialCount; ++i)
    {
        m_characterRenderer->materials[i]->SetTexture("_DistortionNoiseTexture", system->resource->Find(TEX_NOISE_02));
    }

    m_R_Hand_Weapon_cnt_tr = m_characterRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));
    m_RightFoot = m_characterRenderer->GetNodeTransformByName(TEXT("RightFoot"));
    m_Head = m_characterRenderer->GetNodeTransformByName(TEXT("Head"));
    
    m_Neck = m_characterRenderer->GetNodeTransformByName(TEXT("Neck"));
    m_Spine1 = m_characterRenderer->GetNodeTransformByName(TEXT("Spine1"));
    m_Spine = m_characterRenderer->GetNodeTransformByName(TEXT("Spine"));
    m_Hips = m_characterRenderer->GetNodeTransformByName(TEXT("Hips"));
    m_RightArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("RightArmRoll"));
    m_LeftArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("LeftArmRoll"));
    m_RightForeArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("RightForeArmRoll"));
    m_LeftForeArmRoll = m_characterRenderer->GetNodeTransformByName(TEXT("LeftForeArmRoll"));
    m_L_Hand_Weapon_cnt_tr = m_characterRenderer->GetNodeTransformByName(TEXT("L_Hand_Weapon_cnt_tr"));
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
    MaterialUtil::SetupPBRMaterials(m_swordRenderer, system->resource->FindBinrayShader(SHADER_STANDARD_DISTORTION), 0);
    for (uint i = 0; i < m_swordRenderer->materialCount; ++i)
    {
        m_swordRenderer->materials[i]->SetTexture("_DistortionNoiseTexture", system->resource->Find(TEX_NOISE_02));
    }

    m_goSwordTip = CreateGameObjectToChild(m_goSword->transform);
    m_goSwordTip->transform->localEulerAngles = V3(90.0f, 0.0f, 0.0f);
    m_goSwordTip->transform->localPosition = V3(0.0f, 0.0f, -1.0f);

    m_goSwordTrail = CreateGameObjectToChild(m_goSwordTip->transform);
    m_goSwordTrail->transform->localPosition = V3(0.0f, 0.5f, 0.0f);
    m_goSwordTrail->transform->localEulerAngles = V3(0.0f, 0.0f, 90.0f);
    m_swordTrail = m_goSwordTrail->AddComponent<TrailRenderer>();
    m_swordTrail->alignment = TrailRenderer::Alignment::Local;
    m_swordTrail->shrinkDistance = 30.0f;
    m_swordTrail->beginShrinkDelay = 0.17f;
    m_swordTrail->minVertexDistance = 0.2f;
    m_swordTrail->width = 2.5f;
    m_swordTrail->autoTrail = false;
    m_swordTrail->interpolateStep = 30;
    m_swordTrail->applyWidthByLength = false;
    m_swordTrail->fitVToLength = true;

    ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_SWORDTRAIL);
    ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
    ResourceRef<Texture> baseTexture = system->resource->Find(TEX_EFFECT_SWORDTRAIL);
    ResourceRef<Texture> distortionTexture = system->resource->Find(TEX_NOISE_01);
    material->SetTexture("_BaseTexture", baseTexture);
    material->SetTexture("_DistortionTexture", distortionTexture);
    material->SetFloat("_DistortionPower", 100.0f);
    material->SetColor("_MinColor", Color::RGBA255(255, 255, 194, 255));
    material->SetColor("_MaxColor", Color::RGBA255(254, 128, 0, 255));
    m_swordTrail->material = material;
}

void BossAshi::SetupAttackTrigger()
{
    GameObject* goSwordTrigger = CreateGameObjectToChild(m_goSwordTip->transform);
    goSwordTrigger->transform->localPosition = V3(0.0f, 0.6f, 0.0f);
    m_attackTrigger[SWORD_TRIGGER] = goSwordTrigger->AddComponent<CapsuleCollider>();
    m_attackTrigger[SWORD_TRIGGER]->isTrigger = true;
    m_attackTrigger[SWORD_TRIGGER]->enable = false;
    CapsuleCollider* swordTrigger = (CapsuleCollider*)m_attackTrigger[SWORD_TRIGGER];
    swordTrigger->radius = 0.5f;
    swordTrigger->halfHeight = 0.25f;

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
    m_audioSource->maxDistance = m_audioSource->minDistance + 50.0f;
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
    // Look at player
    float angle = V3::Angle(transform->forward, ToPlayerDirection());
    if (angle < 80.0f)
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
        endChanged && endChanged.GetPointer() == m_animator->BH_IDLE)
    {
        SetState(State::IDLE);
        m_manualLook = false;
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
    if (desc.ContextInt & BossAshiAnimator::IntContext::SWORDTRAIL_START)
    {
        m_swordTrail->autoTrail = true;
    }
    if (desc.ContextInt & BossAshiAnimator::IntContext::SWORDTRAIL_END)
    {
        m_swordTrail->autoTrail = false;
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

            if (XZDistanceBetweenPlayer() > 7.5f)
            {
                SetState(State::ATK_BACKJUMP);
            }
            else
            {
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
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::ATK_FAR)
    {
        SetState(State::ATK_FAR_RAND);
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::ATK_SHOOT_BEAM)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> rdYAngle(-10.0f, +10.0f);
        std::uniform_real_distribution<float> rdXZAngle(-10.0f, +10.0f);
        V3 aimDelta = player->transform->position - m_Head->position;
        V3 aimDirection = aimDelta.normalized;
        Q rdRotate = Q::Euler(rdXZAngle(gen), rdYAngle(gen), rdXZAngle(gen));
        V3 direction = rdRotate.MultiplyVector(aimDirection).normalized;

        GameObject* goBeam = CreateGameObject();
        goBeam->transform->position = m_Head->position;
        goBeam->transform->forward = direction;
        goBeam->transform->localScale = V3::one() * 0.5f;
        ProjectileWaveBeam* beam = goBeam->AddComponent<ProjectileWaveBeam>();

        EffectRing01::Create(
            gameObject->regionScene,
            m_Head->position,
            0.3f,
            0.5f,
            500.0f,
            1.0f, 2.0f,
            Color(1.0f, 0.9764f, 0.466f, 1.0f)
        );
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::ATK_GROUND_IMPACT)
    {
        PhysicsHit hit;
        PhysicsRay ray;
        ray.Point = transform->position;
        ray.Direction = V3::down();
        ray.Length = 100.0f;
        V3 position = CCT->footPosition;
        if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
        {
            position = hit.Point;
        }

        EffectGroundImapct::Create(
            gameObject->regionScene,
            position + V3::up() * 0.1f,
            0.6f,
            1.0f, 100.0f, 0.8f,
            1000.0f, Color::red(), 0.5f
        );

        EffectRing01::Create(
            gameObject->regionScene,
            m_Head->position,
            0.5f,
            0.5f,
            500.0f,
            1.0f, 5.0f,
            Color(1.0f, 0.0f, 0.0f, 1.0f)
        );
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::SWORD_DUST)
    {
        PhysicsHit hit;
        PhysicsRay ray;
        ray.Point = m_goSwordTip->transform->position;
        ray.Point.y = CCT->footPosition.y + 0.1f;
        ray.Direction = V3::down();
        ray.Length = 100.0f;
        V3 position = m_goSwordTip->transform->position;
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
            2.0f, 6.0f, 0.5f,
            Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
            uint(desc.ContextFloat));
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::RIGHT_FOOT_DUST)
    {
        PhysicsHit hit;
        PhysicsRay ray;
        ray.Point = m_attackTrigger[RIGHT_FOOT_TRIGGER]->transform->position;
        ray.Point.y = CCT->footPosition.y + 0.1f;
        ray.Direction = V3::down();
        ray.Length = 100.0f;
        V3 position = m_attackTrigger[RIGHT_FOOT_TRIGGER]->transform->position;
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
            2.0f, 6.0f, 0.5f,
            Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
            uint(desc.ContextFloat));
    }
    if (desc.ContextUInt & BossAshiAnimator::UIntContext::BODY_DUST)
    {
        PhysicsHit hit;
        PhysicsRay ray;
        ray.Point = m_Spine->position;
        ray.Point.y = CCT->footPosition.y + 0.1f;
        ray.Direction = V3::down();
        ray.Length = 100.0f;
        V3 position = m_Spine->position;
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
            2.0f, 6.0f, 0.5f,
            Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
            uint(desc.ContextFloat));
    }

    if (desc.ContextByte & BossAshiAnimator::ByteContext::CAM_SHAKE)
    {
        float shakeValue = desc.ContextFloat;
        Player* player = Player::GetInstance();
        if (player)
        {
            TPSCamera* camera = Player::GetInstance()->GetTPSCamera();
            camera->Shake(V3::up(), shakeValue * 0.1f, 1.1f, 0.15f, 4.0f / 0.15f);
        }
    }
    if (desc.ContextByte & BossAshiAnimator::ByteContext::PLAY_SOUND)
    {
        ResourceRef<AudioClip> audioClip = system->resource->Find(desc.ContextTStr);
        m_audioSource->PlayOneshot(audioClip, desc.ContextFloat);
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

float BossAshi::GetMaxHP() const
{
    return 300.0f;
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
                    direction = rand() % 3 + 1; // 1, 2, 3: 90~270
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
            if (m_animator->Layer->IsPlaying(m_animator->ATK_RUSH_LP) && XZDistanceBetweenPlayer() < 5.0f)
            {
                m_animator->ATK_RUSH_End_TProperty->SetTriggerState();
            }

            if (m_animator->Layer->currentNode.GetPointer() == m_animator->ATK_RUSH_LP)
            {
                PhysicsHit hit;
                PhysicsRay ray;
                ray.Point = m_attackTrigger[SWORD_TRIGGER]->transform->position;
                ray.Point.y = CCT->footPosition.y + 0.1f;
                ray.Direction = V3::down();
                ray.Length = 100.0f;
                V3 position = ray.Point;
                if (system->physics->query->Raycast(hit, ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider))
                {
                    position = hit.Point;
                }

                float randomAngle = float(rand() % 361);
                float randomRadian = randomAngle * Deg2Rad;
                float randomDistance = float(rand() % 101) / 100.0f * 0.5f;
                V3 adjustDirection = V3(Cos(randomRadian), 0.0f, Sin(randomRadian));

                ParticleSpark::Create(
                    gameObject->regionScene,
                    position + randomDistance * adjustDirection,
                    V3::up(),
                    0.0f, 45.0f,
                    1.0f, 5.0f,
                    2.0f, 9.0f,
                    0.1f, 0.5f,
                    1.0f, 3.0f,
                    1,
                    false);
            }
        }
        break;
        case State::DIE:
        {
            if (m_animator->Layer->IsPlaying(m_animator->DMG_DIE_ED) && m_animator->DMG_DIE_ED->normalizedTime >= 1.0f)
            {
                m_distortionPercent += system->time->deltaTime / 5.0f;
                if (m_distortionPercent >= 1.0f)
                {
                    gameObject->Destroy();
                }
            }
        }
        break;
    }

    switch (m_state)
    {
        case State::WALK_ANYWHERE:
        case State::WALK_TRACE:
        {
            RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 50.0f, system->time->deltaTime);
        }
        break;
        case State::TURN:
        case State::RUN_TRACE:
        {
            RotateOnYAxisToDirection(ToPlayerDirectionXZ(), 100.0f, system->time->deltaTime);
        }
        break;
    }
}

void BossAshi::StateChanged(BossAshi::State before, BossAshi::State next)
{
    switch (next)
    {
        case State::IDLE:
        {
            m_idleLeftCounter = 0.1f;
            m_animator->MoveFProperty->valueAsFloat = 0.0f;
            m_animator->TurnBProperty->valueAsBool = false;
            m_animator->Layer->OffAllTriggers();
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
            uint random = (rand() % 3);
            float dir = random * 0.25f + 0.25f;
            m_animator->WalkDirectionFProperty->valueAsFloat = dir;
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
            GameSystem::GetInstance()->StopMusic(3.0f);
            Enemy::UnregistEnemy(this);
            m_animator->Layer->OffAllTriggers();
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

            if (atk == State::ATK_BACKJUMP)
                atk = State::ATK_DOUBLEHAND_SLASH;

            m_numComboAttack = 1 + (rand() % 5);

            uint addCombo = 0;
            float hpPercent = hp / maxHP;
            if (hpPercent > 0.9f) addCombo = 0;
            else if (hpPercent > 0.8f) addCombo = 2;
            else if (hpPercent > 0.7f) addCombo = 4;
            else if (hpPercent > 0.6f) addCombo = 5;
            else if (hpPercent > 0.5f) addCombo = 7;
            else if (hpPercent > 0.4f) addCombo = 9;
            else if (hpPercent > 0.3f) addCombo = 10;
            else if (hpPercent > 0.2f) addCombo = 12;
            else if (hpPercent > 0.1f) addCombo = 15;
            m_numComboAttack += addCombo;
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

void BossAshi::MakeDarkDust()
{
    Ref<NodeTransform> nodes[11] =
    {
        m_Head,
        m_Neck,
        m_Spine1,
        m_Spine,
        m_Hips,
        m_RightArmRoll,
        m_LeftArmRoll,
        m_RightForeArmRoll,
        m_LeftForeArmRoll,
        m_R_Hand_Weapon_cnt_tr,
        m_L_Hand_Weapon_cnt_tr,
    };
    uint index = rand() % 11;
    Ref<NodeTransform> node = nodes[index];

    float scaleFactor = float(rand() % 101) / 100.0f;
    scaleFactor = Max(0.1f, scaleFactor);

    ParticleDust01::CreateOnce(
        gameObject->regionScene,
        node->position,
        V3::up() * 2.0f, 1.0f,
        1.0f, 
        1.0f * scaleFactor, 3.0f * scaleFactor, 0.5f,
        Color(0.0f, 0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
        );
}
