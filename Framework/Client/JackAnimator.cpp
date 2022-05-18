#include "stdafx.h"
#include "JackAnimator.h"

void JackAnimator::Awake()
{
	Animator::Awake();

	Initialize();
}

bool JackAnimator::Initialize()
{
	if (!FindJackMesh())
		RETURN_FALSE_ERROR_MESSAGE("JackAnimator::FindJackMesh");

	m_layer = new AnimatorLayer;

	SetupProperties();
	SetupMainLayer();

	AddLayer(m_layer);
	m_layer->SetRootNodeByName(TEXT("CharacterRoot"));

	return true;
}

bool JackAnimator::FindJackMesh()
{
	m_jackMesh = system->resource->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));
	return nullptr != m_jackMesh;
}

void JackAnimator::SetupProperties()
{
	weaponTypeProperty = m_layer->AddProperty(TEXT("WeaponType"), AnimatorProperty::Type::FLOAT);
	moveTypeProperty = m_layer->AddProperty(TEXT("MoveType"), AnimatorProperty::Type::FLOAT);
	moveProperty = m_layer->AddProperty(TEXT("MoveBool"), AnimatorProperty::Type::FLOAT);
	attackProperty = m_layer->AddProperty(TEXT("Attack"), AnimatorProperty::Type::TRIGGER);
	comboProperty = m_layer->AddProperty(TEXT("Combo"), AnimatorProperty::Type::TRIGGER);
	jumpProperty = m_layer->AddProperty(TEXT("Jump"), AnimatorProperty::Type::TRIGGER);
	jumpingProperty = m_layer->AddProperty(TEXT("Jumping"), AnimatorProperty::Type::TRIGGER);
	hasGroundProperty = m_layer->AddProperty(TEXT("HasGround"), AnimatorProperty::Type::BOOL);
}

void JackAnimator::SetupMainLayer()
{
	// Setup Nodes ===============================================================================================================================================================

	AnimatorBlendNode* blendNodeIdle = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneIdle;
		AnimatorSingleNode* snIdleKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_IDL_01_Lp")), true);
		bneIdle.push_back(AnimatorBlendNodeElement::Create(snIdleKatana, 0.0f));
		blendNodeIdle = AnimatorBlendNode::Create(TEXT("Idle"), bneIdle, weaponTypeProperty, true);
	}
	m_layer->AddNode(blendNodeIdle);

	AnimatorBlendNode* blendNodeMove = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneMove;;

		AnimatorBlendNode* blendNodeRun = nullptr;
		{
			vector<Ref<AnimatorBlendNodeElement>> bneRun;
			AnimatorSingleNode* snRunKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_RUN_Lp")), true);
			bneRun.push_back(AnimatorBlendNodeElement::Create(snRunKatana, 0.0f));
			blendNodeRun = AnimatorBlendNode::Create(TEXT("Run"), bneRun, weaponTypeProperty, true);
		}
		bneMove.push_back(AnimatorBlendNodeElement::Create(blendNodeRun, 0.0f));

		AnimatorBlendNode* blendNodeDash = nullptr;
		{
			vector<Ref<AnimatorBlendNodeElement>> bneDashRun;
			AnimatorSingleNode* snDashKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_IDL_01_Lp")), true);
			bneDashRun.push_back(AnimatorBlendNodeElement::Create(snDashKatana, 0.0f));
			blendNodeDash = AnimatorBlendNode::Create(TEXT("Dash"), bneDashRun, weaponTypeProperty, true);
		}
		bneMove.push_back(AnimatorBlendNodeElement::Create(blendNodeDash, 1.0f));

		blendNodeMove = AnimatorBlendNode::Create(TEXT("Move"), bneMove, moveTypeProperty, true);
	}
	m_layer->AddNode(blendNodeMove);

	AnimatorBlendNode* blendNodeBeginJump = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneBeginJump;
		AnimatorSingleNode* snBeginJumpKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_JMP_VT")), false);
		bneBeginJump.push_back(AnimatorBlendNodeElement::Create(snBeginJumpKatana, 0.0f));
		blendNodeBeginJump = AnimatorBlendNode::Create(TEXT("BeginJump"), bneBeginJump, weaponTypeProperty, false);
	}
	m_layer->AddNode(blendNodeBeginJump);

	AnimatorBlendNode* blendNodeJumping = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneJumping;
		AnimatorSingleNode* snJumppingKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_JMP_F")), false);
		bneJumping.push_back(AnimatorBlendNodeElement::Create(snJumppingKatana, 0.0f));
		blendNodeJumping = AnimatorBlendNode::Create(TEXT("Jumping"), bneJumping, weaponTypeProperty, false);
	}
	m_layer->AddNode(blendNodeJumping);

	AnimatorBlendNode* blendNodeLand = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneLand;
		AnimatorSingleNode* snLandKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_LND")), false);
		bneLand.push_back(AnimatorBlendNodeElement::Create(snLandKatana, 0.0f));
		blendNodeLand = AnimatorBlendNode::Create(TEXT("Land"), bneLand, weaponTypeProperty, false);
	}
	m_layer->AddNode(blendNodeLand);

	AnimatorBlendNode* blendNodeAtkX = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneAtkX;
		AnimatorSingleNode* snAtkXKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_01Y")), false);
		bneAtkX.push_back(AnimatorBlendNodeElement::Create(snAtkXKatana, 0.0f));
		blendNodeAtkX = AnimatorBlendNode::Create(TEXT("AtkX"), bneAtkX, weaponTypeProperty, false);
	}
	m_layer->AddNode(blendNodeAtkX);

	AnimatorBlendNode* blendNodeAtkXX = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneAtkXX;
		AnimatorSingleNode* snAtkXXKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_01YY")), false);
		bneAtkXX.push_back(AnimatorBlendNodeElement::Create(snAtkXXKatana, 0.0f));
		blendNodeAtkXX = AnimatorBlendNode::Create(TEXT("AtkXX"), bneAtkXX, weaponTypeProperty, false);
	}
	m_layer->AddNode(blendNodeAtkXX);

	AnimatorBlendNode* blendNodeAtkXXX = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneAtkXXX;
		AnimatorSingleNode* snAtkXXXKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_Kick")), false);
		bneAtkXXX.push_back(AnimatorBlendNodeElement::Create(snAtkXXXKatana, 0.0f));
		blendNodeAtkXXX = AnimatorBlendNode::Create(TEXT("AtkXXX"), bneAtkXXX, weaponTypeProperty, false);
	}
	m_layer->AddNode(blendNodeAtkXXX);

	AnimatorBlendNode* blendNodeAtkXXXX = nullptr;
	{
		vector<Ref<AnimatorBlendNodeElement>> bneAtkXXXX;
		AnimatorSingleNode* snAtkXXXXKatana = AnimatorSingleNode::Create(m_jackMesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_Jump_Slashing")), false);
		bneAtkXXXX.push_back(AnimatorBlendNodeElement::Create(snAtkXXXXKatana, 0.0f));
		blendNodeAtkXXXX = AnimatorBlendNode::Create(TEXT("AtkXXXX"), bneAtkXXXX, weaponTypeProperty, false);
	}
	m_layer->AddNode(blendNodeAtkXXXX);

	// Setup Transitions =========================================================================================================================================================

	{	// Idle to Move
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(moveProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		m_layer->AddTransition(blendNodeIdle, blendNodeMove, values, 0.0f, 0.2f);
	}

	{	// Any to Jump
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(jumpProperty));
		m_layer->AddTransition(nullptr, blendNodeBeginJump, values, 0.0f, 0.1f);
	}

	{	// Any to Jumping
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(jumpingProperty));
		m_layer->AddTransition(nullptr, blendNodeJumping, values, 0.0f, 0.1f);
	}

	{	// Jump to Land
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(hasGroundProperty, true, AnimatorTransition::Compare::EQUAL));
		m_layer->AddTransition(blendNodeBeginJump, blendNodeLand, values, 0.0f, 0.1f);
	}

	{	// Jumping to Land
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(hasGroundProperty, true, AnimatorTransition::Compare::EQUAL));
		m_layer->AddTransition(blendNodeJumping, blendNodeLand, values, 0.0f, 0.1f);
	}

	{	// Exit Land
		vector<AnimatorTransition::PropertyValue> values;
		m_layer->AddTransition(blendNodeLand, nullptr, values, 0.9f, 0.1f);
	}

	{	// Land to Move
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(moveProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		m_layer->AddTransition(blendNodeLand, blendNodeMove, values, 0.0f, 0.2f);
	}

	{	// Move to Idle
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(moveProperty, 0.0f, AnimatorTransition::Compare::LESS_EQAUL));
		m_layer->AddTransition(blendNodeMove, blendNodeIdle, values, 0.0f, 0.1f);
	}

	{	// Any to AttackX
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(attackProperty));
		m_layer->AddTransition(nullptr, blendNodeAtkX, values, 0.0f, 0.1f);
	}

	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(comboProperty));
		// AttackX to AttackXX
		m_layer->AddTransition(blendNodeAtkX, blendNodeAtkXX, values, 0.0f, 0.1f, 0.1f);
		// AttackXX to AttackXXX
		m_layer->AddTransition(blendNodeAtkXX, blendNodeAtkXXX, values, 0.0f, 0.1f);
		// AttackXXX to AttackXXXX
		m_layer->AddTransition(blendNodeAtkXXX, blendNodeAtkXXXX, values, 0.0f, 0.1f, 0.05f);
	}

	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(moveProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		// AttackX to Move
		m_layer->AddTransition(blendNodeAtkX, nullptr, values, 0.4f, 0.1f);
		// AttackXX to Move
		m_layer->AddTransition(blendNodeAtkXX, nullptr, values, 0.5f, 0.1f);
		// AttackXXX to Move
		m_layer->AddTransition(blendNodeAtkXXX, nullptr, values, 0.45f, 0.1f);
		// AttackXXXX to Move
		m_layer->AddTransition(blendNodeAtkXXXX, nullptr, values, 0.65f, 0.1f);
	}

	{
		vector<AnimatorTransition::PropertyValue> values;
		// Exit AttackX
		m_layer->AddTransition(blendNodeAtkX, nullptr, values, 0.5f, 0.2f);
		// Exit AttackXX
		m_layer->AddTransition(blendNodeAtkXX, nullptr, values, 0.6f, 0.1f);
		// Exit AttackXXX
		m_layer->AddTransition(blendNodeAtkXXX, nullptr, values, 0.9f, 0.1f);
		// Exit AttackXXXX
		m_layer->AddTransition(blendNodeAtkXXXX, nullptr, values, 0.9f, 0.05f, 0.05f);
	}
}
