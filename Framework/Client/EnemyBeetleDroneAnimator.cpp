#include "stdafx.h"
#include "EnemyBeetleDroneAnimator.h"
#include "Config.h"

void EnemyBeetleDroneAnimator::Awake()
{
    Animator::Awake();
	SetupMesh();
	SetupLayer();
	SetupProperties();
	SetupNodes();
	SetupTransitions();
	speed = 1.2f;
}

SubResourceRef<AnimationClip> EnemyBeetleDroneAnimator::GetClip(const tstring& name)
{
    return Mesh->GetAnimationClipByName(TEXT("ROOT|") + name);
}

void EnemyBeetleDroneAnimator::SetupMesh()
{
	Mesh = system->resource->Find(TEXT("../Resource/BeetleDrone/BeetleDrone.FBX"));
}

void EnemyBeetleDroneAnimator::SetupLayer()
{
	Layer = new AnimatorLayer;
	AddLayer(Layer);
	Layer->SetRootNodeByName(TEXT("CharacterRoot"));
}

void EnemyBeetleDroneAnimator::SetupProperties()
{
	 MoveBProperty = Layer->AddProperty(TEXT("MoveB"), AnimatorProperty::Type::BOOL);
	 JumpTProperty = Layer->AddProperty(TEXT("JumpT"), AnimatorProperty::Type::TRIGGER);
	 ATKTProperty = Layer->AddProperty(TEXT("ATKT"), AnimatorProperty::Type::TRIGGER);
	 ATK3TypeIProperty = Layer->AddProperty(TEXT("ATK3TypeI"), AnimatorProperty::Type::INT);
	 GuardBProperty = Layer->AddProperty(TEXT("GuardB"), AnimatorProperty::Type::BOOL);
	 DamageTProperty = Layer->AddProperty(TEXT("DamageT"), AnimatorProperty::Type::TRIGGER);
	 DamageDirectionFProperty = Layer->AddProperty(TEXT("DamageDirectionFProperty"), AnimatorProperty::Type::FLOAT);
	 DamageTypeIProperty = Layer->AddProperty(TEXT("DamageTypeI"), AnimatorProperty::Type::INT);
	 HasGroundBProperty = Layer->AddProperty(TEXT("HasGroundB"), AnimatorProperty::Type::BOOL);
}

void EnemyBeetleDroneAnimator::SetupNodes()
{
	BH_STD_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_STD_IDLE")), LOOP);
	Layer->AddNode(BH_STD_IDLE);

	BH_STD_WALK = AnimatorSingleNode::Create(GetClip(TEXT("BH_STD_WALK")), LOOP);
	Layer->AddNode(BH_STD_WALK);

	BH_STD_JUMP = AnimatorSingleNode::Create(GetClip(TEXT("BH_STD_JUMP")), NOLOOP);
	Layer->AddNode(BH_STD_JUMP);

	BH_STD_FALL = AnimatorSingleNode::Create(GetClip(TEXT("BH_STD_FALL")), LOOP);
	Layer->AddNode(BH_STD_FALL);

	BH_STD_LAND = AnimatorSingleNode::Create(GetClip(TEXT("BH_STD_LAND")), NOLOOP);
	Layer->AddNode(BH_STD_LAND);

	STD_ATK1 = AnimatorSingleNode::Create(GetClip(TEXT("STD_ATK1")), NOLOOP);
	Layer->AddNode(STD_ATK1);

	STD_ATK2 = AnimatorSingleNode::Create(GetClip(TEXT("STD_ATK2")), NOLOOP);
	Layer->AddNode(STD_ATK2);

	STD_ATK3A = AnimatorSingleNode::Create(GetClip(TEXT("STD_ATK3A")), NOLOOP);
	Layer->AddNode(STD_ATK3A);

	STD_ATK3B = AnimatorSingleNode::Create(GetClip(TEXT("STD_ATK3B")), NOLOOP);
	Layer->AddNode(STD_ATK3B);

	DMG_STD_AIR = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_AIR")), NOLOOP);
	Layer->AddNode(DMG_STD_AIR);

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* DMG_STD_F = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_F")), NOLOOP);
		AnimatorSingleNode* DMG_STD_B = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_B")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_F, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_B, 1.0f));

		DMG_STD_LIGHT = AnimatorBlendNode::Create(TEXT("DMG_STD_LIGHT"), blendNodeElements, DamageDirectionFProperty, NOLOOP);
		Layer->AddNode(DMG_STD_LIGHT);
	}

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* DMG_STD_F_L = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_F_L")), NOLOOP);
		AnimatorSingleNode* DMG_STD_B_L = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_B_L")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_F_L, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_B_L, 1.0f));

		DMG_STD_HEAVY = AnimatorBlendNode::Create(TEXT("DMG_STD_HEAVY"), blendNodeElements, DamageDirectionFProperty, NOLOOP);
		Layer->AddNode(DMG_STD_HEAVY);
	}

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* DMG_STD_BLOW_F = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_BLOW_F")), NOLOOP);
		AnimatorSingleNode* DMG_STD_BLOW_B = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_BLOW_B")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_BLOW_F, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_BLOW_B, 1.0f));

		DMG_STD_BLOW = AnimatorBlendNode::Create(TEXT("DMG_STD_BLOW"), blendNodeElements, DamageDirectionFProperty, NOLOOP);
		Layer->AddNode(DMG_STD_BLOW);
	}

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* DMG_STD_GETUP_F = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_GETUP_F")), NOLOOP);
		AnimatorSingleNode* DMG_STD_GETUP_B = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_GETUP_B")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_GETUP_F, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_STD_GETUP_B, 1.0f));

		DMG_STD_GETUP = AnimatorBlendNode::Create(TEXT("DMG_STD_GETUP"), blendNodeElements, DamageDirectionFProperty, NOLOOP);
		Layer->AddNode(DMG_STD_GETUP);
	}

	DMG_STD_BLOWUP_START = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_BLOWUP_START")), NOLOOP);
	Layer->AddNode(DMG_STD_BLOWUP_START);

	DMG_STD_BLOWUP_FALL = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_BLOWUP_FALL")), LOOP);
	Layer->AddNode(DMG_STD_BLOWUP_FALL);

	DMG_STD_BLOWUP_END = AnimatorSingleNode::Create(GetClip(TEXT("DMG_STD_BLOWUP_END")), NOLOOP);
	Layer->AddNode(DMG_STD_BLOWUP_END);

	GAD_STD_LOOP = AnimatorSingleNode::Create(GetClip(TEXT("GAD_STD_LOOP")), NOLOOP);
	Layer->AddNode(GAD_STD_LOOP);

	GAD_STD_HIT = AnimatorSingleNode::Create(GetClip(TEXT("GAD_STD_HIT")), NOLOOP);
	Layer->AddNode(GAD_STD_HIT);

	GAD_STD_BREAK = AnimatorSingleNode::Create(GetClip(TEXT("GAD_STD_BREAK")), NOLOOP);
	Layer->AddNode(GAD_STD_BREAK);

	ETC_APPEAR = AnimatorSingleNode::Create(GetClip(TEXT("ETC_APPEAR")), NOLOOP);
	Layer->AddNode(ETC_APPEAR);
}

void EnemyBeetleDroneAnimator::SetupTransitions()
{
	// BH_STD_IDLE -> BH_STD_WALK
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_STD_IDLE, BH_STD_WALK, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_STD_WALK -> BH_STD_IDLE
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveBProperty, false, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_STD_WALK, BH_STD_IDLE, values, 0.0f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> STD_ATK1
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(ATKTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(ANY, STD_ATK1, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
		transition->SetCallback(this);
	}

	// STD_ATK1 -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(STD_ATK1, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// STD_ATK1 -> STD_ATK2
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(ATKTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(STD_ATK1, STD_ATK2, values, 0.5f, 0.1f);
	}

	// STD_ATK2 -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(STD_ATK2, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// STD_ATK2 -> STD_ATK3A
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(ATKTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(ATK3TypeIProperty, 0, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(STD_ATK2, STD_ATK3A, values, 0.5f, 0.1f);
	}

	// STD_ATK3A -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(STD_ATK3A, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// STD_ATK2 -> STD_ATK3B
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(ATKTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(ATK3TypeIProperty, 0, AnimatorTransition::Compare::NOT_EQUAL));
		Layer->AddTransition(STD_ATK2, STD_ATK3B, values, 0.5f, 0.1f);
	}

	// STD_ATK3B -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(STD_ATK3B, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> DMG_STD_AIR
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 1, AnimatorTransition::Compare::LESS_EQAUL));
		Layer->AddTransition(ANY, DMG_STD_AIR, values, 0.0f, 0.1f);
	}

	// DMG_STD_AIR -> DMG_STD_BLOWUP_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_STD_AIR, DMG_STD_BLOWUP_FALL, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> DMG_STD_LIGHT
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 0, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_STD_LIGHT, values, 0.0f, 0.1f);
	}

	// DMG_STD_LIGHT -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_STD_LIGHT, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> DMG_STD_HEAVY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 1, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_STD_HEAVY, values, 0.0f, 0.1f);
	}

	// DMG_STD_HEAVY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_STD_HEAVY, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> DMG_STD_BLOW
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 2, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_STD_BLOW, values, 0.0f, 0.1f);
	}

	// DMG_STD_BLOW -> DMG_STD_GETUP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(DMG_STD_BLOW, DMG_STD_GETUP, values, 0.8f, 0.1f);
	}

	// DMG_STD_GETUP -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_STD_GETUP, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> DMG_STD_BLOWUP_START
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 3, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_STD_BLOWUP_START, values, 0.0f, 0.1f);
	}

	// DMG_STD_BLOWUP_START -> DMG_STD_BLOWUP_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_STD_BLOWUP_START, DMG_STD_BLOWUP_FALL, values, 0.9f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// DMG_STD_BLOWUP_FALL -> DMG_STD_BLOWUP_END
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(DMG_STD_BLOWUP_FALL, DMG_STD_BLOWUP_END, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// DMG_STD_BLOWUP_END -> DMG_STD_GETUP
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_STD_BLOWUP_END, DMG_STD_GETUP, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
}

bool EnemyBeetleDroneAnimator::IsPlayingAirAction() const
{
	return false;
}

bool EnemyBeetleDroneAnimator::Transferable(
	Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition, 
	AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const
{
	if (transition->nextNode == STD_ATK1)
	{
		if (layer->IsPlaying(STD_ATK1) || 
			layer->IsPlaying(STD_ATK2) ||
			layer->IsPlaying(STD_ATK3A) ||
			layer->IsPlaying(STD_ATK3B))
			return false;
	}

	return true;
}
