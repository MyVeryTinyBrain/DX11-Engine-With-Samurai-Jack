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

    // Additive ===============================================================================================================

    AdditiveDamageTProperty = AdditiveLayer->AddProperty(TEXT("AdditiveDamageTProperty"), AnimatorProperty::Type::TRIGGER);
}

void BossAshiAnimator::SetupNodes()
{
    BH_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_IDLE")), LOOP);
    Layer->AddNode(BH_IDLE);

    {
        AnimatorNode* BH_WALK_F_ST = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_F_ST")), NOLOOP);
        AnimatorNode* BH_WALK_R_ST = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_R_ST")), NOLOOP);
        AnimatorNode* BH_WALK_B_ST = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_B_ST")), NOLOOP);
        AnimatorNode* BH_WALK_L_ST = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_L_ST")), NOLOOP);
        AnimatorNode* BH_WALK_F_ST_RP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_F_ST")), NOLOOP);
        vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_F_ST, 0.0f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_R_ST, 0.25f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_B_ST, 0.5f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_L_ST, 0.75f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_F_ST_RP, 1.0f));
        BH_WALK_ST = AnimatorBlendNode::Create(TEXT("BH_WALK_ST"), blendNodeElements, WalkDirectionFProperty, NOLOOP);
        Layer->AddNode(BH_WALK_ST);
    }

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
        Layer->AddNode(BH_WALK_LP);
    }

    {
        AnimatorNode* BH_WALK_F_ED = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_F_ED")), NOLOOP);
        AnimatorNode* BH_WALK_R_ED = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_R_ED")), NOLOOP);
        AnimatorNode* BH_WALK_B_ED = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_B_ED")), NOLOOP);
        AnimatorNode* BH_WALK_L_ED = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_L_ED")), NOLOOP);
        AnimatorNode* BH_WALK_F_ED_RP = AnimatorSingleNode::Create(GetClip(TEXT("BH_WALK_F_ED")), NOLOOP);
        vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_F_ED, 0.0f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_R_ED, 0.25f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_B_ED, 0.5f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_L_ED, 0.75f));
        blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_WALK_F_ED_RP, 1.0f));
        BH_WALK_ED = AnimatorBlendNode::Create(TEXT("BH_WALK_ED"), blendNodeElements, WalkDirectionFProperty, NOLOOP);
        Layer->AddNode(BH_WALK_ED);
    }

    BH_RUN_ST = AnimatorSingleNode::Create(GetClip(TEXT("BH_RUN_ST")), NOLOOP);
    Layer->AddNode(BH_RUN_ST);

    BH_RUN_LP = AnimatorSingleNode::Create(GetClip(TEXT("BH_RUN_LP")), LOOP);
    Layer->AddNode(BH_RUN_LP);

    BH_RUN_ED = AnimatorSingleNode::Create(GetClip(TEXT("BH_RUN_ED")), NOLOOP);
    Layer->AddNode(BH_RUN_ED);

    DMG_DIE_ST = AnimatorSingleNode::Create(GetClip(TEXT("DMG_DIE_ST")), NOLOOP);
    Layer->AddNode(DMG_DIE_ST);

    DMG_DIE_ED = AnimatorSingleNode::Create(GetClip(TEXT("DMG_DIE_ED")), NOLOOP);
    Layer->AddNode(DMG_DIE_ED);

    // Additive ===============================================================================================================

    ADDITIVE_EMPTY = AnimatorEmptyNode::Create(TEXT("ADDITIVE_EMPTY"), 0.0f, LOOP);
    AdditiveLayer->AddNode(ADDITIVE_EMPTY);

    ADDITIVE_DMG_ADDITIVE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_ADDITIVE")), NOLOOP);
    AdditiveLayer->AddNode(ADDITIVE_DMG_ADDITIVE);
}

void BossAshiAnimator::SetupTransitions()
{
    // BH_IDLE -> BH_WALK_ST
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 1.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_WALK_ST, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WALK_ST -> BH_WALK_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 1.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_WALK_ST, BH_WALK_LP, values, 1.0f, 0.0f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WALK_ST -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 1.0f, AnimatorTransition::Compare::NOT_EQUAL));
        Layer->AddTransition(BH_WALK_ST, EXIT, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WALK_LP -> BH_WALK_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 1.0f, AnimatorTransition::Compare::NOT_EQUAL));
        Layer->AddTransition(BH_WALK_LP, BH_WALK_ED, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_WALK_ED -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(BH_WALK_ED, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_IDLE -> BH_RUN_ST
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 2.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_IDLE, BH_RUN_ST, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_RUN_ST -> BH_RUN_LP
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 2.0f, AnimatorTransition::Compare::EQUAL));
        Layer->AddTransition(BH_RUN_ST, BH_RUN_LP, values, 1.0f, 0.0f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_RUN_ST -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 2.0f, AnimatorTransition::Compare::NOT_EQUAL));
        Layer->AddTransition(BH_RUN_ST, EXIT, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_RUN_LP -> RUN_ED
    {
        vector<AnimatorTransition::PropertyValue> values;
        values.push_back(AnimatorTransition::PropertyValue::PropertyValue(MoveFProperty, 2.0f, AnimatorTransition::Compare::NOT_EQUAL));
        Layer->AddTransition(BH_RUN_LP, BH_RUN_ED, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
    }

    // BH_RUN_ED -> EXIT
    {
        vector<AnimatorTransition::PropertyValue> values;
        Layer->AddTransition(BH_RUN_ED, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
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
