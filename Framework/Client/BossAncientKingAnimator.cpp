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
    AdditiveDamageTProperty = AdditiveLayer->AddProperty(TEXT("AdditiveDamageT"), AnimatorProperty::Type::TRIGGER);
    TurnBProperty = AdditiveLayer->AddProperty(TEXT("TurnB"), AnimatorProperty::Type::BOOL);
    WalkBProperty = AdditiveLayer->AddProperty(TEXT("WalkB"), AnimatorProperty::Type::BOOL);
}

void BossAncientKingAnimator::SetupNodes()
{
    BH_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_IDLE")), LOOP);
    Layer->AddNode(BH_IDLE);

    BH_TURN = AnimatorSingleNode::Create(GetClip(TEXT("BH_TURN")), LOOP);
    Layer->AddNode(BH_TURN);

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
    // BH_IDLE -> BH_TURN
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, true, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_TURN, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_TURN -> BH_IDLE
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(TurnBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_TURN, BH_IDLE, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
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

    // BH_WLK_LP -> BH_WLK_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(WalkBProperty, false, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_WLK_LP, BH_WLK_ED, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
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
