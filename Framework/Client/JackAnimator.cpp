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
	m_jackMesh = system->resourceManagement->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));
	return nullptr != m_jackMesh;
}

void JackAnimator::SetupProperties()
{
	weaponTypeProperty = new AnimatorProperty(TEXT("WeaponType"), AnimatorProperty::Type::FLOAT);
	m_layer->AddProperty(weaponTypeProperty);

	moveTypeProperty = new AnimatorProperty(TEXT("MoveType"), AnimatorProperty::Type::FLOAT);
	m_layer->AddProperty(moveTypeProperty);

	moveProperty = new AnimatorProperty(TEXT("MoveBool"), AnimatorProperty::Type::FLOAT);
	m_layer->AddProperty(moveProperty);

	attackProperty = new AnimatorProperty(TEXT("Attack"), AnimatorProperty::Type::TRIGGER);
	m_layer->AddProperty(attackProperty);

	comboProperty = new AnimatorProperty(TEXT("Combo"), AnimatorProperty::Type::TRIGGER);
	m_layer->AddProperty(comboProperty);

	jumpProperty = new AnimatorProperty(TEXT("Jump"), AnimatorProperty::Type::TRIGGER);
	m_layer->AddProperty(jumpProperty);

	jumpingProperty = new AnimatorProperty(TEXT("Jumping"), AnimatorProperty::Type::TRIGGER);
	m_layer->AddProperty(jumpingProperty);

	hasGroundProperty = new AnimatorProperty(TEXT("HasGround"), AnimatorProperty::Type::BOOL);
	m_layer->AddProperty(hasGroundProperty);
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

	AnimatorTransition* transitionIdleToMove = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(moveProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		transitionIdleToMove = AnimatorTransition::Create(blendNodeIdle, blendNodeMove, values, 0.0f, 0.2f);
	}
	m_layer->AddTransition(transitionIdleToMove);

	AnimatorTransition* transitionAnyToJump = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(jumpProperty));
		transitionAnyToJump = AnimatorTransition::Create(nullptr, blendNodeBeginJump, values, 0.0f, 0.1f);
	}
	m_layer->AddTransition(transitionAnyToJump);

	AnimatorTransition* transitionAnyToJumping = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(jumpingProperty));
		transitionAnyToJumping = AnimatorTransition::Create(nullptr, blendNodeJumping, values, 0.0f, 0.1f);
	}
	m_layer->AddTransition(transitionAnyToJumping);

	AnimatorTransition* transitionJumpToLand = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(hasGroundProperty, true, AnimatorTransition::Compare::EQUAL));
		transitionJumpToLand = AnimatorTransition::Create(blendNodeBeginJump, blendNodeLand, values, 0.0f, 0.1f);
	}
	m_layer->AddTransition(transitionJumpToLand);

	AnimatorTransition* transitionJumpingToLand = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(hasGroundProperty, true, AnimatorTransition::Compare::EQUAL));
		transitionJumpingToLand = AnimatorTransition::Create(blendNodeJumping, blendNodeLand, values, 0.0f, 0.1f);
	}
	m_layer->AddTransition(transitionJumpingToLand);

	AnimatorTransition* transitionExitLand = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		transitionExitLand = AnimatorTransition::Create(blendNodeLand, nullptr, values, 0.9f, 0.1f);
	}
	m_layer->AddTransition(transitionExitLand);

	AnimatorTransition* transitionMoveToIdle = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(moveProperty, 0.0f, AnimatorTransition::Compare::LESS_EQAUL));
		transitionMoveToIdle = AnimatorTransition::Create(blendNodeMove, blendNodeIdle, values, 0.0f, 0.1f);
	}
	m_layer->AddTransition(transitionMoveToIdle);

	AnimatorTransition* transitionAttackX = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(attackProperty));
		transitionAttackX = AnimatorTransition::Create(nullptr, blendNodeAtkX, values, 0.0f, 0.1f);
	}
	m_layer->AddTransition(transitionAttackX);

	AnimatorTransition* transitionAttackXX = nullptr;
	AnimatorTransition* transitionAttackXXX = nullptr;
	AnimatorTransition* transitionAttackXXXX = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(comboProperty));
		transitionAttackXX = AnimatorTransition::Create(blendNodeAtkX, blendNodeAtkXX, values, 0.0f, 0.1f, 0.1f);
		transitionAttackXXX = AnimatorTransition::Create(blendNodeAtkXX, blendNodeAtkXXX, values, 0.0f, 0.1f);
		transitionAttackXXXX = AnimatorTransition::Create(blendNodeAtkXXX, blendNodeAtkXXXX, values, 0.0f, 0.1f, 0.05f);
	}
	m_layer->AddTransition(transitionAttackXX);
	m_layer->AddTransition(transitionAttackXXX);
	m_layer->AddTransition(transitionAttackXXXX);

	AnimatorTransition* transitionExitAttackX = nullptr;
	AnimatorTransition* transitionExitAttackXX = nullptr;
	AnimatorTransition* transitionExitAttackXXX = nullptr;
	AnimatorTransition* transitionExitAttackXXXX = nullptr;
	{
		vector<AnimatorTransition::PropertyValue> values;
		transitionExitAttackX = AnimatorTransition::Create(blendNodeAtkX, nullptr, values, 0.5f, 0.2f);
		transitionExitAttackXX = AnimatorTransition::Create(blendNodeAtkXX, nullptr, values, 0.6f, 0.1f);
		transitionExitAttackXXX = AnimatorTransition::Create(blendNodeAtkXXX, nullptr, values, 0.9f, 0.1f);
		transitionExitAttackXXXX = AnimatorTransition::Create(blendNodeAtkXXXX, nullptr, values, 0.9f, 0.05f, 0.05f);
	}
	m_layer->AddTransition(transitionExitAttackX);
	m_layer->AddTransition(transitionExitAttackXX);
	m_layer->AddTransition(transitionExitAttackXXX);
	m_layer->AddTransition(transitionExitAttackXXXX);
}
