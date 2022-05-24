#include "stdafx.h"
#include "PlayerAnimator.h"
#include "Config.h"

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
	RollTProperty = Layer->AddProperty(TEXT("RollT"), AnimatorProperty::Type::TRIGGER);
	LightAttackTProperty = Layer->AddProperty(TEXT("LightAttackT"), AnimatorProperty::Type::TRIGGER);
	HeavyAttackTProperty = Layer->AddProperty(TEXT("HeavyAttackT"), AnimatorProperty::Type::TRIGGER);
	GuardStateBProperty = Layer->AddProperty(TEXT("GuardStateB"), AnimatorProperty::Type::BOOL);
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
		GAD_START_NODASH->speed = 1.5f;
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
		DMG_LIGHT->speed = 2.0f;
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
	ATK_X->AddEvent(7 / 34.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_X->AddEvent(10 / 34.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_X);

	ATK_XX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XX")), NOLOOP);
	ATK_XX->AddEvent(12 / 45.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_XX->AddEvent(17 / 45.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_XX);

	ATK_XXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXX")), NOLOOP);
	ATK_XXX->AddEvent(8 / 70.0f, ANIM_ATK_FOOT_START | ANIM_ATK_HEAVY);
	ATK_XXX->AddEvent(17 / 70.0f, ANIM_ATK_FOOT_END);
	Layer->AddNode(ATK_XXX);

	ATK_XXXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXX")), NOLOOP);
	ATK_XXXX->AddEvent(10 / 35.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_XXXX->AddEvent(15 / 35.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_XXXX);

	ATK_XXXXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXX")), NOLOOP);
	ATK_XXXXX->AddEvent(18 / 53.0f, ANIM_ATK_KT_START | ANIM_ATK_HEAVY);
	ATK_XXXXX->AddEvent(23 / 53.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_XXXXX);

	ATK_AIR_X = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_X")), NOLOOP);
	ATK_AIR_X->speed = 1.2f;
	ATK_AIR_X->AddEvent(4 / 40.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_AIR_X->AddEvent(8 / 40.0f, ANIM_ATK_KT_END);
	ATK_AIR_X->AddEvent(12 / 40.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_AIR_X->AddEvent(16 / 40.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_AIR_X);

	ATK_AIR_XX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_XX")), NOLOOP);
	ATK_AIR_XX->speed = 1.2f;
	ATK_AIR_XX->AddEvent(11 / 40.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_AIR_XX->AddEvent(19 / 40.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_AIR_XX);

	ATK_AIR_XXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_XXX")), NOLOOP);
	ATK_AIR_XXX->speed = 1.2f;
	ATK_AIR_XXX->AddEvent(8 / 34.0f, ANIM_ATK_FOOT_START | ANIM_ATK_LIGHT);
	ATK_AIR_XXX->AddEvent(14 / 34.0f, ANIM_ATK_FOOT_END);
	Layer->AddNode(ATK_AIR_XXX);

	ATK_Y = AnimatorSingleNode::Create(GetClip(TEXT("ATK_Y")), NOLOOP);
	ATK_Y->AddEvent(8 / 60.0f, ANIM_ATK_KT_START | ANIM_ATK_HEAVY);
	ATK_Y->AddEvent(14 / 60.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_Y);

	ATK_YY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_YY")), NOLOOP);
	ATK_YY->AddEvent(5 / 72.0f, ANIM_ATK_KT_START | ANIM_ATK_HEAVY);
	ATK_YY->AddEvent(12 / 72.0f, ANIM_ATK_KT_END);
	ATK_YY->AddEvent(15 / 72.0f, ANIM_ATK_KT_START | ANIM_ATK_HEAVY);
	ATK_YY->AddEvent(20 / 72.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_YY);

	ATK_YYY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_YYY")), NOLOOP);
	ATK_YYY->AddEvent(9 / 49.0f, ANIM_ATK_KT_STING_START | ANIM_ATK_BLOW);
	ATK_YYY->AddEvent(12 / 49.0f, ANIM_ATK_KT_STING_END);
	Layer->AddNode(ATK_YYY);

	ATK_XY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XY")), NOLOOP);
	ATK_XY->AddEvent(5 / 30.0f, ANIM_ATK_KT_START | ANIM_ATK_BLOWUP | ANIM_JUMP);
	ATK_XY->AddEvent(14 / 30.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_XY);

	ATK_XXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXY")), NOLOOP);
	ATK_XXXY->AddEvent(8 / 68.0f, ANIM_ATK_FOOT_START | ANIM_ATK_HEAVY);
	ATK_XXXY->AddEvent(18 / 68.0f, ANIM_ATK_FOOT_START | ANIM_ATK_HEAVY);
	ATK_XXXY->AddEvent(26 / 68.0f, ANIM_ATK_FOOT_START | ANIM_ATK_HEAVY);
	ATK_XXXY->AddEvent(32 / 68.0f, ANIM_ATK_FOOT_END);
	Layer->AddNode(ATK_XXXY);

	ATK_XXXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXY")), NOLOOP);
	ATK_XXXXY->AddEvent(11 / 42.0f, ANIM_ATK_KT_START | ANIM_ATK_HEAVY);
	ATK_XXXXY->AddEvent(16 / 42.0f, ANIM_ATK_KT_END);
	ATK_XXXXY->AddEvent(22 / 42.0f, ANIM_ATK_KT_START | ANIM_ATK_HEAVY);
	ATK_XXXXY->AddEvent(27 / 42.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_XXXXY);

	ATK_XXXXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXXY")), NOLOOP);
	ATK_XXXXXY->AddEvent(11 / 52.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_XXXXXY->AddEvent(19 / 52.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_XXXXXY->AddEvent(29 / 52.0f, ANIM_ATK_KT_START | ANIM_ATK_LIGHT);
	ATK_XXXXXY->AddEvent(35 / 52.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_XXXXXY);

	ATK_AIR_Y_START = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_START")), NOLOOP);
	ATK_AIR_Y_START->AddEvent(11 / 13.0f, ANIM_ATK_KT_START | ANIM_ATK_BLOWDOWN);
	ATK_AIR_Y_START->speed = 1.2f;
	Layer->AddNode(ATK_AIR_Y_START);

	ATK_AIR_Y_LOOP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_LOOP")), LOOP);
	ATK_AIR_Y_LOOP->AddEvent(0.0f, ANIM_ATK_KT_START | ANIM_ATK_BLOWDOWN);
	Layer->AddNode(ATK_AIR_Y_LOOP);

	ATK_AIR_Y_END = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_END")), NOLOOP);
	ATK_AIR_Y_END->AddEvent(0.0f, ANIM_ATK_KT_START | ANIM_ATK_BLOW);
	ATK_AIR_Y_END->AddEvent(5 / 35.0f, ANIM_ATK_KT_END);
	Layer->AddNode(ATK_AIR_Y_END);
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
		Layer->AddTransition(ANY, BH_ROLL, values, 0.0f, 0.1f, 0.1f, AnimatorTransition::Interrupt::Current, true);
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
		auto transition = Layer->AddTransition(ANY, GAD_START, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
		transition->SetCallback(this);
	}

	// GAD_START -> GAD_LOOP
	{
		vector<AnimatorTransition::PropertyValue> values;
		auto transition = Layer->AddTransition(GAD_START, GAD_LOOP, values, 0.5f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
		transition->SetCallback(this);
	}

	// GAD_LOOP -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GuardStateBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(GAD_LOOP, EXIT, values, 0.0f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
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
		auto transition = Layer->AddTransition(GAD_HIT, GAD_LOOP, values, 0.6f, 0.1f);
		transition->SetCallback(this);
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

	// DMG_LIGHT -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_LIGHT, EXIT, values, 0.8f, 0.1f);
	}

	// ANY -> DMG_HEAVY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(DamageTProperty));
		values.push_back(AnimatorTransition::PropertyValue(DamageTypeIProperty, 1, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, DMG_HEAVY, values, 0.0f, 0.1f);
	}

	// DMG_HEAVY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_HEAVY, EXIT, values, 0.8f, 0.1f);
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
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::LESS_EQAUL));
		auto transition = Layer->AddTransition(ANY, ATK_X, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::Current, true);
		transition->SetCallback(this);
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ATK_X, ATK_XX, values, 0.1f, 0.05f, 0.1f);
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
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
		auto transition = Layer->AddTransition(ANY, ATK_AIR_X, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
		transition->SetCallback(this);
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ATK_AIR_X, ATK_AIR_XX, values, 0.3f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
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
		auto transition = Layer->AddTransition(ANY, ATK_AIR_Y_START, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::Current, true);
		transition->SetCallback(this);
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

	// ANY -> ATK_Y
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::LESS_EQAUL));
		auto transition = Layer->AddTransition(ANY, ATK_Y, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
		transition->SetCallback(this);
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
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
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_YYY, ATK_XXXXXY, values, 0.5f, 0.1f);
	}

	// ATK_X -> ATK_XY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_X, ATK_XY, values, 0.1f, 0.1f);
	}

	// ATK_XX -> ATK_XY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_XX, ATK_XY, values, 0.1f, 0.1f);
	}

	// ATK_XY -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XY, BH_FALL, values, 0.5f, 0.2f);
	}

	// ATK_XXX -> ATK_XXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_XXX, ATK_XXXY, values, 0.2f, 0.1f);
	}

	// ATK_XXXY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXY, EXIT, values, 0.85f, 0.1f);
	}

	// ATK_XXXX -> ATK_XXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_XXXX, ATK_XXXXY, values, 0.2f, 0.1f);
	}

	// ATK_XXXXY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXXY, EXIT, values, 0.8f, 0.1f);
	}

	// ATK_XXXXX -> ATK_XXXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_XXXXX, ATK_XXXXXY, values, 0.2f, 0.1f);
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

	// BH_RUN_BH_DASH -> ATK_XY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(HasGroundBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::GREATER));
		Layer->AddTransition(BH_RUN_BH_DASH, ATK_XY, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current);
	}
}

bool PlayerAnimator::IsPlayingJump() const
{
	return
		Layer->IsPlaying(BH_JUMP) ||
		Layer->IsPlaying(BH_AIR_JUMP) ||
		Layer->IsPlaying(BH_LAND);
}

bool PlayerAnimator::IsPlayingGuard() const
{
	return
		Layer->IsPlaying(GAD_START) ||
		Layer->IsPlaying(GAD_LOOP) ||
		Layer->IsPlaying(GAD_HIT) ||
		Layer->IsPlaying(GAD_BREAK);
}

bool PlayerAnimator::IsPlayingAttack() const
{
	return
		Layer->IsPlaying(ATK_X) ||
		Layer->IsPlaying(ATK_XX) ||
		Layer->IsPlaying(ATK_XXX) ||
		Layer->IsPlaying(ATK_XXXX) ||
		Layer->IsPlaying(ATK_XXXXX) ||
		Layer->IsPlaying(ATK_AIR_X) ||
		Layer->IsPlaying(ATK_AIR_XX) ||
		Layer->IsPlaying(ATK_AIR_XXX) ||
		Layer->IsPlaying(ATK_Y) ||
		Layer->IsPlaying(ATK_YY) ||
		Layer->IsPlaying(ATK_YYY) ||
		Layer->IsPlaying(ATK_XY) ||
		Layer->IsPlaying(ATK_XXXXY) ||
		Layer->IsPlaying(ATK_XXXXXY) ||
		Layer->IsPlaying(ATK_AIR_Y_START) ||
		Layer->IsPlaying(ATK_AIR_Y_LOOP) ||
		Layer->IsPlaying(ATK_AIR_Y_END);
}

bool PlayerAnimator::IsPlayingDamage() const
{
	return
		Layer->IsPlaying(DMG_LIGHT) ||
		Layer->IsPlaying(DMG_HEAVY) ||
		Layer->IsPlaying(DMG_BLOW_START) ||
		Layer->IsPlaying(DMG_BLOW_LOOP) ||
		Layer->IsPlaying(DMG_BLOW_END) ||
		Layer->IsPlaying(DMG_BLOW_GETUP);
}

bool PlayerAnimator::IsPlayingAirAction() const
{
	return
		Layer->IsPlaying(ATK_AIR_X) ||
		Layer->IsPlaying(ATK_AIR_XX) ||
		Layer->IsPlaying(ATK_AIR_XXX) ||
		Layer->IsPlaying(ATK_AIR_Y_START);
}

bool PlayerAnimator::IsPlayingGuardableAnimation(bool withoutGuardHit) const
{
	if (Layer->IsPlaying(GAD_START) ||
		Layer->IsPlaying(GAD_LOOP))
		return true;

	if (!withoutGuardHit &&
		Layer->IsPlaying(GAD_HIT))
		return true;

	return false;
}

bool PlayerAnimator::IsPlayingNonAttackableAnimation() const
{
	if (IsPlayingGuardableAnimation(true))
		return true;

	if (IsPlayingDamage())
		return true;

	return false;
}

bool PlayerAnimator::IsPlayingNonJumpableAnimation() const
{
	if (IsPlayingAttack())
		return true;

	return false;
}

bool PlayerAnimator::IsPlayingNonMovableAnimation() const
{
	if (IsPlayingAttack())
		return true;

	if (IsPlayingDamage())
		return true;

	return false;
}

bool PlayerAnimator::IsPlayingNonRollableAnimation() const
{
	if (IsPlayingDamage())
		return true;

	return false;
}

bool PlayerAnimator::IsPlayingNonGuardableAnimation() const
{
	if (IsPlayingAttack())
		return true;

	if (Layer->IsPlaying(BH_ROLL))
		return true;

	return false;
}

bool PlayerAnimator::Transferable(
	Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition,
	AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const
{
	if (transition->nextNode == ATK_X ||
		transition->nextNode == ATK_Y ||
		transition->nextNode == ATK_AIR_X)
	{
		if (layer->IsContains(TEXT("ATK")))
			return false;
	}

	if (transition->nextNode == ATK_AIR_Y_START)
	{
		if (layer->IsContains(TEXT("ATK_AIR_Y")) ||
			layer->IsPlaying(ATK_XY))
			return false;
	}

	if (transition->nextNode == GAD_START)
	{
		if (IsPlayingGuard() ||
			IsPlayingDamage() ||
			IsPlayingAttack() ||
			IsPlayingJump())
			return false;
	}

	if (transition->nextNode == GAD_LOOP)
	{
		if (IsPlayingDamage() ||
			IsPlayingAttack() ||
			IsPlayingJump())
			return false;
	}

	return true;
}
