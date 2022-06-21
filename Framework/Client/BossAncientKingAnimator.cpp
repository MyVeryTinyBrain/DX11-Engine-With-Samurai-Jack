#include "stdafx.h"
#include "BossAncientKingAnimator.h"
#include "Config.h"

void BossAncientKingAnimator::Awake()
{
    Animator::Awake();
    SetupMesh();
    SetupLayer();
    SetupProperties();
    SetupNodes();
    SetupTransitions();
}

SubResourceRef<AnimationClip> BossAncientKingAnimator::GetClip(const tstring& name)
{
    return Mesh->GetAnimationClipByName(TEXT("ROOT|") + name);
}

void BossAncientKingAnimator::SetupMesh()
{
    Mesh = system->resource->Find(MESH_ANCIENT_KING);
}

void BossAncientKingAnimator::SetupLayer()
{
    Layer = new AnimatorLayer;
    AddLayer(Layer);
    Layer->SetRootNodeByName(TEXT("CharacterRoot"));

    // Additive ===============================================================================================================

    AdditiveLayer = new AnimatorLayer(TEXT("AdditiveLayer"), AnimatorLayer::AnimateType::Additive);
    AddLayer(AdditiveLayer);
}

void BossAncientKingAnimator::SetupProperties()
{
    TurnBProperty = AdditiveLayer->AddProperty(TEXT("TurnB"), AnimatorProperty::Type::BOOL);
    WalkBProperty = AdditiveLayer->AddProperty(TEXT("WalkB"), AnimatorProperty::Type::BOOL);
    
    KeepAttackBProperty = AdditiveLayer->AddProperty(TEXT("KeepAttackB"), AnimatorProperty::Type::BOOL);

    ATK_TURN_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_TURN_T"), AnimatorProperty::Type::TRIGGER);
    ATK_SWING_H_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_SWING_H_T"), AnimatorProperty::Type::TRIGGER);
    ATK_SWING_V_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_SWING_V_T"), AnimatorProperty::Type::TRIGGER);
    ATK_STOMP_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_STOMP_T"), AnimatorProperty::Type::TRIGGER);
    ATK_STEPON_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_STEPON_T"), AnimatorProperty::Type::TRIGGER);
    ATK_RUSH_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_RUSH_T"), AnimatorProperty::Type::TRIGGER);
    ATK_JUMP_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_JUMP_T"), AnimatorProperty::Type::TRIGGER);
    ATK_BACKJUMP_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_BACKJUMP_T"), AnimatorProperty::Type::TRIGGER);
    ATK_ELECTRIC_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_ELECTRIC_T"), AnimatorProperty::Type::TRIGGER);
    ATK_DOWNSTRIKE_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_DOWNSTRIKE_T"), AnimatorProperty::Type::TRIGGER);
    ATK_BEAM_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_BEAM_T"), AnimatorProperty::Type::TRIGGER);

    GROGY_TProperty = AdditiveLayer->AddProperty(TEXT("GROGY_T"), AnimatorProperty::Type::TRIGGER);
    DIE_TProperty = AdditiveLayer->AddProperty(TEXT("DIE_T"), AnimatorProperty::Type::TRIGGER);
    RAGE_TProperty = AdditiveLayer->AddProperty(TEXT("RAGE_T"), AnimatorProperty::Type::TRIGGER);

    // Additive ===============================================================================================================

    AdditiveDamageTProperty = AdditiveLayer->AddProperty(TEXT("AdditiveDamageT"), AnimatorProperty::Type::TRIGGER);
}

void BossAncientKingAnimator::SetupNodes()
{
    BH_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_IDLE")), LOOP);
    Layer->AddNode(BH_IDLE);

    BH_TURN_ROTATE = AnimatorSingleNode::Create(GetClip(TEXT("BH_TURN_ROTATE")), NOLOOP);
    {
        AnimationEventDesc e0, e1;
        e0.NormalizedTime = 22 / 44.0f;
        e1.NormalizedTime = 42 / 44.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e1.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 0.5f;
        e1.ContextFloat = 0.5f;
        BH_TURN_ROTATE->AddEvent(e0);
        BH_TURN_ROTATE->AddEvent(e1);

        AnimationEventDesc s0, s1;
        s0.NormalizedTime = 22 / 44.0f;
        s1.NormalizedTime = 42 / 44.0f;
        s0.ContextByte = ByteContext::PLAY_SOUND;
        s1.ContextByte = ByteContext::PLAY_SOUND;
        s0.ContextTStr = SOUND_CRASH_01;
        s1.ContextTStr = SOUND_CRASH_01;
        s0.ContextFloat = 1.0f;
        s1.ContextFloat = 1.0f;
        BH_TURN_ROTATE->AddEvent(s0);
        BH_TURN_ROTATE->AddEvent(s1);
    }
    Layer->AddNode(BH_TURN_ROTATE);

    BH_TURN_WAIT = AnimatorSingleNode::Create(GetClip(TEXT("BH_TURN_WAIT")), NOLOOP);
    Layer->AddNode(BH_TURN_WAIT);

    BH_WLK_ST = AnimatorSingleNode::Create(GetClip(TEXT("BH_WLK_ST")), NOLOOP);
    {
        AnimationEventDesc e0, e1;
        e0.NormalizedTime = 17 / 60.0f;
        e1.NormalizedTime = 44 / 60.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e1.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 0.5f;
        e1.ContextFloat = 0.5f;
        BH_WLK_ST->AddEvent(e0);
        BH_WLK_ST->AddEvent(e1);

        AnimationEventDesc s0, s1;
        s0.NormalizedTime = 16 / 60.0f;
        s1.NormalizedTime = 44 / 60.0f;
        s0.ContextByte = ByteContext::PLAY_SOUND;
        s1.ContextByte = ByteContext::PLAY_SOUND;
        s0.ContextTStr = SOUND_CRASH_01;
        s1.ContextTStr = SOUND_CRASH_01;
        s0.ContextFloat = 1.0f;
        s1.ContextFloat = 1.0f;        
        BH_WLK_ST->AddEvent(s0);
        BH_WLK_ST->AddEvent(s1);
    }
    Layer->AddNode(BH_WLK_ST);

    BH_WLK_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WLK_LP")), LOOP);
    {
        AnimationEventDesc e0, e1;
        e0.NormalizedTime = 24 / 80.0f;
        e1.NormalizedTime = 65 / 80.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e1.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 0.5f;
        e1.ContextFloat = 0.5f;
        BH_WLK_LP->AddEvent(e0);
        BH_WLK_LP->AddEvent(e1);

        AnimationEventDesc s0, s1;
        s0.NormalizedTime = 24 / 80.0f;
        s1.NormalizedTime = 65 / 80.0f;
        s0.ContextByte = ByteContext::PLAY_SOUND;
        s1.ContextByte = ByteContext::PLAY_SOUND;
        s0.ContextTStr = SOUND_CRASH_01;
        s1.ContextTStr = SOUND_CRASH_01;
        s0.ContextFloat = 1.0f;
        s1.ContextFloat = 1.0f;
        BH_WLK_LP->AddEvent(s0);
        BH_WLK_LP->AddEvent(s1);
    }
    Layer->AddNode(BH_WLK_LP);

    BH_WLK_ED = AnimatorSingleNode::Create(GetClip(TEXT("BH_WLK_ED")), NOLOOP);
    {
        AnimationEventDesc e0, e1;
        e0.NormalizedTime = 24 / 70.0f;
        e1.NormalizedTime = 60 / 70.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e1.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 0.5f;
        e1.ContextFloat = 0.5f;
        BH_WLK_ED->AddEvent(e0);
        BH_WLK_ED->AddEvent(e1);

        AnimationEventDesc s0, s1;
        s0.NormalizedTime = 24 / 70.0f;
        s1.NormalizedTime = 60 / 70.0f;
        s0.ContextByte = ByteContext::PLAY_SOUND;
        s1.ContextByte = ByteContext::PLAY_SOUND;
        s0.ContextTStr = SOUND_CRASH_01;
        s1.ContextTStr = SOUND_CRASH_01;
        s0.ContextFloat = 1.0f;
        s1.ContextFloat = 1.0f;
        BH_WLK_ED->AddEvent(s0);
        BH_WLK_ED->AddEvent(s1);
    }
    Layer->AddNode(BH_WLK_ED);

    DMG_GROGY_ST = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_ST")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 15 / 35.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextUInt = UIntContext::RF_DUST | UIntContext::LF_DUST | UIntContext::HAMMER_DUST;
        e0.ContextFloat = 1.0f;
        DMG_GROGY_ST->AddEvent(e0);

        AnimationEventDesc s;
        s.NormalizedTime = 15 / 35.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_CRASH_01;
        s.ContextFloat = 1.0f;
        DMG_GROGY_ST->AddEvent(s);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 0.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_GROGY;
            s.ContextFloat = 1.0f;
            DMG_GROGY_ST->AddEvent(s);
        }
    }
    Layer->AddNode(DMG_GROGY_ST);

    DMG_GROGY_LP = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_LP")), LOOP);
    Layer->AddNode(DMG_GROGY_LP);

    DMG_GROGY_ED = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_ED")), NOLOOP);
    {
        AnimationEventDesc s;
        s.NormalizedTime = 0.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_ANCIENT_KING_ATTACK08;
        s.ContextFloat = 1.0f;
        DMG_GROGY_ED->AddEvent(s);
    }
    Layer->AddNode(DMG_GROGY_ED);

    DMG_GROGY_HIT = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_HIT")), NOLOOP);
    Layer->AddNode(DMG_GROGY_HIT);

    DMG_DIE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_DIE")), NOLOOP);
    {
        {
            AnimationEventDesc e0;
            e0.NormalizedTime = 79 / 100.0f;
            e0.ContextByte = ByteContext::CAM_SHAKE;
            e0.ContextFloat = 1.0f;
            DMG_DIE->AddEvent(e0);

            AnimationEventDesc s;
            s.NormalizedTime = 79 / 100.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_CRASH_01;
            s.ContextFloat = 1.0f;
            DMG_DIE->AddEvent(s);
        }

        {
            AnimationEventDesc e0;
            e0.NormalizedTime = 88 / 100.0f;
            e0.ContextByte = ByteContext::CAM_SHAKE;
            e0.ContextUInt = UIntContext::RF_DUST | UIntContext::LF_DUST | UIntContext::HAMMER_DUST;
            e0.ContextFloat = 2.0f;
            DMG_DIE->AddEvent(e0);

            AnimationEventDesc s;
            s.NormalizedTime = 88 / 100.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_CRASH_02;
            s.ContextFloat = 1.0f;
            DMG_DIE->AddEvent(s);
        }

        {
            AnimationEventDesc s;
            s.NormalizedTime = 0.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_DIE;
            s.ContextFloat = 1.0f;
            DMG_DIE->AddEvent(s);
        }
    }
    Layer->AddNode(DMG_DIE);
  
    ATK_TURN = AnimatorSingleNode::Create(GetClip(TEXT("ATK_TURN")), NOLOOP);
    {
        {
            AnimationEventDesc e0;
            e0.NormalizedTime = 10 / 75.0f;
            e0.ContextByte = ByteContext::CAM_SHAKE;
            e0.ContextFloat = 0.5f;
            ATK_TURN->AddEvent(e0);
        }
        {
            AnimationEventDesc e0;
            e0.NormalizedTime = 29 / 75.0f;
            e0.ContextByte = ByteContext::CAM_SHAKE;
            e0.ContextFloat = 0.5f;
            ATK_TURN->AddEvent(e0);
        }

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 20 / 75.0f;
        a1.NormalizedTime = 33 / 75.0f;
        a0.ContextInt = IntContext::HAMMER_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::HAMMER_END;
        ATK_TURN->AddEvent(a0);
        ATK_TURN->AddEvent(a1);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 10 / 75.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_CRASH_01;
            s.ContextFloat = 1.0f;
            ATK_TURN->AddEvent(s);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 29 / 75.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_CRASH_01;
            s.ContextFloat = 1.0f;
            ATK_TURN->AddEvent(s);
        }
        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 20 / 75.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_01;
            swingSound.ContextFloat = 1.0f;
            ATK_TURN->AddEvent(swingSound);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 10 / 75.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK05;
            s.ContextFloat = 1.0f;
            ATK_TURN->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_TURN);

    ATK_SWING_L = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SWING_L")), NOLOOP);
    {
        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 27 / 100.0f;
        a1.NormalizedTime = 38 / 100.0f;
        a0.ContextInt = IntContext::HAMMER_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::HAMMER_END;
        ATK_SWING_L->AddEvent(a0);
        ATK_SWING_L->AddEvent(a1);

        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 27 / 100.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_01;
            swingSound.ContextFloat = 1.0f;
            ATK_SWING_L->AddEvent(swingSound);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 25 / 100.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK06;
            s.ContextFloat = 1.0f;
            ATK_SWING_L->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_SWING_L);

    ATK_SWING_R = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SWING_R")), NOLOOP);
    {
        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 18 / 89.0f;
        a1.NormalizedTime = 34 / 89.0f;
        a0.ContextInt = IntContext::HAMMER_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::HAMMER_END;
        ATK_SWING_R->AddEvent(a0);
        ATK_SWING_R->AddEvent(a1);

        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 18 / 89.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_01;
            swingSound.ContextFloat = 1.0f;
            ATK_SWING_R->AddEvent(swingSound);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 5 / 89.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK07;
            s.ContextFloat = 1.0f;
            ATK_SWING_R->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_SWING_R);
        
    ATK_SWING_V = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SWING_V")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 39 / 85.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextUInt = UIntContext::HAMMER_DUST;
        e0.ContextFloat = 1.0f;
        ATK_SWING_V->AddEvent(e0);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 35 / 85.0f;
        a1.NormalizedTime = 39 / 85.0f;
        a0.ContextInt = IntContext::HAMMER_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::HAMMER_END;
        ATK_SWING_V->AddEvent(a0);
        ATK_SWING_V->AddEvent(a1);

        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 34 / 85.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_01;
            swingSound.ContextFloat = 1.0f;
            ATK_SWING_V->AddEvent(swingSound);
        }
        {
            AnimationEventDesc crashSound;
            crashSound.NormalizedTime = 39 / 85.0f;
            crashSound.ContextByte = ByteContext::PLAY_SOUND;
            crashSound.ContextTStr = SOUND_CRASH_02;
            crashSound.ContextFloat = 1.0f;
            ATK_SWING_V->AddEvent(crashSound);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 30 / 85.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK08;
            s.ContextFloat = 1.0f;
            ATK_SWING_V->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_SWING_V);

    ATK_STOMP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_STOMP")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 78 / 260.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 1.0f;
        ATK_STOMP->AddEvent(e0);

        AnimationEventDesc shockwave;
        shockwave.NormalizedTime = 78 / 260.0f;
        shockwave.ContextByte = ByteContext::HAMMER_SHOCKWAVE;
        shockwave.ContextUInt = UIntContext::HAMMER_DUST;
        ATK_STOMP->AddEvent(shockwave);

        for (uint i = 0; i < 3; ++i)
        {
            AnimationEventDesc groundimpact0;
            groundimpact0.NormalizedTime = (78 + i * 6) / 260.0f;
            groundimpact0.ContextUInt = UIntContext::HAMMER_GROUND_IMPACT;
            ATK_STOMP->AddEvent(groundimpact0);

            AnimationEventDesc impactSound;
            impactSound.NormalizedTime = groundimpact0.NormalizedTime;
            impactSound.ContextByte = ByteContext::PLAY_SOUND;
            impactSound.ContextTStr = SOUND_IMPACT_01;
            impactSound.ContextFloat = 1.0f;
            ATK_STOMP->AddEvent(impactSound);
        }

        AnimationEventDesc sphereAttack0, sphereAttack1;
        sphereAttack0.NormalizedTime = 78 / 260.0f;
        sphereAttack1.NormalizedTime = 80 / 260.0f;
        sphereAttack0.ContextInt = IntContext::HAMMER_TIP_START;
        sphereAttack1.ContextInt = IntContext::HAMMER_TIP_END;
        ATK_STOMP->AddEvent(sphereAttack0);
        ATK_STOMP->AddEvent(sphereAttack1);

		AnimationEventDesc a0, a1;
		a0.NormalizedTime = 77 / 260.0f;
		a1.NormalizedTime = 82 / 260.0f;
        a0.ContextInt = IntContext::HAMMER_START;
        a0.ContextUInt = UIntContext::ATK_BLOW;
		a1.ContextInt = IntContext::HAMMER_END;
		ATK_STOMP->AddEvent(a0);
		ATK_STOMP->AddEvent(a1);

        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 72 / 260.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_01;
            swingSound.ContextFloat = 1.0f;
            ATK_STOMP->AddEvent(swingSound);
        }
        {
            AnimationEventDesc crashSound;
            crashSound.NormalizedTime = 78 / 260.0f;
            crashSound.ContextByte = ByteContext::PLAY_SOUND;
            crashSound.ContextTStr = SOUND_CRASH_02;
            crashSound.ContextFloat = 1.0f;
            ATK_STOMP->AddEvent(crashSound);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 35 / 260.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_BEGIN01;
            s.ContextFloat = 1.0f;
            ATK_STOMP->AddEvent(s);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 75 / 260.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK01;
            s.ContextFloat = 1.0f;
            ATK_STOMP->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_STOMP);

    ATK_STEPON_L = AnimatorSingleNode::Create(GetClip(TEXT("ATK_STEPON_L")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 59 / 123.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE | ByteContext::LF_SHOCKWAVE;
        e0.ContextUInt = UIntContext::LF_DUST;
        e0.ContextFloat = 1.0f;
        ATK_STEPON_L->AddEvent(e0);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 59 / 123.0f;
        a1.NormalizedTime = 61 / 123.0f;
        a0.ContextInt = IntContext::LF_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::LF_END;
        ATK_STEPON_L->AddEvent(a0);
        ATK_STEPON_L->AddEvent(a1);

        AnimationEventDesc s;
        s.NormalizedTime = 59 / 123.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_CRASH_01;
        s.ContextFloat = 1.5f;
        ATK_STEPON_L->AddEvent(s);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 38 / 110.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK04;
            s.ContextFloat = 1.0f;
            ATK_STEPON_L->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_STEPON_L);

    ATK_STEPON_R = AnimatorSingleNode::Create(GetClip(TEXT("ATK_STEPON_R")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 40 / 110.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE | ByteContext::RF_SHOCKWAVE;
        e0.ContextUInt = UIntContext::RF_DUST;
        e0.ContextFloat = 1.0f;
        ATK_STEPON_R->AddEvent(e0);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 40 / 123.0f;
        a1.NormalizedTime = 43 / 123.0f;
        a0.ContextInt = IntContext::RF_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::RF_END;
        ATK_STEPON_R->AddEvent(a0);
        ATK_STEPON_R->AddEvent(a1);

        AnimationEventDesc s;
        s.NormalizedTime = 40 / 110.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_CRASH_01;
        s.ContextFloat = 1.5f;
        ATK_STEPON_R->AddEvent(s);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 38 / 110.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK03;
            s.ContextFloat = 1.0f;
            ATK_STEPON_R->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_STEPON_R);

    ATK_RUSH_ST = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_ST")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 33 / 35.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 0.5f;
        ATK_RUSH_ST->AddEvent(e0);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 5 / 35.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_RAGE01;
            s.ContextFloat = 1.0f;
            ATK_RUSH_ST->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_RUSH_ST);

    ATK_RUSH_LP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_LP")), LOOP);
    {
        AnimationEventDesc e0, e1;
        e0.NormalizedTime = 3 / 20.0f;
        e1.NormalizedTime = 13 / 20.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e1.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 0.5f;
        e1.ContextFloat = 0.5f;
        ATK_RUSH_LP->AddEvent(e0);
        ATK_RUSH_LP->AddEvent(e1);

        AnimationEventDesc s0, s1;
        s0.NormalizedTime = 3 / 20.0f;
        s1.NormalizedTime = 13 / 20.0f;
        s0.ContextByte = ByteContext::PLAY_SOUND;
        s1.ContextByte = ByteContext::PLAY_SOUND;
        s0.ContextTStr = SOUND_CRASH_01;
        s1.ContextTStr = SOUND_CRASH_01;
        s0.ContextFloat = 1.0f;
        s1.ContextFloat = 1.0f;
        ATK_RUSH_LP->AddEvent(s0);
        ATK_RUSH_LP->AddEvent(s1);
    }
    Layer->AddNode(ATK_RUSH_LP);

    ATK_RUSH_ED = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_ED")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 17 / 52.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextFloat = 1.0f;
        ATK_RUSH_ED->AddEvent(e0);
    }
    Layer->AddNode(ATK_RUSH_ED);

    ATK_JUMP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_JUMP")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 33 / 110.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE | ByteContext::LF_SHOCKWAVE | ByteContext::RF_SHOCKWAVE;
        e0.ContextUInt = UIntContext::LF_DUST | UIntContext::RF_DUST;
        e0.ContextFloat = 1.0f;
        ATK_JUMP->AddEvent(e0);

        AnimationEventDesc groundimpact;
        groundimpact.NormalizedTime = 33 / 110.0f;
        groundimpact.ContextUInt = UIntContext::LF_GROUND_IMPACT | UIntContext::RF_GROUND_IMPACT;
        ATK_JUMP->AddEvent(groundimpact);

        AnimationEventDesc s;
        s.NormalizedTime = 33 / 110.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_CRASH_01;
        s.ContextFloat = 1.5f;
        ATK_JUMP->AddEvent(s);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 17 / 110.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK02;
            s.ContextFloat = 1.0f;
            ATK_JUMP->AddEvent(s);
        }
        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 17 / 110.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_02;
            swingSound.ContextFloat = 0.5f;
            ATK_JUMP->AddEvent(swingSound);
        }
        {
            AnimationEventDesc impactSound;
            impactSound.NormalizedTime = 33 / 110.0f;
            impactSound.ContextByte = ByteContext::PLAY_SOUND;
            impactSound.ContextTStr = SOUND_IMPACT_01;
            impactSound.ContextFloat = 1.0f;
            ATK_JUMP->AddEvent(impactSound);
        }
    }
    Layer->AddNode(ATK_JUMP);

    ATK_BACKJUMP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_BACKJUMP")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 33 / 110.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE | ByteContext::LF_SHOCKWAVE | ByteContext::RF_SHOCKWAVE;
        e0.ContextUInt = UIntContext::LF_DUST | UIntContext::RF_DUST;
        e0.ContextFloat = 1.0f;
        ATK_BACKJUMP->AddEvent(e0);

        AnimationEventDesc groundimpact;
        groundimpact.NormalizedTime = 33 / 110.0f;
        groundimpact.ContextUInt = UIntContext::LF_GROUND_IMPACT | UIntContext::RF_GROUND_IMPACT;
        ATK_BACKJUMP->AddEvent(groundimpact);

        AnimationEventDesc s;
        s.NormalizedTime = 33 / 110.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_CRASH_01;
        s.ContextFloat = 1.5f;
        ATK_BACKJUMP->AddEvent(s);

        AnimationEventDesc bjend;
        bjend.NormalizedTime = 45 / 110.0f;
        bjend.ContextByte = ByteContext::BACKJUMP_END;
        ATK_BACKJUMP->AddEvent(bjend);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 17 / 110.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK02;
            s.ContextFloat = 1.0f;
            ATK_BACKJUMP->AddEvent(s);
        }
        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 17 / 110.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_02;
            swingSound.ContextFloat = 0.5f;
            ATK_BACKJUMP->AddEvent(swingSound);
        }
        {
            AnimationEventDesc impactSound;
            impactSound.NormalizedTime = 33 / 110.0f;
            impactSound.ContextByte = ByteContext::PLAY_SOUND;
            impactSound.ContextTStr = SOUND_IMPACT_01;
            impactSound.ContextFloat = 1.0f;
            ATK_BACKJUMP->AddEvent(impactSound);
        }
    }
    Layer->AddNode(ATK_BACKJUMP);

    ATK_ELECTRIC = AnimatorSingleNode::Create(GetClip(TEXT("ATK_ELECTRIC")), NOLOOP);
    {
        AnimationEventDesc a0, a1, a2;
        a0.NormalizedTime = 45 / 123.0f;
        a1.NormalizedTime = 50 / 123.0f;
        a2.NormalizedTime = 75 / 123.0f;
        a0.ContextUInt = UIntContext::SET_AIM_ON_RH;
        a1.ContextUInt = UIntContext::ATK_ELECTRIC_START;
        a2.ContextUInt = UIntContext::ATK_ELECTRIC_END;
        ATK_ELECTRIC->AddEvent(a0);
        ATK_ELECTRIC->AddEvent(a1);
        ATK_ELECTRIC->AddEvent(a2);

        {
            AnimationEventDesc e0, e1;
            e0.NormalizedTime = 12 / 123.0f;
            e1.NormalizedTime = 30 / 123.0f;
            e0.ContextUInt = UIntContext::MANUALLOOK_START;
            e1.ContextUInt = UIntContext::MANUALLOOK_END;
            ATK_ELECTRIC->AddEvent(e0);
            ATK_ELECTRIC->AddEvent(e1);
        }
        {
            AnimationEventDesc e0, e1;
            e0.NormalizedTime = 43 / 123.0f;
            e1.NormalizedTime = 49 / 123.0f;
            e0.ContextUInt = UIntContext::MANUALLOOK_START;
            e1.ContextUInt = UIntContext::MANUALLOOK_END;
            ATK_ELECTRIC->AddEvent(e0);
            ATK_ELECTRIC->AddEvent(e1);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 50 / 123.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ELECTRIC_02;
            s.ContextFloat = 1.0f;
            ATK_ELECTRIC->AddEvent(s);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 5.0f / 123.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_BEGIN01;
            s.ContextFloat = 1.0f;
            ATK_ELECTRIC->AddEvent(s);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 44 / 123.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK03;
            s.ContextFloat = 1.0f;
            ATK_ELECTRIC->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_ELECTRIC);

    ATK_DOWNSTRIKE_L = AnimatorSingleNode::Create(GetClip(TEXT("ATK_DOWNSTRIKE_L")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 52 / 115.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextUInt = UIntContext::HAMMER_DUST;
        e0.ContextFloat = 1.0f;
        ATK_DOWNSTRIKE_L->AddEvent(e0);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 48 / 115.0f;
        a1.NormalizedTime = 52 / 115.0f;
        a0.ContextInt = IntContext::HAMMER_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::HAMMER_END;
        ATK_DOWNSTRIKE_L->AddEvent(a0);
        ATK_DOWNSTRIKE_L->AddEvent(a1);

        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 46 / 115.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_01;
            swingSound.ContextFloat = 1.0f;
            ATK_DOWNSTRIKE_L->AddEvent(swingSound);
        }
        {
            AnimationEventDesc crashSound;
            crashSound.NormalizedTime = 52 / 115.0f;
            crashSound.ContextByte = ByteContext::PLAY_SOUND;
            crashSound.ContextTStr = SOUND_CRASH_02;
            crashSound.ContextFloat = 1.0f;
            ATK_DOWNSTRIKE_L->AddEvent(crashSound);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 40 / 115.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK07;
            s.ContextFloat = 1.0f;
            ATK_DOWNSTRIKE_L->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_DOWNSTRIKE_L);

    ATK_DOWNSTRIKE_R = AnimatorSingleNode::Create(GetClip(TEXT("ATK_DOWNSTRIKE_R")), NOLOOP);
    {
        AnimationEventDesc e0;
        e0.NormalizedTime = 49 / 120.0f;
        e0.ContextByte = ByteContext::CAM_SHAKE;
        e0.ContextUInt = UIntContext::HAMMER_DUST;
        e0.ContextFloat = 1.0f;
        ATK_DOWNSTRIKE_R->AddEvent(e0);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 44 / 110.0f;
        a1.NormalizedTime = 50 / 110.0f;
        a0.ContextInt = IntContext::HAMMER_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::HAMMER_END;
        ATK_DOWNSTRIKE_R->AddEvent(a0);
        ATK_DOWNSTRIKE_R->AddEvent(a1);

        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 43 / 115.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_01;
            swingSound.ContextFloat = 1.0f;
            ATK_DOWNSTRIKE_R->AddEvent(swingSound);
        }
        {
            AnimationEventDesc crashSound;
            crashSound.NormalizedTime = 49 / 120.0f;
            crashSound.ContextByte = ByteContext::PLAY_SOUND;
            crashSound.ContextTStr = SOUND_CRASH_02;
            crashSound.ContextFloat = 1.0f;
            ATK_DOWNSTRIKE_R->AddEvent(crashSound);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 40 / 120.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK06;
            s.ContextFloat = 1.0f;
            ATK_DOWNSTRIKE_R->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_DOWNSTRIKE_R);

    ATK_BEAM = AnimatorSingleNode::Create(GetClip(TEXT("ATK_BEAM")), NOLOOP);
    {
        AnimationEventDesc a;
        a.NormalizedTime = float(40) / 90.0f;
        a.ContextUInt = UIntContext::SET_AIM_ON_HEAD;
        ATK_BEAM->AddEvent(a);

        for (uint i = 0; i < 13; ++i)
        {
            AnimationEventDesc e;
            e.NormalizedTime = float(42 + i) / 90.0f;
            e.ContextUInt = UIntContext::ATK_SHOOT_BEAM;
            ATK_BEAM->AddEvent(e);
        }

        AnimationEventDesc e0, e1;
        e0.NormalizedTime = 18 / 90.0f;
        e1.NormalizedTime = 37 / 90.0f;
        e0.ContextUInt = UIntContext::MANUALLOOK_START;
        e1.ContextUInt = UIntContext::MANUALLOOK_END;
        ATK_BEAM->AddEvent(e0);
        ATK_BEAM->AddEvent(e1);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 42 / 123.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ELECTRIC_01;
            s.ContextFloat = 1.0f;
            ATK_BEAM->AddEvent(s);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 1.0f / 123.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_BEGIN02;
            s.ContextFloat = 1.0f;
            ATK_BEAM->AddEvent(s);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 34 / 123.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_ATTACK01;
            s.ContextFloat = 1.0f;
            ATK_BEAM->AddEvent(s);
        }
    }
    Layer->AddNode(ATK_BEAM);

    ETC_RAGE = AnimatorSingleNode::Create(GetClip(TEXT("ETC_RAGE")), NOLOOP);
    {
        {
            AnimationEventDesc s;
            s.NormalizedTime = 19 / 104.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_RAGE01;
            s.ContextFloat = 1.0f;
            ETC_RAGE->AddEvent(s);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 53 / 104.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_ANCIENT_KING_RAGE02;
            s.ContextFloat = 1.0f;
            ETC_RAGE->AddEvent(s);
        }
    }
    Layer->AddNode(ETC_RAGE);

    // Additive ===============================================================================================================

    ADDITIVE_EMPTY = AnimatorEmptyNode::Create(TEXT("ADDITIVE_EMPTY"), 0.0f, LOOP);
    AdditiveLayer->AddNode(ADDITIVE_EMPTY);

    ADDITIVE_DMG_ADDITIVE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_ADDITIVE")), NOLOOP);
    AdditiveLayer->AddNode(ADDITIVE_DMG_ADDITIVE);
}

void BossAncientKingAnimator::SetupTransitions()
{
    // BH_IDLE -> BH_TURN_ROTATE
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_TURN_ROTATE, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_TURN_ROTATE -> BH_TURN_WAIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_TURN_ROTATE, BH_TURN_WAIT, values, 1.0f, 0.0f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_TURN_ROTATE -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_TURN_ROTATE, EXIT, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_TURN_WAIT -> BH_TURN_ROTATE
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_TURN_WAIT, BH_TURN_ROTATE, values, 1.0f, 0.0f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_TURN_WAIT -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_TURN_WAIT, EXIT, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_IDLE -> BH_WLK_ST
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(WalkBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_WLK_ST, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WLK_ST -> BH_WLK_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(WalkBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_WLK_ST, BH_WLK_LP, values, 1.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WLK_ST -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(WalkBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_WLK_ST, EXIT, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WLK_LP -> BH_WLK_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(WalkBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_WLK_LP, BH_WLK_ED, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WLK_ED -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(BH_WLK_ED, EXIT, values, 1.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_TURN
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_TURN_TProperty));
        Layer->AddTransition(ANY, ATK_TURN, values, 0.0f, 0.05f);
    }

    // ATK_TURN -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_TURN, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_SWING_L
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_SWING_H_TProperty));
        Layer->AddTransition(ANY, ATK_SWING_L, values, 0.0f, 0.05f);
    }

    // ATK_SWING_L -> ATK_SWING_R
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(KeepAttackBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(ATK_SWING_L, ATK_SWING_R, values, 0.4f, 0.05f);
    }

    // ATK_SWING_L -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_SWING_L, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ATK_SWING_R -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_SWING_R, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_SWING_V
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_SWING_V_TProperty));
        Layer->AddTransition(ANY, ATK_SWING_V, values, 0.0f, 0.05f);
    }

    // ATK_SWING_V -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_SWING_V, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_STOMP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_STOMP_TProperty));
        Layer->AddTransition(ANY, ATK_STOMP, values, 0.0f, 0.05f);
    }

    // ATK_STOMP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_STOMP, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_STEPON_R
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_STEPON_TProperty));
        Layer->AddTransition(ANY, ATK_STEPON_R, values, 0.0f, 0.05f);
    }

    // ATK_STEPON_R -> ATK_STEPON_L
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(KeepAttackBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(ATK_STEPON_R, ATK_STEPON_L, values, 0.6f, 0.05f);
    }

    // ATK_STEPON_R -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_STEPON_R, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ATK_STEPON_L -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_STEPON_L, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_RUSH_ST
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_RUSH_TProperty));
        Layer->AddTransition(ANY, ATK_RUSH_ST, values, 0.0f, 0.05f);
    }

    // ATK_RUSH_ST -> ATK_RUSH_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_RUSH_ST, ATK_RUSH_LP, values, 0.9f, 0.05f);
    }

    // ATK_RUSH_LP -> ATK_RUSH_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(KeepAttackBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(ATK_RUSH_LP, ATK_RUSH_ED, values, 0.0f, 0.05f, 0.3f);
    }

    // ATK_RUSH_ED -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_RUSH_ED, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_JUMP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_JUMP_TProperty));
        Layer->AddTransition(ANY, ATK_JUMP, values, 0.0f, 0.05f);
    }

    // ATK_JUMP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_JUMP, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_BACKJUMP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_BACKJUMP_TProperty));
        Layer->AddTransition(ANY, ATK_BACKJUMP, values, 0.0f, 0.05f);
    }

    // ATK_BACKJUMP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_BACKJUMP, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_ELECTRIC
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_ELECTRIC_TProperty));
        Layer->AddTransition(ANY, ATK_ELECTRIC, values, 0.0f, 0.05f);
    }

    // ATK_ELECTRIC -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_ELECTRIC, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_DOWNSTRIKE_L
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_DOWNSTRIKE_TProperty));
        Layer->AddTransition(ANY, ATK_DOWNSTRIKE_L, values, 0.0f, 0.05f);
    }

    // ATK_DOWNSTRIKE_L -> ATK_DOWNSTRIKE_R
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(KeepAttackBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(ATK_DOWNSTRIKE_L, ATK_DOWNSTRIKE_R, values, 0.5f, 0.05f);
    }

    // ATK_DOWNSTRIKE_L -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_DOWNSTRIKE_L, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ATK_DOWNSTRIKE_R -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_DOWNSTRIKE_R, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_BEAM
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(ATK_BEAM_TProperty));
        Layer->AddTransition(ANY, ATK_BEAM, values, 0.0f, 0.05f);
    }

    // ATK_BEAM -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_BEAM, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANT -> DMG_GROGY_ST
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(GROGY_TProperty));
        Layer->AddTransition(ANY, DMG_GROGY_ST, values, 0.0f, 0.05f, 0.0f);
    }

    // DMG_GROGY_ST -> DMG_GROGY_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(DMG_GROGY_ST, DMG_GROGY_LP, values, 1.0f, 0.0f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // DMG_GROGY_LP -> DMG_GROGY_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(DMG_GROGY_LP, DMG_GROGY_ED, values, 2.0f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // DMG_GROGY_ED -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(DMG_GROGY_ED, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> DMG_DIE
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(DIE_TProperty));
        Layer->AddTransition(ANY, DMG_DIE, values, 0.0f, 0.05f, 0.0f);
    }

    // ANY -> ETC_RAGE
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(RAGE_TProperty));
        Layer->AddTransition(ANY, ETC_RAGE, values, 0.0f, 0.05f);
    }

    // ETC_RAGE -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ETC_RAGE, EXIT, values, 0.95f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // Additive ===============================================================================================================

    // ANY -> ADDITIVE_DMG_ADDITIVE
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::Trigger(AdditiveDamageTProperty));
        AdditiveLayer->AddTransition(ANY, ADDITIVE_DMG_ADDITIVE, values, 0.0f, 0.0f);
    }

    // ADDITIVE_DMG_ADDITIVE -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        AdditiveLayer->AddTransition(ADDITIVE_DMG_ADDITIVE, EXIT, values, 1.0f, 0.0f, 0.0f, AnimatorTransition::Interrupt::None);
    }
}
