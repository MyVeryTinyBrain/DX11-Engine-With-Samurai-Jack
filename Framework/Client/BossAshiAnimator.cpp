#include "stdafx.h"
#include "BossAshiAnimator.h"
#include "Config.h"

void BossAshiAnimator::Awake()
{
    Animator::Awake();
    SetupMesh();
    SetupLayer();
    SetupProperties();
    SetupNodes();
    SetupTransitions();
}

SubResourceRef<AnimationClip> BossAshiAnimator::GetClip(const tstring& name)
{
    return Mesh->GetAnimationClipByName(TEXT("ROOT|") + name);
}

void BossAshiAnimator::SetupMesh()
{
    Mesh = system->resource->Find(MESH_ASHI);
}

void BossAshiAnimator::SetupLayer()
{
    Layer = new AnimatorLayer;
    AddLayer(Layer);
    Layer->SetRootNodeByName(TEXT("CharacterRoot"));

    // Additive ===============================================================================================================

    AdditiveLayer = new AnimatorLayer(TEXT("AdditiveLayer"), AnimatorLayer::AnimateType::Additive);
    AddLayer(AdditiveLayer);
}

void BossAshiAnimator::SetupProperties()
{
    MoveFProperty = Layer->AddProperty(TEXT("MoveFProperty"), AnimatorProperty::Type::FLOAT);
    WalkDirectionFProperty = Layer->AddProperty(TEXT("WalkDirectionFProperty"), AnimatorProperty::Type::FLOAT);
    TurnBProperty = Layer->AddProperty(TEXT("TurnBProperty"), AnimatorProperty::Type::BOOL);
    BackjumpTProperty = Layer->AddProperty(TEXT("BackjumpTProperty"), AnimatorProperty::Type::TRIGGER);
    DieTProperty = Layer->AddProperty(TEXT("DieTProperty"), AnimatorProperty::Type::TRIGGER);

    ATK_DOUBLEHAND_SLASH_TProperty = Layer->AddProperty(TEXT("ATK_DOUBLEHAND_SLASH_TProperty"), AnimatorProperty::Type::TRIGGER);
    ATK_H_SLASH_TProperty = Layer->AddProperty(TEXT("ATK_H_SLASH_TProperty"), AnimatorProperty::Type::TRIGGER);
    ATK_SHOLDER_SLASH_TProperty = Layer->AddProperty(TEXT("ATK_SHOLDER_SLASH_TProperty"), AnimatorProperty::Type::TRIGGER);
    ATK_SLASHUP_TProperty = Layer->AddProperty(TEXT("ATK_SLASHUP_TProperty"), AnimatorProperty::Type::TRIGGER);

    ATK_DROPKICK_TProperty = Layer->AddProperty(TEXT("ATK_DROPKICK_TProperty"), AnimatorProperty::Type::TRIGGER);
    ATK_LEGSTEP_TProperty = Layer->AddProperty(TEXT("ATK_LEGSTEP_TProperty"), AnimatorProperty::Type::TRIGGER);
    ATK_SPINKICK_TProperty = Layer->AddProperty(TEXT("ATK_SPINKICK_TProperty"), AnimatorProperty::Type::TRIGGER);

    ATK_LASER_TProperty = Layer->AddProperty(TEXT("ATK_LASER_TProperty"), AnimatorProperty::Type::TRIGGER);

    ATK_RAGE_TProperty = Layer->AddProperty(TEXT("ATK_RAGE_TProperty"), AnimatorProperty::Type::TRIGGER);
    ATK_RUSH_Start_TProperty = Layer->AddProperty(TEXT("ATK_RUSH_Start_TProperty"), AnimatorProperty::Type::TRIGGER);
    ATK_RUSH_End_TProperty = Layer->AddProperty(TEXT("ATK_RUSH_End_TProperty"), AnimatorProperty::Type::TRIGGER);

    ATK_ANGRY_TProperty = Layer->AddProperty(TEXT("ATK_ANGRY_TProperty"), AnimatorProperty::Type::TRIGGER);

    // Additive ===============================================================================================================

    AdditiveDamageTProperty = AdditiveLayer->AddProperty(TEXT("AdditiveDamageTProperty"), AnimatorProperty::Type::TRIGGER);
}

void BossAshiAnimator::SetupNodes()
{
    BH_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_IDLE")), LOOP);
    Layer->AddNode(BH_IDLE);

    {
        AnimatorNode* BH_WALK_F_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_F_LP")), LOOP);
        AnimatorNode* BH_WALK_R_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_R_LP")), LOOP);
        AnimatorNode* BH_WALK_B_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_B_LP")), LOOP);
        AnimatorNode* BH_WALK_L_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_L_LP")), LOOP);
        AnimatorNode* BH_WALK_F_LP_RP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_F_LP")), LOOP);
        vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_F_LP, 0.0f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_R_LP, 0.25f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_B_LP, 0.5f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_L_LP, 0.75f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_F_LP_RP, 1.0f));
        BH_WALK_LP = AnimatorBlendNode::Create(TEXT("BH_WALK_LP"), blendNodeElements, WalkDirectionFProperty, LOOP);
        {
            AnimationEventDesc s0, s1;
            s0.NormalizedTime = 26 / 50.0f;
            s1.NormalizedTime = 49 / 50.0f;
            s0.ContextByte = ByteContext::PLAY_SOUND;
            s1.ContextByte = ByteContext::PLAY_SOUND;
            s0.ContextTStr = SOUND_FOOTSTEP_LIGHT_01;
            s1.ContextTStr = SOUND_FOOTSTEP_LIGHT_02;
            s0.ContextFloat = 0.45f;
            s1.ContextFloat = 0.3f;
            BH_WALK_LP->AddEvent(s0);
            BH_WALK_LP->AddEvent(s1);
        }
        Layer->AddNode(BH_WALK_LP);
    }

    BH_RUN_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_RUN_LP")), LOOP);
    {
        AnimationEventDesc s0, s1;
        s0.NormalizedTime = 2 / 14.0f;
        s1.NormalizedTime = 9 / 14.0f;
        s0.ContextByte = ByteContext::PLAY_SOUND;
        s1.ContextByte = ByteContext::PLAY_SOUND;
        s0.ContextTStr = SOUND_FOOTSTEP_HEAVY_01;
        s1.ContextTStr = SOUND_FOOTSTEP_HEAVY_02;
        s0.ContextFloat = 0.5f;
        s1.ContextFloat = 0.5f;
        BH_RUN_LP->AddEvent(s0);
        BH_RUN_LP->AddEvent(s1);
    }
    Layer->AddNode(BH_RUN_LP);

    BH_TURN = AnimatorSingleNode::Create(GetClip(TEXT("BH_TURN")), LOOP);
    Layer->AddNode(BH_TURN);

    BH_BACKJUMP = AnimatorSingleNode::Create(GetClip(TEXT("BH_BACKJUMP")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 30.0f;
        m1.NormalizedTime = 5 / 30.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        BH_BACKJUMP->AddEvent(m0);
        BH_BACKJUMP->AddEvent(m1);

        AnimationEventDesc f;
        f.NormalizedTime = 20 / 30.0f;
        f.ContextUInt = UIntContext::ATK_FAR;
        BH_BACKJUMP->AddEvent(f);

        {
            AnimationEventDesc swingSound;
            swingSound.NormalizedTime = 2 / 30.0f;
            swingSound.ContextByte = ByteContext::PLAY_SOUND;
            swingSound.ContextTStr = SOUND_BIG_SWING_02;
            swingSound.ContextFloat = 0.5f;
            BH_BACKJUMP->AddEvent(swingSound);
        }
    }
    Layer->AddNode(BH_BACKJUMP);

    ATK_DOUBLEHAND_SLASH = AnimatorSingleNode::Create(GetClip(TEXT("ATK_DOUBLEHAND_SLASH")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 80.0f;
        m1.NormalizedTime = 25 / 80.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_DOUBLEHAND_SLASH->AddEvent(m0);
        ATK_DOUBLEHAND_SLASH->AddEvent(m1);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 22 / 80.0f;
        a1.NormalizedTime = 25 / 80.0f;
        a0.ContextInt = IntContext::SWORD_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::SWORD_END;
        ATK_DOUBLEHAND_SLASH->AddEvent(a0);
        ATK_DOUBLEHAND_SLASH->AddEvent(a1);

        AnimationEventDesc c;
        c.NormalizedTime = 45 / 80.0f;
        c.ContextUInt = UIntContext::ATK_COMBO;
        ATK_DOUBLEHAND_SLASH->AddEvent(c);

        AnimationEventDesc st0, st1;
        st0.NormalizedTime = 19 / 80.0f;
        st1.NormalizedTime = 26 / 80.0f;
        st0.ContextInt = IntContext::SWORDTRAIL_START;
        st1.ContextInt = IntContext::SWORDTRAIL_END;
        ATK_DOUBLEHAND_SLASH->AddEvent(st0);
        ATK_DOUBLEHAND_SLASH->AddEvent(st1);

        AnimationEventDesc s;
        s.NormalizedTime = 19 / 80.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_SWORD_SWING_01;
        s.ContextFloat = 1.0f;
        ATK_DOUBLEHAND_SLASH->AddEvent(s);

        AnimationEventDesc dust;
        dust.NormalizedTime = 26 / 80.0f;
        dust.ContextUInt = UIntContext::SWORD_DUST;
        dust.ContextFloat = 30.0f;
        ATK_DOUBLEHAND_SLASH->AddEvent(dust);

        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 5 / 80.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK05;
            voice.ContextFloat = 1.0f;
            ATK_DOUBLEHAND_SLASH->AddEvent(voice);
        }
    }
    ATK_DOUBLEHAND_SLASH->speed = 1.1f;
    Layer->AddNode(ATK_DOUBLEHAND_SLASH);

    ATK_H_SLASH = AnimatorSingleNode::Create(GetClip(TEXT("ATK_H_SLASH")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 115.0f;
        m1.NormalizedTime = 53 / 115.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_H_SLASH->AddEvent(m0);
        ATK_H_SLASH->AddEvent(m1);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 49 / 115.0f;
        a1.NormalizedTime = 55 / 115.0f;
        a0.ContextInt = IntContext::SWORD_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::SWORD_END;
        ATK_H_SLASH->AddEvent(a0);
        ATK_H_SLASH->AddEvent(a1);

        AnimationEventDesc c;
        c.NormalizedTime = 62 / 115.0f;
        c.ContextUInt = UIntContext::ATK_COMBO;
        ATK_H_SLASH->AddEvent(c);

        AnimationEventDesc st0, st1;
        st0.NormalizedTime = 45 / 115.0f;
        st1.NormalizedTime = 59 / 115.0f;
        st0.ContextInt = IntContext::SWORDTRAIL_START;
        st1.ContextInt = IntContext::SWORDTRAIL_END;
        ATK_H_SLASH->AddEvent(st0);
        ATK_H_SLASH->AddEvent(st1);

        AnimationEventDesc s;
        s.NormalizedTime = 48 / 115.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_SWORD_SWING_02;
        s.ContextFloat = 1.0f;
        ATK_H_SLASH->AddEvent(s);

        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 40 / 115.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK02;
            voice.ContextFloat = 1.0f;
            ATK_H_SLASH->AddEvent(voice);
        }
    }
    ATK_H_SLASH->speed = 1.15f;
    Layer->AddNode(ATK_H_SLASH);

    ATK_SHOLDER_SLASH = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SHOLDER_SLASH")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 62.0f;
        m1.NormalizedTime = 29 / 62.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_SHOLDER_SLASH->AddEvent(m0);
        ATK_SHOLDER_SLASH->AddEvent(m1);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 25 / 62.0f;
        a1.NormalizedTime = 28 / 62.0f;
        a0.ContextInt = IntContext::SWORD_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::SWORD_END;
        ATK_SHOLDER_SLASH->AddEvent(a0);
        ATK_SHOLDER_SLASH->AddEvent(a1);

        AnimationEventDesc c;
        c.NormalizedTime = 35 / 62.0f;
        c.ContextUInt = UIntContext::ATK_COMBO;
        ATK_SHOLDER_SLASH->AddEvent(c);

        AnimationEventDesc st0, st1;
        st0.NormalizedTime = 22 / 62.0f;
        st1.NormalizedTime = 30 / 62.0f;
        st0.ContextInt = IntContext::SWORDTRAIL_START;
        st1.ContextInt = IntContext::SWORDTRAIL_END;
        ATK_SHOLDER_SLASH->AddEvent(st0);
        ATK_SHOLDER_SLASH->AddEvent(st1);

        AnimationEventDesc s;
        s.NormalizedTime = 24 / 62.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_SWORD_SWING_03;
        s.ContextFloat = 1.0f;
        ATK_SHOLDER_SLASH->AddEvent(s);

        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 15 / 62.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK03;
            voice.ContextFloat = 1.0f;
            ATK_SHOLDER_SLASH->AddEvent(voice);
        }
    }
    ATK_SHOLDER_SLASH->speed = 1.5f;
    Layer->AddNode(ATK_SHOLDER_SLASH);

    ATK_SLASHUP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SLASHUP")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 72.0f;
        m1.NormalizedTime = 29 / 72.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_SLASHUP->AddEvent(m0);
        ATK_SLASHUP->AddEvent(m1);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 24 / 72.0f;
        a1.NormalizedTime = 29 / 72.0f;
        a0.ContextInt = IntContext::SWORD_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::SWORD_END;
        ATK_SLASHUP->AddEvent(a0);
        ATK_SLASHUP->AddEvent(a1);

        AnimationEventDesc c;
        c.NormalizedTime = 50 / 72.0f;
        c.ContextUInt = UIntContext::ATK_COMBO;
        ATK_SLASHUP->AddEvent(c);

        AnimationEventDesc st0, st1;
        st0.NormalizedTime = 24 / 72.0f;
        st1.NormalizedTime = 34 / 72.0f;
        st0.ContextInt = IntContext::SWORDTRAIL_START;
        st1.ContextInt = IntContext::SWORDTRAIL_END;
        ATK_SLASHUP->AddEvent(st0);
        ATK_SLASHUP->AddEvent(st1);

        AnimationEventDesc s;
        s.NormalizedTime = 23 / 72.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_SWORD_SWING_04;
        s.ContextFloat = 1.0f;
        ATK_SLASHUP->AddEvent(s);

        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 1 / 72.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK01;
            voice.ContextFloat = 1.0f;
            ATK_SLASHUP->AddEvent(voice);
        }
        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 25 / 72.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK06;
            voice.ContextFloat = 1.0f;
            ATK_SLASHUP->AddEvent(voice);
        }
    }
    Layer->AddNode(ATK_SLASHUP);

    ATK_DROPKICK = AnimatorSingleNode::Create(GetClip(TEXT("ATK_DROPKICK")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 100.0f;
        m1.NormalizedTime = 41 / 100.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_DROPKICK->AddEvent(m0);
        ATK_DROPKICK->AddEvent(m1);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 43 / 100.0f;
        a1.NormalizedTime = 46 / 100.0f;
        a0.ContextInt = IntContext::RIGHT_FOOT_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::RIGHT_FOOT_END;
        ATK_DROPKICK->AddEvent(a0);
        ATK_DROPKICK->AddEvent(a1);

        AnimationEventDesc c;
        c.NormalizedTime = 55 / 100.0f;
        c.ContextUInt = UIntContext::ATK_COMBO;
        ATK_DROPKICK->AddEvent(c);

        AnimationEventDesc s;
        s.NormalizedTime = 41 / 100.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_MELEE_SWING_02;
        s.ContextFloat = 1.0f;
        ATK_DROPKICK->AddEvent(s);

        AnimationEventDesc dust;
        dust.NormalizedTime = 45 / 100.0f;
        dust.ContextUInt = UIntContext::RIGHT_FOOT_DUST;
        dust.ContextFloat = 30.0f;
        ATK_DROPKICK->AddEvent(dust);

        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 20 / 100.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK01;
            voice.ContextFloat = 1.0f;
            ATK_DROPKICK->AddEvent(voice);
        }
        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 43 / 100.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK04;
            voice.ContextFloat = 1.0f;
            ATK_DROPKICK->AddEvent(voice);
        }
    }
    Layer->AddNode(ATK_DROPKICK);

    ATK_LEGSTEP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_LEGSTEP")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 100.0f;
        m1.NormalizedTime = 36 / 100.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_LEGSTEP->AddEvent(m0);
        ATK_LEGSTEP->AddEvent(m1);

        {
            AnimationEventDesc a0, a1;
            a0.NormalizedTime = 24 / 100.0f;
            a1.NormalizedTime = 26 / 100.0f;
            a0.ContextInt = IntContext::RIGHT_FOOT_START;
            a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
            a1.ContextInt = IntContext::RIGHT_FOOT_END;
            ATK_LEGSTEP->AddEvent(a0);
            ATK_LEGSTEP->AddEvent(a1);
        }
        {
            AnimationEventDesc a0, a1;
            a0.NormalizedTime = 33 / 100.0f;
            a1.NormalizedTime = 37 / 100.0f;
            a0.ContextInt = IntContext::RIGHT_FOOT_START;
            a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
            a1.ContextInt = IntContext::RIGHT_FOOT_END;
            ATK_LEGSTEP->AddEvent(a0);
            ATK_LEGSTEP->AddEvent(a1);
        }

        AnimationEventDesc c;
        c.NormalizedTime = 50 / 100.0f;
        c.ContextUInt = UIntContext::ATK_COMBO;
        ATK_LEGSTEP->AddEvent(c);
    }
    ATK_LEGSTEP->speed = 1.5f;
    Layer->AddNode(ATK_LEGSTEP);

    ATK_SPINKICK = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SPINKICK")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 100.0f;
        m1.NormalizedTime = 34 / 100.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_SPINKICK->AddEvent(m0);
        ATK_SPINKICK->AddEvent(m1);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 27 / 100.0f;
        a1.NormalizedTime = 36 / 100.0f;
        a0.ContextInt = IntContext::RIGHT_FOOT_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::RIGHT_FOOT_END;
        ATK_SPINKICK->AddEvent(a0);
        ATK_SPINKICK->AddEvent(a1);

        AnimationEventDesc c;
        c.NormalizedTime = 50 / 100.0f;
        c.ContextUInt = UIntContext::ATK_COMBO;
        ATK_SPINKICK->AddEvent(c);

        AnimationEventDesc s;
        s.NormalizedTime = 27 / 100.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_MELEE_SWING_02;
        s.ContextFloat = 1.0f;
        ATK_SPINKICK->AddEvent(s);

        //for (uint i = 0; i < 10; ++i)
        //{
        //    AnimationEventDesc dust;
        //    dust.NormalizedTime = (26 + i) / 100.0f;
        //    dust.ContextUInt = UIntContext::RIGHT_FOOT_DUST;
        //    dust.ContextFloat = 5.0f;
        //    ATK_SPINKICK->AddEvent(dust);
        //}

        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 5 / 100.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK01;
            voice.ContextFloat = 1.0f;
            ATK_SPINKICK->AddEvent(voice);
        }
        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 26 / 100.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK04;
            voice.ContextFloat = 1.0f;
            ATK_SPINKICK->AddEvent(voice);
        }
    }
    Layer->AddNode(ATK_SPINKICK);

    ATK_LASER_ST = AnimatorSingleNode::Create(GetClip(TEXT("ATK_LASER_ST")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        ATK_LASER_ST->AddEvent(m0);

        AnimationEventDesc s;
        s.NormalizedTime = 30 / 50.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_ELECTRIC_01;
        s.ContextFloat = 1.0f;
        ATK_LASER_ST->AddEvent(s);

        for (uint i = 0; i < 20; ++i)
        {
            AnimationEventDesc beam;
            beam.NormalizedTime = (30 + i) / 50.0f;
            beam.ContextUInt = UIntContext::ATK_SHOOT_BEAM;
            ATK_LASER_ST->AddEvent(beam);
        }

        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 1 / 50.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK02;
            voice.ContextFloat = 1.0f;
            ATK_LASER_ST->AddEvent(voice);
        }
        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 30 / 50.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_SCREAM01;
            voice.ContextFloat = 1.0f;
            ATK_LASER_ST->AddEvent(voice);
        }
    }
    Layer->AddNode(ATK_LASER_ST);

    ATK_LASER_ED = AnimatorSingleNode::Create(GetClip(TEXT("ATK_LASER_ED")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0.0f;
        m1.NormalizedTime = 25 / 93.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_LASER_ED->AddEvent(m0);
        ATK_LASER_ED->AddEvent(m1);
    }
    Layer->AddNode(ATK_LASER_ED);

    ATK_RAGE = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RAGE")), NOLOOP);
    {
        AnimationEventDesc i0, i1, i2;
        i0.NormalizedTime = 21 / 129.0f;
        i1.NormalizedTime = 38 / 129.0f;
        i2.NormalizedTime = 55 / 129.0f;
        i0.ContextUInt = i1.ContextUInt = i2.ContextUInt = UIntContext::ATK_GROUND_IMPACT;
        ATK_RAGE->AddEvent(i0);
        ATK_RAGE->AddEvent(i1);
        ATK_RAGE->AddEvent(i2);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 21 / 129.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_IMPACT_01;
            s.ContextFloat = 1.0f;
            ATK_RAGE->AddEvent(s);

            AnimationEventDesc e;
            e.NormalizedTime = 21 / 129.0f;
            e.ContextByte = ByteContext::CAM_SHAKE;
            e.ContextFloat = 1.0f;
            ATK_RAGE->AddEvent(e);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 38 / 129.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_IMPACT_01;
            s.ContextFloat = 1.0f;
            ATK_RAGE->AddEvent(s);

            AnimationEventDesc e;
            e.NormalizedTime = 38 / 129.0f;
            e.ContextByte = ByteContext::CAM_SHAKE;
            e.ContextFloat = 1.0f;
            ATK_RAGE->AddEvent(e);
        }
        {
            AnimationEventDesc s;
            s.NormalizedTime = 55 / 129.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_IMPACT_01;
            s.ContextFloat = 1.0f;
            ATK_RAGE->AddEvent(s);

            AnimationEventDesc e;
            e.NormalizedTime = 55 / 129.0f;
            e.ContextByte = ByteContext::CAM_SHAKE;
            e.ContextFloat = 1.0f;
            ATK_RAGE->AddEvent(e);
        }
        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 18 / 129.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_LAUGH01;
            voice.ContextFloat = 1.0f;
            ATK_RAGE->AddEvent(voice);
        }
    }
    ATK_RAGE->speed = 1.25f;
    Layer->AddNode(ATK_RAGE);

    ATK_RUSH_ST_1 = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_ST_1")), NOLOOP);
    ATK_RUSH_ST_1->speed = 1.4f;
    Layer->AddNode(ATK_RUSH_ST_1);

    ATK_RUSH_ST_2 = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_ST_2")), NOLOOP);
    {
        AnimationEventDesc m;
        m.NormalizedTime = 0.0f;
        m.ContextUInt = UIntContext::START_MANUAL_LOOK;
        ATK_RUSH_ST_2->AddEvent(m);

        {
            AnimationEventDesc s;
            s.NormalizedTime = 40 / 41.0f;
            s.ContextByte = ByteContext::PLAY_SOUND;
            s.ContextTStr = SOUND_FOOTSTEP_HEAVY_01;
            s.ContextFloat = 0.7f;
            ATK_RUSH_ST_2->AddEvent(s);
        }
        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 5 / 41.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_ATTACK01;
            voice.ContextFloat = 1.0f;
            ATK_RUSH_ST_2->AddEvent(voice);
        }
        {
            AnimationEventDesc voice;
            voice.NormalizedTime = 38 / 41.0f;
            voice.ContextByte = ByteContext::PLAY_SOUND;
            voice.ContextTStr = SOUND_ASHI_LAUGH02;
            voice.ContextFloat = 1.0f;
            ATK_RUSH_ST_2->AddEvent(voice);
        }
    }
    Layer->AddNode(ATK_RUSH_ST_2);

    ATK_RUSH_LP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_LP")), LOOP);
    {
        AnimationEventDesc m;
        m.NormalizedTime = 0.0f;
        m.ContextUInt = UIntContext::START_MANUAL_LOOK;
        ATK_RUSH_LP->AddEvent(m);

        {
            AnimationEventDesc s0, s1;
            s0.NormalizedTime = 4 / 10.0f;
            s1.NormalizedTime = 9 / 10.0f;
            s0.ContextByte = ByteContext::PLAY_SOUND;
            s1.ContextByte = ByteContext::PLAY_SOUND;
            s0.ContextTStr = SOUND_FOOTSTEP_HEAVY_01;
            s1.ContextTStr = SOUND_FOOTSTEP_HEAVY_02;
            s0.ContextFloat = 0.7f;
            s1.ContextFloat = 0.7f;
            ATK_RUSH_LP->AddEvent(s0);
            ATK_RUSH_LP->AddEvent(s1);
        }
    }
    Layer->AddNode(ATK_RUSH_LP);

    ATK_RUSH_ED = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_ED")), NOLOOP);
    {
        AnimationEventDesc m0, m1;
        m0.NormalizedTime = 0 / 92.0f;
        m1.NormalizedTime = 30 / 92.0f;
        m0.ContextUInt = UIntContext::START_MANUAL_LOOK;
        m1.ContextUInt = UIntContext::END_MANUAL_LOOK;
        ATK_RUSH_ED->AddEvent(m0);
        ATK_RUSH_ED->AddEvent(m1);

        AnimationEventDesc a0, a1;
        a0.NormalizedTime = 23 / 92.0f;
        a1.NormalizedTime = 27 / 92.0f;
        a0.ContextInt = IntContext::SWORD_START;
        a0.ContextUInt = UIntContext::ATK_BLOW | UIntContext::ATK_GADABLE;
        a1.ContextInt = IntContext::SWORD_END;
        ATK_RUSH_ED->AddEvent(a0);
        ATK_RUSH_ED->AddEvent(a1);

        AnimationEventDesc st0, st1;
        st0.NormalizedTime = 22 / 92.0f;
        st1.NormalizedTime = 30 / 92.0f;
        st0.ContextInt = IntContext::SWORDTRAIL_START;
        st1.ContextInt = IntContext::SWORDTRAIL_END;
        ATK_RUSH_ED->AddEvent(st0);
        ATK_RUSH_ED->AddEvent(st1);

        AnimationEventDesc s;
        s.NormalizedTime = 22 / 92.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_SWORD_SWING_05;
        s.ContextFloat = 1.0f;
        ATK_RUSH_ED->AddEvent(s);

        {
            AnimationEventDesc s0, s1, s2;
            s0.NormalizedTime = 4 / 92.0f;
            s1.NormalizedTime = 10 / 92.0f;
            s2.NormalizedTime = 15 / 92.0f;
            s0.ContextByte = ByteContext::PLAY_SOUND;
            s1.ContextByte = ByteContext::PLAY_SOUND;
            s2.ContextByte = ByteContext::PLAY_SOUND;
            s0.ContextTStr = SOUND_FOOTSTEP_HEAVY_01;
            s1.ContextTStr = SOUND_FOOTSTEP_HEAVY_02;
            s2.ContextTStr = SOUND_FOOTSTEP_HEAVY_01;
            s0.ContextFloat = 0.7f;
            s1.ContextFloat = 0.7f;
            s2.ContextFloat = 0.7f;
            ATK_RUSH_ED->AddEvent(s0);
            ATK_RUSH_ED->AddEvent(s1);
            ATK_RUSH_ED->AddEvent(s2);
        }
    }
    ATK_RUSH_ED->speed = 1.5f;
    Layer->AddNode(ATK_RUSH_ED);

    DMG_DIE_ST = AnimatorSingleNode::Create(GetClip(TEXT("DMG_DIE_ST")), NOLOOP);
    {
        AnimationEventDesc s;
        s.NormalizedTime = 0.0f;
        s.ContextByte = ByteContext::PLAY_SOUND;
        s.ContextTStr = SOUND_ASHI_DIE;
        s.ContextFloat = 1.0f;
        DMG_DIE_ST->AddEvent(s);
    }
    Layer->AddNode(DMG_DIE_ST);

    DMG_DIE_ED = AnimatorSingleNode::Create(GetClip(TEXT("DMG_DIE_ED")), NOLOOP);
    {
        AnimationEventDesc dust;
        dust.NormalizedTime = 2 / 24.0f;
        dust.ContextUInt = UIntContext::BODY_DUST;
        dust.ContextFloat = 40.0f;
        DMG_DIE_ED->AddEvent(dust);
    }
    Layer->AddNode(DMG_DIE_ED);

    ATK_ANGRY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_ANGRY")), NOLOOP);
    Layer->AddNode(ATK_ANGRY);

    // Additive ===============================================================================================================

    ADDITIVE_EMPTY = AnimatorEmptyNode::Create(TEXT("ADDITIVE_EMPTY"), 0.0f, LOOP);
    AdditiveLayer->AddNode(ADDITIVE_EMPTY);

    ADDITIVE_DMG_ADDITIVE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_ADDITIVE")), NOLOOP);
    AdditiveLayer->AddNode(ADDITIVE_DMG_ADDITIVE);
}

void BossAshiAnimator::SetupTransitions()
{
    // BH_IDLE -> BH_WALK_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 1.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_WALK_LP, values, 0.0f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WALK_LP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 0.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_WALK_LP, EXIT, values, 0.0f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_IDLE -> BH_RUN_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 2.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_RUN_LP, values, 0.0f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_RUN_LP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 0.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_RUN_LP, EXIT, values, 0.0f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WALK_LP -> BH_RUN_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 2.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_WALK_LP, BH_RUN_LP, values, 0.0f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_RUN_LP -> BH_WALK_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 1.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_RUN_LP, BH_WALK_LP, values, 0.0f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_IDLE -> BH_TURN
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_TURN, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_TURN -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_TURN, EXIT, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> BH_BACKJUMP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(BackjumpTProperty));
        Layer->AddTransition(ANY, BH_BACKJUMP, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::Current, true);
    }

    // BH_BACKJUMP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(BH_BACKJUMP, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> DMG_DIE_ST
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(DieTProperty));
        Layer->AddTransition(ANY, DMG_DIE_ST, values, 0.0f, 0.1f, 0.0f);
    }

    // DMG_DIE_ST -> DMG_DIE_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(DMG_DIE_ST, DMG_DIE_ED, values, 1.0f, 0.0f, 0.0f);
    }

    // ANY -> ATK_DOUBLEHAND_SLASH
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_DOUBLEHAND_SLASH_TProperty));
        Layer->AddTransition(ANY, ATK_DOUBLEHAND_SLASH, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_DOUBLEHAND_SLASH -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_DOUBLEHAND_SLASH, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_H_SLASH
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_H_SLASH_TProperty));
        Layer->AddTransition(ANY, ATK_H_SLASH, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_H_SLASH -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_H_SLASH, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_SHOLDER_SLASH
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_SHOLDER_SLASH_TProperty));
        Layer->AddTransition(ANY, ATK_SHOLDER_SLASH, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_SHOLDER_SLASH -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_SHOLDER_SLASH, EXIT, values, 0.8f, 0.2f, 0.08f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_SLASHUP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_SLASHUP_TProperty));
        Layer->AddTransition(ANY, ATK_SLASHUP, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_SLASHUP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_SLASHUP, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_DROPKICK
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_DROPKICK_TProperty));
        Layer->AddTransition(ANY, ATK_DROPKICK, values, 0.0f, 0.2f, 0.095f);
    }

    // ATK_DROPKICK -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_DROPKICK, EXIT, values, 0.9f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_LEGSTEP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_LEGSTEP_TProperty));
        Layer->AddTransition(ANY, ATK_LEGSTEP, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_LEGSTEP -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_LEGSTEP, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_SPINKICK
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_SPINKICK_TProperty));
        Layer->AddTransition(ANY, ATK_SPINKICK, values, 0.0f, 0.2f, 0.06f);
    }

    // ATK_SPINKICK -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_SPINKICK, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_LASER_ST
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_LASER_TProperty));
        Layer->AddTransition(ANY, ATK_LASER_ST, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_LASER_ST -> ATK_LASER_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_LASER_ST, ATK_LASER_ED, values, 0.9f, 0.1f, 0.0f);
    }

    // ATK_LASER_ED -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_LASER_ED, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_RAGE
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_RAGE_TProperty));
        Layer->AddTransition(ANY, ATK_RAGE, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_RAGE -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_RAGE, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_RUSH_ST_2
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_RUSH_Start_TProperty));
        Layer->AddTransition(ANY, ATK_RUSH_ST_2, values, 0.0f, 0.2f, 0.0f);
    }

    // ATK_RUSH_ST_2 -> ATK_RUSH_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_RUSH_ST_2, ATK_RUSH_LP, values, 0.95f, 0.1f, 0.0f);
    }

    // ATK_RUSH_LP -> ATK_RUSH_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_RUSH_End_TProperty));
        Layer->AddTransition(ATK_RUSH_LP, ATK_RUSH_ED, values, 0.0f, 0.05f, 0.0f);
    }

    // ATK_RUSH_ED -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_RUSH_ED, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // ANY -> ATK_ANGRY
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue::Trigger(ATK_ANGRY_TProperty));
        Layer->AddTransition(ANY, ATK_ANGRY, values, 0.0f, 0.1f, 0.1f);
    }

    // ATK_ANGRY -> ATK_RAGE
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(ATK_ANGRY, ATK_RAGE, values, 0.6f, 0.1f, 0.0f);
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
