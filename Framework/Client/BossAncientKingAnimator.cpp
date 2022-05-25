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

    AdditiveLayer = new AnimatorLayer(TEXT("AdditiveLayer"), AnimatorLayer::AnimateType::Additive);
    AddLayer(AdditiveLayer);
}

void BossAncientKingAnimator::SetupProperties()
{
    AdditiveDamageTProperty = AdditiveLayer->AddProperty(TEXT("AdditiveDamageT"), AnimatorProperty::Type::TRIGGER);
}

void BossAncientKingAnimator::SetupNodes()
{
    BH_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_IDLE")), LOOP);
    Layer->AddNode(BH_IDLE);

    ADDITIVE_EMPTY = AnimatorEmptyNode::Create(TEXT("ADDITIVE_EMPTY"), 0.0f, LOOP);
    AdditiveLayer->AddNode(ADDITIVE_EMPTY);

    ADDITIVE_DMG_ADDITIVE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_ADDITIVE")), NOLOOP);
    AdditiveLayer->AddNode(ADDITIVE_DMG_ADDITIVE);
}

void BossAncientKingAnimator::SetupTransitions()
{
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
