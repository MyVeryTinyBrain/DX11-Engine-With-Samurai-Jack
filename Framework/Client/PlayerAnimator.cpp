#include "stdafx.h"
#include "PlayerAnimator.h"

#define LOOP		true
#define NOLOOP		false
#define EXIT		nullptr
#define ANY			nullptr

void PlayerAnimator::Awake()
{
	Animator::Awake();
	SetupMesh();
	SetupLayer();
	SetupProperties();
	SetupNodes();
	SetupTransitions();
}

SubResourceRef<AnimationClip> PlayerAnimator::GetClip(const tstring& name)
{
	return Mesh->GetAnimationClipByName(TEXT("ROOT|") + name);
}

void PlayerAnimator::SetupMesh()
{
	Mesh = system->resource->Find(TEXT("../Resource/Jack/jack.FBX"));
}

void PlayerAnimator::SetupLayer()
{
	Layer = new AnimatorLayer;
	AddLayer(Layer);
	Layer->SetRootNodeByName(TEXT("CharacterRoot"));
}

void PlayerAnimator::SetupProperties()
{
	MoveStateFProperty = Layer->AddProperty(TEXT("MoveStateF"), AnimatorProperty::Type::FLOAT);
	DashStateFProperty = Layer->AddProperty(TEXT("DashStateF"), AnimatorProperty::Type::BOOL);
	DamageTProperty = Layer->AddProperty(TEXT("DamageT"), AnimatorProperty::Type::TRIGGER);
	DamageTypeIProperty = Layer->AddProperty(TEXT("DamageTypeI"), AnimatorProperty::Type::INT);
	DeadBProperty = Layer->AddProperty(TEXT("DeadB"), AnimatorProperty::Type::BOOL);
	RollTProperty = Layer->AddProperty(TEXT("RollT"), AnimatorProperty::Type::TRIGGER);
	LightAttackTProperty = Layer->AddProperty(TEXT("LightAttackT"), AnimatorProperty::Type::TRIGGER);
	HeavyAttackTProperty = Layer->AddProperty(TEXT("HeavyAttackT"), AnimatorProperty::Type::TRIGGER);
	GuardBProperty = Layer->AddProperty(TEXT("GuardB"), AnimatorProperty::Type::BOOL);
	GuardBreakTProperty = Layer->AddProperty(TEXT("GuardBreakT"), AnimatorProperty::Type::TRIGGER);
	GuardHitTProperty = Layer->AddProperty(TEXT("HuardHitT"), AnimatorProperty::Type::TRIGGER);
	JumpTProperty = Layer->AddProperty(TEXT("JumpT"), AnimatorProperty::Type::TRIGGER);
	AirJumpTProperty = Layer->AddProperty(TEXT("AirJumpT"), AnimatorProperty::Type::TRIGGER);
	HasGroundBProperty = Layer->AddProperty(TEXT("HasGroundB"), AnimatorProperty::Type::BOOL);
}

void PlayerAnimator::SetupNodes()
{
	BH_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_IDLE")), LOOP);
	Layer->AddNode(BH_IDLE);

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* BH_RUN = AnimatorSingleNode::Create(GetClip(TEXT("BH_RUN")), LOOP);
		AnimatorSingleNode* BH_DASH = AnimatorSingleNode::Create(GetClip(TEXT("BH_DASH")), LOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_RUN, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(BH_DASH, 1.0f));
	
		BH_RUN_BH_DASH = AnimatorBlendNode::Create(TEXT("BH_RUN_BH_DASH"), blendNodeElements, DashStateFProperty, LOOP);
		Layer->AddNode(BH_RUN_BH_DASH);
	}

	BH_JUMP = AnimatorSingleNode::Create(GetClip(TEXT("BH_JUMP")), NOLOOP);
	Layer->AddNode(BH_JUMP);

	BH_AIR_JUMP = AnimatorSingleNode::Create(GetClip(TEXT("BH_AIR_JUMP")), NOLOOP);
	Layer->AddNode(BH_AIR_JUMP);

	BH_FALL = AnimatorSingleNode::Create(GetClip(TEXT("BH_FALL")), LOOP);
	Layer->AddNode(BH_FALL);

	BH_LAND = AnimatorSingleNode::Create(GetClip(TEXT("BH_LAND")), NOLOOP);
	Layer->AddNode(BH_LAND);

	BH_ROLL = AnimatorSingleNode::Create(GetClip(TEXT("BH_ROLL")), NOLOOP);
	Layer->AddNode(BH_ROLL);
}

void PlayerAnimator::SetupTransitions()
{
	// BH_IDLE -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_IDLE, BH_RUN_BH_DASH, values, 0.0f, 0.2f);
	}

	// BH_RUN_BH_DASH -> BH_IDLE
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::LESS_EQAUL));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_RUN_BH_DASH, BH_IDLE, values, 0.0f, 0.2f);
	}

	// BH_JUMP -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_JUMP, BH_FALL, values, 0.6f, 1.0f, 0.0f);
	}

	// BH_AIR_JUMP -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_AIR_JUMP, BH_FALL, values, 0.6f, 1.0f, 0.0f);
	}

	// BH_IDLE -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_IDLE, BH_FALL, values, 0.0f, 0.1f, 0.0f);
	}

	// BH_RUN_BH_DASH -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_RUN_BH_DASH, BH_FALL, values, 0.0f, 0.1f, 0.0f);
	}

	// BH_JUMP -> BH_LAND
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_JUMP, BH_LAND, values, 0.0f, 0.05f);
	}

	// BH_AIR_JUMP -> BH_LAND
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_AIR_JUMP, BH_LAND, values, 0.0f, 0.05f);
	}

	// BH_FALL -> BH_LAND
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_FALL, BH_LAND, values, 0.0f, 0.1f);
	}

	// BH_LAND -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(BH_LAND, EXIT, values, 1.0f, 0.2f);
	}

	// BH_LAND -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		Layer->AddTransition(BH_LAND, BH_RUN_BH_DASH, values, 0.0f, 0.2f);
	}

	// ANY -> BH_JUMP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(JumpTProperty));
		Layer->AddTransition(ANY, BH_JUMP, values, 0.0f, 0.1f);
	}

	// ANY -> BH_AIR_JUMP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(AirJumpTProperty));
		Layer->AddTransition(ANY, BH_AIR_JUMP, values, 0.0f, 0.1f);
	}

	// ANY -> BH_ROLL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(RollTProperty));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, BH_ROLL, values, 0.0f, 0.1f);
	}

	// BH_ROLL -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(BH_ROLL, EXIT, values, 0.9f, 0.1f);
	}

	// BH_ROLL -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		Layer->AddTransition(BH_ROLL, BH_RUN_BH_DASH, values, 0.55f, 0.3f);
	}
}
