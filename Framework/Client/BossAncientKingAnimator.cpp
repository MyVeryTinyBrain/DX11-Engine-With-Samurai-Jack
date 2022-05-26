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
    ATK_ELECTRIC_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_ELECTRIC_T"), AnimatorProperty::Type::TRIGGER);
    ATK_DOWNSTRIKE_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_DOWNSTRIKE_T"), AnimatorProperty::Type::TRIGGER);
    ATK_BEAM_TProperty = AdditiveLayer->AddProperty(TEXT("ATK_BEAM_T"), AnimatorProperty::Type::TRIGGER);
    // Additive ===============================================================================================================

    AdditiveDamageTProperty = AdditiveLayer->AddProperty(TEXT("AdditiveDamageT"), AnimatorProperty::Type::TRIGGER);
}

void BossAncientKingAnimator::SetupNodes()
{
    BH_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_IDLE")), LOOP);
    Layer->AddNode(BH_IDLE);

    BH_TURN_ROTATE = AnimatorSingleNode::Create(GetClip(TEXT("BH_TURN_ROTATE")), NOLOOP);
    Layer->AddNode(BH_TURN_ROTATE);

    BH_TURN_WAIT = AnimatorSingleNode::Create(GetClip(TEXT("BH_TURN_WAIT")), NOLOOP);
    Layer->AddNode(BH_TURN_WAIT);

    BH_WLK_ST = AnimatorSingleNode::Create(GetClip(TEXT("BH_WLK_ST")), NOLOOP);
    Layer->AddNode(BH_WLK_ST);

    BH_WLK_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WLK_LP")), LOOP);
    Layer->AddNode(BH_WLK_LP);

    BH_WLK_ED = AnimatorSingleNode::Create(GetClip(TEXT("BH_WLK_ED")), NOLOOP);
    Layer->AddNode(BH_WLK_ED);

    DMG_GROGY_ST = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_ST")), NOLOOP);
    Layer->AddNode(DMG_GROGY_ST);

    DMG_GROGY_LP = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_LP")), LOOP);
    Layer->AddNode(DMG_GROGY_LP);

    DMG_GROGY_ED = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_ED")), NOLOOP);
    Layer->AddNode(DMG_GROGY_ED);

    DMG_GROGY_HIT = AnimatorSingleNode::Create(GetClip(TEXT("DMG_GROGY_HIT")), NOLOOP);
    Layer->AddNode(DMG_GROGY_HIT);

    DMG_DIE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_DIE")), NOLOOP);
    Layer->AddNode(DMG_DIE);
  
    ATK_TURN = AnimatorSingleNode::Create(GetClip(TEXT("ATK_TURN")), NOLOOP);
    Layer->AddNode(ATK_TURN);

    ATK_SWING_L = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SWING_L")), NOLOOP);
    Layer->AddNode(ATK_SWING_L);

    ATK_SWING_R = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SWING_R")), NOLOOP);
    Layer->AddNode(ATK_SWING_R);

    ATK_SWING_V = AnimatorSingleNode::Create(GetClip(TEXT("ATK_SWING_V")), NOLOOP);
    Layer->AddNode(ATK_SWING_V);

    ATK_STOMP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_STOMP")), NOLOOP);
    Layer->AddNode(ATK_STOMP);

    ATK_STEPON_L = AnimatorSingleNode::Create(GetClip(TEXT("ATK_STEPON_L")), NOLOOP);
    Layer->AddNode(ATK_STEPON_L);

    ATK_STEPON_R = AnimatorSingleNode::Create(GetClip(TEXT("ATK_STEPON_R")), NOLOOP);
    Layer->AddNode(ATK_STEPON_R);

    ATK_RUSH_ST = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_ST")), NOLOOP);
    Layer->AddNode(ATK_RUSH_ST);

    ATK_RUSH_LP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_LP")), LOOP);
    Layer->AddNode(ATK_RUSH_LP);

    ATK_RUSH_ED = AnimatorSingleNode::Create(GetClip(TEXT("ATK_RUSH_ED")), NOLOOP);
    Layer->AddNode(ATK_RUSH_ED);

    ATK_JUMP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_JUMP")), NOLOOP);
    Layer->AddNode(ATK_JUMP);

    ATK_ELECTRIC = AnimatorSingleNode::Create(GetClip(TEXT("ATK_ELECTRIC")), NOLOOP);
    Layer->AddNode(ATK_ELECTRIC);

    ATK_DOWNSTRIKE_L = AnimatorSingleNode::Create(GetClip(TEXT("ATK_DOWNSTRIKE_L")), NOLOOP);
    Layer->AddNode(ATK_DOWNSTRIKE_L);

    ATK_DOWNSTRIKE_R = AnimatorSingleNode::Create(GetClip(TEXT("ATK_DOWNSTRIKE_R")), NOLOOP);
    Layer->AddNode(ATK_DOWNSTRIKE_R);

    ATK_BEAM = AnimatorSingleNode::Create(GetClip(TEXT("ATK_BEAM")), NOLOOP);
    Layer->AddNode(ATK_BEAM);

    ETC_RAGE = AnimatorSingleNode::Create(GetClip(TEXT("ETC_RAGE")), NOLOOP);
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
