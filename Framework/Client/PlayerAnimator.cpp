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
	speed = 1.2f;
}

SubResourceRef<AnimationClip> PlayerAnimator::GetClip(const tstring& name)
{
	return Mesh->GetAnimationClipByName(TEXT("ROOT|") + name);
}

void PlayerAnimator::SetupMesh()
{
	Mesh = system->resource->Find(TEXT("../Resource/Jack/Jack.FBX"));
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
	LastDashStateFProperty = Layer->AddProperty(TEXT("LastDashStateF"), AnimatorProperty::Type::FLOAT);
	DamageTProperty = Layer->AddProperty(TEXT("DamageT"), AnimatorProperty::Type::TRIGGER);
	DamageDirectionFProperty = Layer->AddProperty(TEXT("DamageDirectionFProperty"), AnimatorProperty::Type::FLOAT);
	DamageTypeIProperty = Layer->AddProperty(TEXT("DamageTypeI"), AnimatorProperty::Type::INT);
	DeadBProperty = Layer->AddProperty(TEXT("DeadB"), AnimatorProperty::Type::BOOL);
	RollTProperty = Layer->AddProperty(TEXT("RollT"), AnimatorProperty::Type::TRIGGER);
	LightAttackTProperty = Layer->AddProperty(TEXT("LightAttackT"), AnimatorProperty::Type::TRIGGER);
	HeavyAttackTProperty = Layer->AddProperty(TEXT("HeavyAttackT"), AnimatorProperty::Type::TRIGGER);
	LightAttackComboTProperty = Layer->AddProperty(TEXT("LightAttackComboT"), AnimatorProperty::Type::TRIGGER);
	HeavyAttackComboTProperty = Layer->AddProperty(TEXT("HeavyAttackComboT"), AnimatorProperty::Type::TRIGGER);
	LightToHeavtAttackTProperty = Layer->AddProperty(TEXT("LightToHeavtAttackT"), AnimatorProperty::Type::TRIGGER);
	GuardStateBProperty = Layer->AddProperty(TEXT("GuardStateB"), AnimatorProperty::Type::BOOL);
	IsPlayingGuardBProperty = Layer->AddProperty(TEXT("IsPlayingGuardB"), AnimatorProperty::Type::BOOL);
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

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* GAD_START_NODASH = AnimatorSingleNode::Create(GetClip(TEXT("GAD_START_NODASH")), NOLOOP);
		AnimatorSingleNode* GAD_START_DASH = AnimatorSingleNode::Create(GetClip(TEXT("GAD_START_DASH")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(GAD_START_NODASH, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(GAD_START_DASH, 1.0f));

		GAD_START = AnimatorBlendNode::Create(TEXT("GAD_START"), blendNodeElements, LastDashStateFProperty, NOLOOP);
		Layer->AddNode(GAD_START);
	}

	GAD_LOOP = AnimatorSingleNode::Create(GetClip(TEXT("GAD_LOOP")), LOOP);
	Layer->AddNode(GAD_LOOP);

	GAD_HIT = AnimatorSingleNode::Create(GetClip(TEXT("GAD_HIT")), LOOP);
	Layer->AddNode(GAD_HIT);

	GAD_BREAK = AnimatorSingleNode::Create(GetClip(TEXT("GAD_BREAK")), LOOP);
	Layer->AddNode(GAD_BREAK);

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* DMG_FORWARD = AnimatorSingleNode::Create(GetClip(TEXT("DMG_FORWARD")), NOLOOP);
		AnimatorSingleNode* DMG_BACK = AnimatorSingleNode::Create(GetClip(TEXT("DMG_BACK")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_FORWARD, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_BACK, 1.0f));

		DMG_LIGHT = AnimatorBlendNode::Create(TEXT("DMG_LIGHT"), blendNodeElements, DamageDirectionFProperty, NOLOOP);
		Layer->AddNode(DMG_LIGHT);
	}

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* DMG_FORWARD_LARGE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_FORWARD_LARGE")), NOLOOP);
		AnimatorSingleNode* DMG_BACK_LARGE = AnimatorSingleNode::Create(GetClip(TEXT("DMG_BACK_LARGE")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_FORWARD_LARGE, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(DMG_BACK_LARGE, 1.0f));

		DMG_HEAVY = AnimatorBlendNode::Create(TEXT("DMG_HEAVY"), blendNodeElements, DamageDirectionFProperty, NOLOOP);
		Layer->AddNode(DMG_HEAVY);
	}

	DMG_BLOW_START = AnimatorSingleNode::Create(GetClip(TEXT("DMG_BLOW_START")), NOLOOP);
	Layer->AddNode(DMG_BLOW_START);

	DMG_BLOW_LOOP = AnimatorSingleNode::Create(GetClip(TEXT("DMG_BLOW_LOOP")), LOOP);
	Layer->AddNode(DMG_BLOW_LOOP);

	DMG_BLOW_END = AnimatorSingleNode::Create(GetClip(TEXT("DMG_BLOW_END")), NOLOOP);
	Layer->AddNode(DMG_BLOW_END);

	DMG_BLOW_GETUP = AnimatorSingleNode::Create(GetClip(TEXT("DMG_BLOW_GETUP")), NOLOOP);
	Layer->AddNode(DMG_BLOW_GETUP);

	ATK_X = AnimatorSingleNode::Create(GetClip(TEXT("ATK_X")), NOLOOP);
	ATK_X->AddEvent(7 / 34.0f, ATK_KT_START);
	ATK_X->AddEvent(10 / 34.0f, ATK_KT_END);
	Layer->AddNode(ATK_X);

	ATK_XX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XX")), NOLOOP);
	ATK_XX->AddEvent(12 / 45.0f, ATK_KT_START);
	ATK_XX->AddEvent(17 / 45.0f, ATK_KT_END);
	Layer->AddNode(ATK_XX);

	ATK_XXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXX")), NOLOOP);
	ATK_XXX->AddEvent(8 / 70.0f, ATK_FOOT_START);
	ATK_XXX->AddEvent(17 / 70.0f, ATK_FOOT_END);
	Layer->AddNode(ATK_XXX);

	ATK_XXXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXX")), NOLOOP);
	ATK_XXXX->AddEvent(10 / 35.0f, ATK_KT_START);
	ATK_XXXX->AddEvent(15 / 35.0f, ATK_KT_END);
	Layer->AddNode(ATK_XXXX);

	ATK_XXXXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXX")), NOLOOP);
	ATK_XXXXX->AddEvent(18 / 53.0f, ATK_KT_START);
	ATK_XXXXX->AddEvent(23 / 53.0f, ATK_KT_END);
	Layer->AddNode(ATK_XXXXX);

	ATK_AIR_X = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_X")), NOLOOP);
	ATK_AIR_X->AddEvent(4 / 40.0f, ATK_KT_START);
	ATK_AIR_X->AddEvent(8 / 40.0f, ATK_KT_END);
	ATK_AIR_X->AddEvent(12 / 40.0f, ATK_KT_START);
	ATK_AIR_X->AddEvent(18 / 40.0f, ATK_KT_END);
	Layer->AddNode(ATK_AIR_X);

	ATK_AIR_XX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_XX")), NOLOOP);
	ATK_AIR_XX->AddEvent(11 / 40.0f, ATK_KT_START);
	ATK_AIR_XX->AddEvent(20 / 40.0f, ATK_KT_END);
	Layer->AddNode(ATK_AIR_XX);

	ATK_AIR_XXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_XXX")), NOLOOP);
	ATK_AIR_XXX->AddEvent(8 / 34.0f, ATK_FOOT_START);
	ATK_AIR_XXX->AddEvent(20 / 34.0f, ATK_FOOT_END);
	Layer->AddNode(ATK_AIR_XXX);

	ATK_Y = AnimatorSingleNode::Create(GetClip(TEXT("ATK_Y")), NOLOOP);
	ATK_Y->AddEvent(8 / 60.0f, ATK_KT_START);
	ATK_Y->AddEvent(14 / 60.0f, ATK_KT_END);
	Layer->AddNode(ATK_Y);

	ATK_YY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_YY")), NOLOOP);
	ATK_YY->AddEvent(5 / 72.0f, ATK_KT_START);
	ATK_YY->AddEvent(12 / 72.0f, ATK_KT_END);
	ATK_YY->AddEvent(15 / 72.0f, ATK_KT_START);
	ATK_YY->AddEvent(20 / 72.0f, ATK_KT_END);
	Layer->AddNode(ATK_YY);

	ATK_YYY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_YYY")), NOLOOP);
	Layer->AddNode(ATK_YYY);

	ATK_XY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XY")), NOLOOP);
	ATK_XY->AddEvent(5 / 30.0f, ATK_KT_START);
	ATK_XY->AddEvent(14 / 30.0f, ATK_KT_END);
	Layer->AddNode(ATK_XY);

	ATK_XXXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXY")), NOLOOP);
	ATK_XXXXY->AddEvent(11 / 42.0f, ATK_KT_START);
	ATK_XXXXY->AddEvent(16 / 42.0f, ATK_KT_END);
	ATK_XXXXY->AddEvent(22 / 42.0f, ATK_KT_START);
	ATK_XXXXY->AddEvent(27 / 42.0f, ATK_KT_END);
	Layer->AddNode(ATK_XXXXY);

	ATK_XXXXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXXY")), NOLOOP);
	ATK_XXXXXY->AddEvent(11 / 52.0f, ATK_KT_START);
	ATK_XXXXXY->AddEvent(35 / 52.0f, ATK_KT_END);
	Layer->AddNode(ATK_XXXXXY);

	ATK_AIR_Y_START = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_START")), NOLOOP);
	ATK_AIR_Y_START->AddEvent(11 / 13.0f, ATK_KT_START);
	Layer->AddNode(ATK_AIR_Y_START);

	ATK_AIR_Y_LOOP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_LOOP")), LOOP);
	ATK_AIR_Y_LOOP->AddEvent(0.0f, ATK_KT_START);
	Layer->AddNode(ATK_AIR_Y_LOOP);

	ATK_AIR_Y_END = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_END")), NOLOOP);
	ATK_AIR_Y_END->AddEvent(0.0f, ATK_KT_START);
	ATK_AIR_Y_END->AddEvent(5 / 35.0f, ATK_KT_END);
	Layer->AddNode(ATK_AIR_Y_END);

	ATK_PARRY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_PARRY")), NOLOOP);
	ATK_PARRY->AddEvent(4 / 20.0f, ATK_KT_START);
	ATK_PARRY->AddEvent(10 / 20.0f, ATK_KT_END);
	Layer->AddNode(ATK_PARRY);

	ATK_ONDASH_UPPER = AnimatorSingleNode::Create(GetClip(TEXT("ATK_ONDASH_UPPER")), NOLOOP);
	ATK_ONDASH_UPPER->AddEvent(8 / 39.0f, ATK_KT_START);
	ATK_ONDASH_UPPER->AddEvent(12 / 39.0f, ATK_KT_END);
	Layer->AddNode(ATK_ONDASH_UPPER);
}

void PlayerAnimator::SetupTransitions()
{
	// BH_IDLE -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_IDLE, BH_RUN_BH_DASH, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_RUN_BH_DASH -> BH_IDLE
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::LESS_EQAUL));
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(IsPlayingGuardBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_RUN_BH_DASH, BH_IDLE, values, 0.0f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_JUMP -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_JUMP, BH_FALL, values, 0.6f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_AIR_JUMP -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_AIR_JUMP, BH_FALL, values, 0.6f, 0.5f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_IDLE -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_IDLE, BH_FALL, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_RUN_BH_DASH -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_RUN_BH_DASH, BH_FALL, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
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
		Layer->AddTransition(BH_FALL, BH_LAND, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::CurrentNext);
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
		Layer->AddTransition(ANY, BH_ROLL, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// BH_ROLL -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(BH_ROLL, EXIT, values, 0.9f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_ROLL -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		//Layer->AddTransition(BH_ROLL, BH_RUN_BH_DASH, values, 0.55f, 0.3f);
		Layer->AddTransition(BH_ROLL, BH_RUN_BH_DASH, values, 0.6f, 0.3f);
	}

	// ANY -> GAD_START
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(GuardStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(IsPlayingGuardBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, GAD_START, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// GAD_START -> GAD_LOOP
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(GAD_START, GAD_LOOP, values, 1.0f, 0.0f);
	}

	// GAD_LOOP -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GuardStateBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(GAD_LOOP, EXIT, values, 0.0f, 0.05f);
	}

	// ANY -> GAD_HIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(GuardHitTProperty));
		Layer->AddTransition(ANY, GAD_HIT, values, 0.0f, 0.1f);
	}

	// GAD_HIT -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(GAD_HIT, EXIT, values, 0.7f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// GAD_HIT -> GAD_LOOP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(GuardStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(GAD_HIT, GAD_LOOP, values, 0.6f, 0.1f);
	}

	// ANY -> GAD_BREAK
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(GuardBreakTProperty));
		Layer->AddTransition(ANY, GAD_BREAK, values, 0.0f, 0.1f);
	}

	// GAD_BREAK -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(GAD_BREAK, EXIT, values, 0.8f, 0.1f);
	}

	// ANY -> DMG_LIGHT
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 0, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_LIGHT, values, 0.0f, 0.1f);
	}

	// ANY -> DMG_HEAVY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 1, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_HEAVY, values, 0.0f, 0.1f);
	}

	// ANY -> DMG_BLOW_START
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 2, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_BLOW_START, values, 0.0f, 0.1f);
	}

	// DMG_BLOW_START -> DMG_BLOW_LOOP
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_BLOW_START, DMG_BLOW_LOOP, values, 1.0f, 0.0f);
	}

	// DMG_BLOW_LOOP -> DMG_BLOW_END
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(DMG_BLOW_LOOP, DMG_BLOW_END, values, 0.0f, 0.1f);
	}

	// DMG_BLOW_END -> DMG_BLOW_GETUP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(DMG_BLOW_END, DMG_BLOW_GETUP, values, 1.0f, 0.0f);
	}

	// DMG_BLOW_GETUP -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_BLOW_GETUP, EXIT, values, 0.7f, 0.1f);
	}

	// ANY -> ATK_X
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(IsPlayingGuardBProperty, false, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::LESS_EQAUL));
		Layer->AddTransition(ANY, ATK_X, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_X -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_X, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_X -> ATK_XX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackComboTProperty));
		Layer->AddTransition(ATK_X, ATK_XX, values, 0.1f, 0.05f);
	}

	// ATK_XX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XX, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_XX -> ATK_XXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackComboTProperty));
		Layer->AddTransition(ATK_XX, ATK_XXX, values, 0.35f, 0.1f);
	}

	// ATK_XXX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXX, EXIT, values, 0.5f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_XXX -> ATK_XXXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackComboTProperty));
		Layer->AddTransition(ATK_XXX, ATK_XXXX, values, 0.3f, 0.1f);
	}

	// ATK_XXXX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXX, EXIT, values, 0.5f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_XXXX -> ATK_XXXXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackComboTProperty));
		Layer->AddTransition(ATK_XXXX, ATK_XXXXX, values, 0.3f, 0.1f);
	}

	// ATK_XXXXX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXXX, EXIT, values, 0.7f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> ATK_AIR_X
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ANY, ATK_AIR_X, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_AIR_X -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_AIR_X, BH_FALL, values, 0.6f, 0.3f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_AIR_X -> ATK_AIR_XX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackComboTProperty));
		Layer->AddTransition(ATK_AIR_X, ATK_AIR_XX, values, 0.4f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_AIR_XX -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_AIR_XX, BH_FALL, values, 0.6f, 0.3f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_AIR_XX -> ATK_AIR_XXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackComboTProperty));
		Layer->AddTransition(ATK_AIR_XX, ATK_AIR_XXX, values, 0.4f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_AIR_XXX -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_AIR_XXX, BH_FALL, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> ATK_AIR_Y_START
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, false, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ANY, ATK_AIR_Y_START, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_AIR_Y_START -> ATK_AIR_Y_LOOP
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_AIR_Y_START, ATK_AIR_Y_LOOP, values, 1.0f);
	}

	// ATK_AIR_Y_LOOP -> ATK_AIR_Y_END
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ATK_AIR_Y_LOOP, ATK_AIR_Y_END, values, 0.0f, 0.1f);
	}

	// ATK_AIR_Y_END -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_AIR_Y_END, EXIT, values, 0.6f, 0.1f);
	}

	// GAD_HIT -> ATK_PARRY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(GAD_HIT, ATK_PARRY, values, 0.0f, 0.1f);
	}

	// ATK_PARRY -> ATK_X
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ATK_PARRY, ATK_X, values, 0.5f, 0.3f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_PARRY -> ATK_Y
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_PARRY, ATK_Y, values, 0.5f, 0.3f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_PARRY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_PARRY, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> ATK_Y
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::LESS_EQAUL));
		Layer->AddTransition(ANY, ATK_Y, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
	}

	// ATK_Y -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_Y, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_Y -> ATK_YY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackComboTProperty));
		Layer->AddTransition(ATK_Y, ATK_YY, values, 0.4f, 0.03f);
	}

	// ATK_YY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_YY, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_YY -> ATK_YYY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackComboTProperty));
		Layer->AddTransition(ATK_YY, ATK_YYY, values, 0.5f, 0.1f);
	}

	// ATK_YYY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_YYY, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ATK_YYY -> ATK_XXXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackComboTProperty));
		Layer->AddTransition(ATK_YYY, ATK_XXXXXY, values, 0.5f, 0.1f);
	}

	// ATK_X -> ATK_XY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightToHeavtAttackTProperty));
		Layer->AddTransition(ATK_X, ATK_XY, values, 0.0f, 0.1f);
	}

	// ATK_XX -> ATK_XY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightToHeavtAttackTProperty));
		Layer->AddTransition(ATK_XX, ATK_XY, values, 0.0f, 0.1f);
	}

	// ATK_XY -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XY, BH_FALL, values, 0.6f, 0.3f);
	}

	// ATK_XXXX -> ATK_XXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightToHeavtAttackTProperty));
		Layer->AddTransition(ATK_XXXX, ATK_XXXXY, values, 0.0f, 0.1f);
	}

	// ATK_XXXXY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXXY, EXIT, values, 0.8f, 0.1f);
	}

	// ATK_XXXXX -> ATK_XXXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightToHeavtAttackTProperty));
		Layer->AddTransition(ATK_XXXXX, ATK_XXXXXY, values, 0.0f, 0.1f);
	}

	// ATK_XXXXXY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXXXY, EXIT, values, 0.9f, 0.1f);
	}

	// BH_RUN_BH_DASH -> ATK_XXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::GREATER));
		Layer->AddTransition(BH_RUN_BH_DASH, ATK_XXX, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current);
	}

	// BH_RUN_BH_DASH -> ATK_ONDASH_UPPER
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::GREATER));
		Layer->AddTransition(BH_RUN_BH_DASH, ATK_ONDASH_UPPER, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current);
	}

	// ATK_ONDASH_UPPER -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_ONDASH_UPPER, EXIT, values, 0.5f, 0.1f);
	}
}
