#include "stdafx.h"
#include "PlayerAnimator.h"
#include "Config.h"

#define BH_ROLL_EXITTIME (0.6f)

void PlayerAnimator::Awake()
{
	Animator::Awake();
	SetupMesh();
	SetupLayer();
	SetupProperties();
	SetupNodes();
	SetupTransitions();
	speed = 1.2f;

	GroundStateBProperty->valueAsBool = true;
}

SubResourceRef<AnimationClip> PlayerAnimator::GetClip(const tstring& name)
{
	return Mesh->GetAnimationClipByName(TEXT("ROOT|") + name);
}

void PlayerAnimator::SetupMesh()
{
	Mesh = system->resource->Find(MESH_JACK);
}

void PlayerAnimator::SetupLayer()
{
	Layer = new AnimatorLayer;
	AddLayer(Layer);
	Layer->SetRootNodeByName(TEXT("CharacterRoot"));
}

void PlayerAnimator::SetupProperties()
{
	GroundStateBProperty = Layer->AddProperty(TEXT("GroundStateBProperty"), AnimatorProperty::Type::BOOL);
	MoveStateFProperty = Layer->AddProperty(TEXT("MoveStateFProperty"), AnimatorProperty::Type::FLOAT);
	DashStateFProperty = Layer->AddProperty(TEXT("DashStateFProperty"), AnimatorProperty::Type::FLOAT);
	LastDashStateFProperty = Layer->AddProperty(TEXT("LastDashStateFProperty"), AnimatorProperty::Type::FLOAT);
	JumpTProperty = Layer->AddProperty(TEXT("JumpTProperty"), AnimatorProperty::Type::TRIGGER);
	AirJumpTProperty = Layer->AddProperty(TEXT("AirJumpTProperty"), AnimatorProperty::Type::TRIGGER);
	RollTProperty = Layer->AddProperty(TEXT("RollTProperty"), AnimatorProperty::Type::TRIGGER);
	GuardBProperty = Layer->AddProperty(TEXT("GuardBProperty"), AnimatorProperty::Type::BOOL);
	GuardHitTProperty = Layer->AddProperty(TEXT("GuardHitTProperty"), AnimatorProperty::Type::TRIGGER);
	GuardBreakTProperty = Layer->AddProperty(TEXT("GuardBreakTProperty"), AnimatorProperty::Type::TRIGGER);
	DamageTProperty = Layer->AddProperty(TEXT("DamageTProperty"), AnimatorProperty::Type::TRIGGER);
	DamageDirectionFProperty = Layer->AddProperty(TEXT("DamageDirectionFProperty"), AnimatorProperty::Type::FLOAT);
	DamageTypeIProperty = Layer->AddProperty(TEXT("DamageTypeIProperty"), AnimatorProperty::Type::INT);
	LightAttackTProperty = Layer->AddProperty(TEXT("LightAttackTProperty"), AnimatorProperty::Type::TRIGGER);
	HeavyAttackTProperty = Layer->AddProperty(TEXT("HeavyAttackTProperty"), AnimatorProperty::Type::TRIGGER);
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
		{
			AnimationEventDesc s0, s1;
			s0.NormalizedTime = 3 / 16.0f;
			s1.NormalizedTime = 10 / 16.0f;
			s0.ContextByte = ByteContext::PLAY_SOUND;
			s1.ContextByte = ByteContext::PLAY_SOUND;
			s0.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_01;
			s1.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_02;
			s0.ContextFloat = 0.4f;
			s1.ContextFloat = 0.4f;
			BH_RUN_BH_DASH->AddEvent(s0);
			BH_RUN_BH_DASH->AddEvent(s1);
		}
		Layer->AddNode(BH_RUN_BH_DASH);
	}

	BH_JUMP = AnimatorSingleNode::Create(GetClip(TEXT("BH_JUMP")), NOLOOP);
	{
		AnimationEventDesc desc;
		desc.NormalizedTime = 0.0f;
		desc.ContextByte = ByteContext::JUMP;
		desc.ContextFloat = JUMP_SPEED;
		BH_JUMP->AddEvent(desc);

		AnimationEventDesc s;
		s.NormalizedTime = 0.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_03;
		s.ContextFloat = 1.0f;
		BH_JUMP->AddEvent(s);
	}
	Layer->AddNode(BH_JUMP);

	BH_AIR_JUMP = AnimatorSingleNode::Create(GetClip(TEXT("BH_AIR_JUMP")), NOLOOP);
	{
		AnimationEventDesc desc;
		desc.NormalizedTime = 0.0f;
		desc.ContextByte = ByteContext::JUMP;
		desc.ContextFloat = AIRJUMP_SPEED;
		BH_AIR_JUMP->AddEvent(desc);

		AnimationEventDesc s;
		s.NormalizedTime = 0.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_03;
		s.ContextFloat = 1.0f;
		BH_AIR_JUMP->AddEvent(s);
	}
	Layer->AddNode(BH_AIR_JUMP);

	BH_FALL = AnimatorSingleNode::Create(GetClip(TEXT("BH_FALL")), LOOP);
	Layer->AddNode(BH_FALL);

	BH_LAND = AnimatorSingleNode::Create(GetClip(TEXT("BH_LAND")), NOLOOP);
	{
		AnimationEventDesc s;
		s.NormalizedTime = 0.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_03;
		s.ContextFloat = 1.0f;
		BH_LAND->AddEvent(s);
	}
	Layer->AddNode(BH_LAND);

	BH_ROLL = AnimatorSingleNode::Create(GetClip(TEXT("BH_ROLL")), NOLOOP);
	{
		AnimationEventDesc s0, s1;
		s0.NormalizedTime = 0.0f;
		s1.NormalizedTime = 0.0f;
		s0.ContextByte = ByteContext::PLAY_SOUND;
		s1.ContextByte = ByteContext::PLAY_SOUND;
		s0.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_03;
		s1.ContextTStr = SOUND_BEHAVIOR_ROLL;
		s0.ContextFloat = 1.0f;
		s1.ContextFloat = 0.5f;
		BH_ROLL->AddEvent(s0);
		BH_ROLL->AddEvent(s1);
	}
	Layer->AddNode(BH_ROLL);

	{
		vector<Ref<AnimatorBlendNodeElement>> blendNodeElements;

		AnimatorSingleNode* GAD_START_NODASH = AnimatorSingleNode::Create(GetClip(TEXT("GAD_START_NODASH")), NOLOOP);
		GAD_START_NODASH->speed = 1.5f;
		AnimatorSingleNode* GAD_START_DASH = AnimatorSingleNode::Create(GetClip(TEXT("GAD_START_DASH")), NOLOOP);

		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(GAD_START_NODASH, 0.0f));
		blendNodeElements.push_back(AnimatorBlendNodeElement::Create(GAD_START_DASH, 1.0f));

		GAD_START = AnimatorBlendNode::Create(TEXT("GAD_START"), blendNodeElements, LastDashStateFProperty, NOLOOP);
		{
			AnimationEventDesc s;
			s.NormalizedTime = 0.0f;
			s.ContextByte = ByteContext::PLAY_SOUND;
			s.ContextTStr = SOUND_BEHAVIOR_ARMOR;
			s.ContextFloat = 1.0f;
			GAD_START->AddEvent(s);
		}
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
		DMG_LIGHT->speed = 2.5f;
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
	DMG_BLOW_END->speed = 2.0f;
	Layer->AddNode(DMG_BLOW_END);

	DMG_BLOW_GETUP = AnimatorSingleNode::Create(GetClip(TEXT("DMG_BLOW_GETUP")), NOLOOP);
	DMG_BLOW_GETUP->speed = 1.5f;
	Layer->AddNode(DMG_BLOW_GETUP);

	ATK_X = AnimatorSingleNode::Create(GetClip(TEXT("ATK_X")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 7 / 34.0f;
		e1.NormalizedTime = 10 / 34.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_LIGHT;
		e1.ContextInt = IntContext::KT_END;
		ATK_X->AddEvent(e0);
		ATK_X->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 7 / 34.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_LIGHT01;
		s.ContextFloat = 1.0f;
		ATK_X->AddEvent(s);
	}
	ATK_X->speed = 1.25f;
	Layer->AddNode(ATK_X);

	ATK_XX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XX")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 12 / 45.0f;
		e1.NormalizedTime = 17 / 45.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_LIGHT;
		e1.ContextInt = IntContext::KT_END;
		ATK_XX->AddEvent(e0);
		ATK_XX->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 14 / 45.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_LIGHT02;
		s.ContextFloat = 1.0f;
		ATK_XX->AddEvent(s);
	}
	ATK_XX->speed = 1.25f;
	Layer->AddNode(ATK_XX);

	ATK_XXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXX")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 11 / 70.0f;
		e1.NormalizedTime = 17 / 70.0f;
		e0.ContextInt = IntContext::FOOT_START;
		e0.ContextUInt = UIntContext::ATK_HEAVY;
		e1.ContextInt = IntContext::FOOT_END;
		ATK_XXX->AddEvent(e0);
		ATK_XXX->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 11 / 70.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_MELEE_SWING_01;
		s.ContextFloat = 1.0f;
		ATK_XXX->AddEvent(s);
	}
	Layer->AddNode(ATK_XXX);

	ATK_XXXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXX")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 10 / 35.0f;
		e1.NormalizedTime = 15 / 35.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_LIGHT;
		e1.ContextInt = IntContext::KT_END;
		ATK_XXXX->AddEvent(e0);
		ATK_XXXX->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 10 / 35.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_HEAVY01;
		s.ContextFloat = 1.0f;
		ATK_XXXX->AddEvent(s);
	}
	Layer->AddNode(ATK_XXXX);

	ATK_XXXXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXX")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 18 / 53.0f;
		e1.NormalizedTime = 23 / 53.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_HEAVY;
		e1.ContextInt = IntContext::KT_END;
		ATK_XXXXX->AddEvent(e0);
		ATK_XXXXX->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 20 / 53.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_HEAVY02;
		s.ContextFloat = 1.0f;
		ATK_XXXXX->AddEvent(s);
	}
	Layer->AddNode(ATK_XXXXX);

	ATK_AIR_X = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_X")), NOLOOP);
	{
		AnimationEventDesc e0, e1, e2, e3;
		e0.NormalizedTime = 4 / 40.0f;
		e1.NormalizedTime = 8 / 40.0f;
		e2.NormalizedTime = 12 / 40.0f;
		e3.NormalizedTime = 18 / 40.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_LIGHT;
		e1.ContextInt = IntContext::KT_END;
		e2.ContextInt = IntContext::KT_START;
		e2.ContextUInt = UIntContext::ATK_LIGHT;
		e3.ContextInt = IntContext::KT_END;
		ATK_AIR_X->AddEvent(e0);
		ATK_AIR_X->AddEvent(e1);
		ATK_AIR_X->AddEvent(e2);
		ATK_AIR_X->AddEvent(e3);

		AnimationEventDesc s0, s1;
		s0.NormalizedTime = 4 / 40.0f;
		s1.NormalizedTime = 13 / 40.0f;
		s0.ContextByte = ByteContext::PLAY_SOUND;
		s1.ContextByte = ByteContext::PLAY_SOUND;
		s0.ContextTStr = SOUND_KATANA_SWING_LIGHT01;
		s1.ContextTStr = SOUND_KATANA_SWING_LIGHT02;
		s0.ContextFloat = 1.0f;
		s1.ContextFloat = 1.0f;
		ATK_AIR_X->AddEvent(s0);
		ATK_AIR_X->AddEvent(s1);
	}
	ATK_AIR_X->speed = 1.2f;
	Layer->AddNode(ATK_AIR_X);

	ATK_AIR_XX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_XX")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 11 / 40.0f;
		e1.NormalizedTime = 20 / 40.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_LIGHT;
		e1.ContextInt = IntContext::KT_END;
		ATK_AIR_XX->AddEvent(e0);
		ATK_AIR_XX->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 13 / 40.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_LIGHT03;
		s.ContextFloat = 1.0f;
		ATK_AIR_XX->AddEvent(s);
	}
	ATK_AIR_XX->speed = 1.2f;
	Layer->AddNode(ATK_AIR_XX);

	ATK_AIR_XXX = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_XXX")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 8 / 34.0f;
		e1.NormalizedTime = 14 / 34.0f;
		e0.ContextInt = IntContext::FOOT_START;
		e0.ContextUInt = UIntContext::ATK_LIGHT;
		e1.ContextInt = IntContext::FOOT_END;
		ATK_AIR_XXX->AddEvent(e0);
		ATK_AIR_XXX->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 10 / 34.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_MELEE_SWING_01;
		s.ContextFloat = 1.0f;
		ATK_AIR_XXX->AddEvent(s);
	}
	ATK_AIR_XXX->speed = 1.2f;
	Layer->AddNode(ATK_AIR_XXX);

	ATK_Y = AnimatorSingleNode::Create(GetClip(TEXT("ATK_Y")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 8 / 60.0f;
		e1.NormalizedTime = 14 / 60.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_HEAVY;
		e1.ContextInt = IntContext::KT_END;
		ATK_Y->AddEvent(e0);
		ATK_Y->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 9 / 60.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_HEAVY01;
		s.ContextFloat = 1.0f;
		ATK_Y->AddEvent(s);
	}
	Layer->AddNode(ATK_Y);

	ATK_YY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_YY")), NOLOOP);
	{
		AnimationEventDesc e0, e1, e2, e3;
		e0.NormalizedTime = 5 / 72.0f;
		e1.NormalizedTime = 12 / 72.0f;
		e2.NormalizedTime = 15 / 72.0f;
		e3.NormalizedTime = 20 / 72.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_HEAVY;
		e1.ContextInt = IntContext::KT_END;
		e2.ContextInt = IntContext::KT_START;
		e2.ContextUInt = UIntContext::ATK_HEAVY;
		e3.ContextInt = IntContext::KT_END;
		ATK_YY->AddEvent(e0);
		ATK_YY->AddEvent(e1);
		ATK_YY->AddEvent(e2);
		ATK_YY->AddEvent(e3);

		AnimationEventDesc s0, s1;
		s0.NormalizedTime = 6 / 72.0f;
		s1.NormalizedTime = 16 / 72.0f;
		s0.ContextByte = ByteContext::PLAY_SOUND;
		s1.ContextByte = ByteContext::PLAY_SOUND;
		s0.ContextTStr = SOUND_KATANA_SWING_HEAVY02;
		s1.ContextTStr = SOUND_KATANA_SWING_HEAVY03;
		s0.ContextFloat = 1.0f;
		s1.ContextFloat = 1.0f;
		ATK_YY->AddEvent(s0);
		ATK_YY->AddEvent(s1);
	}
	Layer->AddNode(ATK_YY);

	ATK_YYY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_YYY")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 6 / 49.0f;
		e1.NormalizedTime = 13 / 49.0f;
		e0.ContextInt = IntContext::KT_STING_START;
		e0.ContextUInt = UIntContext::ATK_BLOW;
		e1.ContextInt = IntContext::KT_STING_END;
		ATK_YYY->AddEvent(e0);
		ATK_YYY->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 6 / 60.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_HEAVY04;
		s.ContextFloat = 1.0f;
		ATK_YYY->AddEvent(s);
	}
	Layer->AddNode(ATK_YYY);

	ATK_XY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XY")), NOLOOP);
	{
		AnimationEventDesc e0, e1, e2;
		e0.NormalizedTime = 5 / 30.0f;
		e1.NormalizedTime = 6 / 30.0f;
		e2.NormalizedTime = 14 / 30.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_BLOWUP;
		e1.ContextByte = ByteContext::JUMP;
		e1.ContextFloat = BLOWUP_SPEED;
		e2.ContextInt = IntContext::KT_END;
		ATK_XY->AddEvent(e0);
		ATK_XY->AddEvent(e1);
		ATK_XY->AddEvent(e2);

		AnimationEventDesc s;
		s.NormalizedTime = 6 / 30.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_LIGHT03;
		s.ContextFloat = 1.0f;
		ATK_XY->AddEvent(s);

		AnimationEventDesc jumpSound;
		jumpSound.NormalizedTime = 6 / 30.0f;
		jumpSound.ContextByte = ByteContext::PLAY_SOUND;
		jumpSound.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_03;
		jumpSound.ContextFloat = 1.0f;
		ATK_XY->AddEvent(jumpSound);
	}
	Layer->AddNode(ATK_XY);

	ATK_XXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXY")), NOLOOP);
	{
		AnimationEventDesc e0, e1, e2, e3;
		e0.NormalizedTime = 8 / 68.0f;
		e1.NormalizedTime = 18 / 68.0f;
		e2.NormalizedTime = 26 / 68.0f;
		e3.NormalizedTime = 32 / 68.0f;
		e0.ContextInt = IntContext::FOOT_START;
		e0.ContextUInt = UIntContext::ATK_HEAVY;
		e1.ContextInt = IntContext::FOOT_START;
		e1.ContextUInt = UIntContext::ATK_HEAVY;
		e2.ContextInt = IntContext::FOOT_START;
		e2.ContextUInt = UIntContext::ATK_HEAVY;
		e3.ContextInt = IntContext::FOOT_END;
		ATK_XXXY->AddEvent(e0);
		ATK_XXXY->AddEvent(e1);
		ATK_XXXY->AddEvent(e2);
		ATK_XXXY->AddEvent(e3);

		AnimationEventDesc s0, s1, s2;
		s0.NormalizedTime = 8 / 68.0f;
		s1.NormalizedTime = 16 / 68.0f;
		s2.NormalizedTime = 24 / 68.0f;
		s0.ContextByte = ByteContext::PLAY_SOUND;
		s1.ContextByte = ByteContext::PLAY_SOUND;
		s2.ContextByte = ByteContext::PLAY_SOUND;
		s0.ContextTStr = SOUND_MELEE_SWING_01;
		s1.ContextTStr = SOUND_MELEE_SWING_01;
		s2.ContextTStr = SOUND_MELEE_SWING_01;
		s0.ContextFloat = 1.0f;
		s1.ContextFloat = 1.0f;
		s2.ContextFloat = 1.0f;
		ATK_XXXY->AddEvent(s0);
		ATK_XXXY->AddEvent(s1);
		ATK_XXXY->AddEvent(s2);
	}
	Layer->AddNode(ATK_XXXY);

	ATK_XXXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXY")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 16 / 56.0f;
		e1.NormalizedTime = 21 / 56.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_HEAVY;
		e1.ContextInt = IntContext::KT_END;
		ATK_XXXXY->AddEvent(e0);
		ATK_XXXXY->AddEvent(e1);

		AnimationEventDesc s;
		s.NormalizedTime = 17 / 56.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_HEAVY02;
		s.ContextFloat = 1.0f;
		ATK_XXXXY->AddEvent(s);
	}
	Layer->AddNode(ATK_XXXXY);

	ATK_XXXXXY = AnimatorSingleNode::Create(GetClip(TEXT("ATK_XXXXXY")), NOLOOP);
	{
		AnimationEventDesc e0, e1, e2, e3;
		e0.NormalizedTime = 11 / 52.0f;
		e1.NormalizedTime = 19 / 52.0f;
		e2.NormalizedTime = 29 / 52.0f;
		e3.NormalizedTime = 35 / 52.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_LIGHT;
		e1.ContextInt = IntContext::KT_START;
		e1.ContextUInt = UIntContext::ATK_LIGHT;
		e2.ContextInt = IntContext::KT_START;
		e2.ContextUInt = UIntContext::ATK_LIGHT;
		e3.ContextInt = IntContext::KT_END;
		ATK_XXXXXY->AddEvent(e0);
		ATK_XXXXXY->AddEvent(e1);
		ATK_XXXXXY->AddEvent(e2);
		ATK_XXXXXY->AddEvent(e3);

		AnimationEventDesc s0, s1, s2;
		s0.NormalizedTime = 11 / 52.0f;
		s1.NormalizedTime = 19 / 52.0f;
		s2.NormalizedTime = 27 / 52.0f;
		s0.ContextByte = ByteContext::PLAY_SOUND;
		s1.ContextByte = ByteContext::PLAY_SOUND;
		s2.ContextByte = ByteContext::PLAY_SOUND;
		s0.ContextTStr = SOUND_KATANA_SWING_HEAVY01;
		s1.ContextTStr = SOUND_KATANA_SWING_HEAVY01;
		s2.ContextTStr = SOUND_KATANA_SWING_HEAVY01;
		s0.ContextFloat = 1.0f;
		s1.ContextFloat = 1.0f;
		s2.ContextFloat = 1.0f;
		ATK_XXXXXY->AddEvent(s0);
		ATK_XXXXXY->AddEvent(s1);
		ATK_XXXXXY->AddEvent(s2);
	}
	Layer->AddNode(ATK_XXXXXY);

	ATK_AIR_Y_START = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_START")), NOLOOP);
	{
		AnimationEventDesc e0;
		e0.NormalizedTime = 11 / 13.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_BLOWDOWN;
		ATK_AIR_Y_START->AddEvent(e0);

		AnimationEventDesc s;
		s.NormalizedTime = 11 / 13.0f;
		s.ContextByte = ByteContext::PLAY_SOUND;
		s.ContextTStr = SOUND_KATANA_SWING_HEAVY03;
		s.ContextFloat = 1.0f;
		ATK_AIR_Y_START->AddEvent(s);
	}
	ATK_AIR_Y_START->speed = 1.2f;
	Layer->AddNode(ATK_AIR_Y_START);

	ATK_AIR_Y_LOOP = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_LOOP")), LOOP);
	{
		AnimationEventDesc e0;
		e0.NormalizedTime = 0.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_BLOWDOWN;
		ATK_AIR_Y_LOOP->AddEvent(e0);
	}
	Layer->AddNode(ATK_AIR_Y_LOOP);

	ATK_AIR_Y_END = AnimatorSingleNode::Create(GetClip(TEXT("ATK_AIR_Y_END")), NOLOOP);
	{
		AnimationEventDesc e0, e1;
		e0.NormalizedTime = 0.0f;
		e1.NormalizedTime = 5 / 35.0f;
		e0.ContextInt = IntContext::KT_START;
		e0.ContextUInt = UIntContext::ATK_BLOW;
		e1.ContextInt = IntContext::KT_END;
		ATK_AIR_Y_END->AddEvent(e0);
		ATK_AIR_Y_END->AddEvent(e1);

		AnimationEventDesc jumpSound;
		jumpSound.NormalizedTime = 0.0f;
		jumpSound.ContextByte = ByteContext::PLAY_SOUND;
		jumpSound.ContextTStr = SOUND_BEHAVIOR_FOOTSTEP_03;
		jumpSound.ContextFloat = 1.0f;
		ATK_AIR_Y_END->AddEvent(jumpSound);
	}
	Layer->AddNode(ATK_AIR_Y_END);
}

void PlayerAnimator::SetupTransitions()
{
	// BH_IDLE -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_IDLE, BH_RUN_BH_DASH, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None, true);
	}

	// BH_LAND -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_LAND, BH_RUN_BH_DASH, values, 0.15f, 0.25f, 0.0f, AnimatorTransition::Interrupt::None, true);
	}

	// BH_ROLL -> BH_RUN_BH_DASH
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_ROLL, BH_RUN_BH_DASH, values, BH_ROLL_EXITTIME, 0.3f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_RUN_BH_DASH -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::LESS_EQAUL));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_RUN_BH_DASH, EXIT, values, 0.0f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> BH_JUMP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(JumpTProperty));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(ANY, BH_JUMP, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
		transition->SetCallback(this);
	}

	// ANY -> BH_AIR_JUMP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(AirJumpTProperty));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, false, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ANY, BH_AIR_JUMP, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> BH_FALL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, false, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(ANY, BH_FALL, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None, true);
		transition->SetCallback(this);
	}

	// BH_JUMP -> BH_LAND
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_JUMP, BH_LAND, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_AIR_JUMP -> BH_LAND
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_AIR_JUMP, BH_LAND, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_FALL -> BH_LAND
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(BH_FALL, BH_LAND, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// BH_LAND -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(BH_LAND, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> BH_ROLL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(RollTProperty));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(ANY, BH_ROLL, values, 0.0f, 0.05f, 0.0f, AnimatorTransition::Interrupt::None, true);
		transition->SetCallback(this);
	}

	// BH_ROLL -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(BH_ROLL, EXIT, values, 0.8f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> GAD_START
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(GuardBProperty, true, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(ANY, GAD_START, values, 0.0f, 0.2f, 0.0f, AnimatorTransition::Interrupt::None, true);
		transition->SetCallback(this);
	}

	// GAD_START -> GAD_LOOP
	{
		vector<AnimatorTransition::PropertyValue> values;
		auto transition = Layer->AddTransition(GAD_START, GAD_LOOP, values, 0.5f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
		transition->SetCallback(this);
	}

	// GAD_HIT -> GAD_LOOP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(GuardBProperty, true, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(GAD_HIT, GAD_LOOP, values, 0.6f, 0.1f);
		transition->SetCallback(this);
	}

	// GAD_LOOP -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(GuardBProperty, false, AnimatorTransition::Compare::EQUAL));
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

	// ANY -> GAD_BREAK
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(GuardBreakTProperty));
		Layer->AddTransition(ANY, GAD_BREAK, values, 0.0f, 0.1f);
	}

	// GAD_BREAK -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(GAD_BREAK, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
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
		Layer->AddTransition(DMG_LIGHT, EXIT, values, 0.5f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
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
		Layer->AddTransition(DMG_HEAVY, EXIT, values, 0.8f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
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
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(DMG_BLOW_LOOP, DMG_BLOW_END, values, 0.0f, 0.1f);
	}

	// DMG_BLOW_END -> DMG_BLOW_GETUP
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(DMG_BLOW_END, DMG_BLOW_GETUP, values, 1.0f, 0.0f);
	}

	// DMG_BLOW_GETUP -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(DMG_BLOW_GETUP, EXIT, values, 0.7f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> ATK_X
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
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
	AddCommonTransitions(ATK_X, 0.6f);

	// ATK_X -> ATK_XX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ATK_X, ATK_XX, values, 0.1f, 0.05f, 0.1f);
	}

	// ATK_XX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XX, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_XX, 0.6f);

	// ATK_XX -> ATK_XXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ATK_XX, ATK_XXX, values, 0.35f, 0.1f);
	}

	// ATK_XXX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXX, EXIT, values, 0.5f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_XXX, 0.3f);

	// ATK_XXX -> ATK_XXXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ATK_XXX, ATK_XXXX, values, 0.3f, 0.2f);
	}

	// ATK_XXXX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXX, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_XXXX, 0.6f);

	// ATK_XXXX -> ATK_XXXXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		Layer->AddTransition(ATK_XXXX, ATK_XXXXX, values, 0.3f, 0.1f);
	}

	// ATK_XXXXX -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXXX, EXIT, values, 0.7f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_XXXXX, 0.7f);

	// ANY -> ATK_AIR_X
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, false, AnimatorTransition::Compare::EQUAL));
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
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, false, AnimatorTransition::Compare::EQUAL));
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
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, false, AnimatorTransition::Compare::EQUAL));
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
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, false, AnimatorTransition::Compare::EQUAL));
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
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		Layer->AddTransition(ATK_AIR_Y_LOOP, ATK_AIR_Y_END, values, 0.0f, 0.1f);
	}

	// ATK_AIR_Y_END -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_AIR_Y_END, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}

	// ANY -> ATK_Y
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
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
	AddCommonTransitions(ATK_Y, 0.5f);

	// ATK_Y -> ATK_YY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_Y, ATK_YY, values, 0.4f, 0.03f);
	}

	// ATK_YY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_YY, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_YY, 0.5f);

	// ATK_YY -> ATK_YYY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_YY, ATK_YYY, values, 0.5f, 0.1f);
	}

	// ATK_YYY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_YYY, EXIT, values, 0.6f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_YYY, 0.5f);

	// ATK_YYY -> ATK_XXXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
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
		Layer->AddTransition(ATK_XXXY, EXIT, values, 0.85f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_XXXY, 0.7f);

	// ATK_XXXX -> ATK_XXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_XXXX, ATK_XXXXY, values, 0.4f, 0.1f);
	}

	// ATK_XXXXY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXXY, EXIT, values, 0.9f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_XXXXY, 0.7f);

	// ATK_XXXXX -> ATK_XXXXXY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		Layer->AddTransition(ATK_XXXXX, ATK_XXXXXY, values, 0.4f, 0.2f);
	}

	// ATK_XXXXXY -> EXIT
	{
		vector<AnimatorTransition::PropertyValue> values;
		Layer->AddTransition(ATK_XXXXXY, EXIT, values, 0.9f, 0.1f, 0.0f, AnimatorTransition::Interrupt::None);
	}
	AddCommonTransitions(ATK_XXXXXY, 0.7f);

	// BH_RUN_BH_DASH -> ATK_XXX
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(LightAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::GREATER));
		Layer->AddTransition(BH_RUN_BH_DASH, ATK_XXX, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current);
	}

	// BH_RUN_BH_DASH -> ATK_XY
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue::Trigger(HeavyAttackTProperty));
		values.push_back(AnimatorTransition::PropertyValue(MoveStateFProperty, 0.0f, AnimatorTransition::Compare::GREATER));
		values.push_back(AnimatorTransition::PropertyValue(LastDashStateFProperty, 0.5f, AnimatorTransition::Compare::GREATER));
		Layer->AddTransition(BH_RUN_BH_DASH, ATK_XY, values, 0.0f, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current);
	}
}

void PlayerAnimator::AddCommonTransitions(AnimatorNode* startNode, float exitTime)
{
	// startNode -> BH_ROLL
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue::Trigger(RollTProperty));
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(startNode, BH_ROLL, values, exitTime, 0.1f, 0.0f, AnimatorTransition::Interrupt::Current, true);
		transition->SetCallback(this);
	}

	// startNode -> GAD_START
	{
		vector<AnimatorTransition::PropertyValue> values;
		values.push_back(AnimatorTransition::PropertyValue(GroundStateBProperty, true, AnimatorTransition::Compare::EQUAL));
		values.push_back(AnimatorTransition::PropertyValue(GuardBProperty, true, AnimatorTransition::Compare::EQUAL));
		auto transition = Layer->AddTransition(startNode, GAD_START, values, exitTime, 0.2f, 0.0f, AnimatorTransition::Interrupt::Current, true);
		transition->SetCallback(this);
	}
}

bool PlayerAnimator::Transferable(
	Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition,
	AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const
{
	if (transition->nextNode == BH_FALL)
	{
		if (layer->IsPlaying(BH_JUMP) && BH_JUMP->normalizedTime < 0.8f)
			return false;
		if (layer->IsPlaying(BH_AIR_JUMP) && BH_AIR_JUMP->normalizedTime < 0.8f)
			return false;
		if (layer->IsContains(TEXT("ATK")))
			return false;
	}

	// ANY -> BH_ROLL ����
	if (transition->nextNode == BH_ROLL && transition->startNode == ANY)
	{
		if (layer->IsContains(TEXT("ATK")))
			return false;
	}
	
	if (transition->nextNode == BH_JUMP)
	{
		if (layer->IsPlaying(BH_ROLL) && BH_ROLL->normalizedTime < BH_ROLL_EXITTIME)
			return false;
	}

	// ANY -> GAD_START ����
	if (transition->nextNode == GAD_START && transition->startNode == ANY)
	{
		if (layer->IsPlaying(BH_ROLL) && BH_ROLL->normalizedTime < BH_ROLL_EXITTIME)
			return false;
		if (layer->IsPlaying(GAD_LOOP))
			return false;
		if (layer->IsPlaying(GAD_HIT))
			return false;
		if (layer->IsContains(TEXT("ATK")))
			return false;
	}

	if (transition->nextNode == ATK_X ||
		transition->nextNode == ATK_Y ||
		transition->nextNode == ATK_AIR_X)
	{
		if (layer->IsContains(TEXT("ATK")))
			return false;
		if (layer->IsPlaying(BH_ROLL) && BH_ROLL->normalizedTime < BH_ROLL_EXITTIME)
			return false;
	}

	if (transition->nextNode == ATK_AIR_Y_START)
	{
		if (layer->IsContains(TEXT("ATK_AIR_Y")) ||
			layer->IsPlaying(ATK_XY))
			return false;
	}

	if (transition->nextNode == BH_ROLL ||
		transition->nextNode == GAD_START ||
		transition->nextNode == GAD_LOOP ||
		transition->nextNode == BH_JUMP ||
		transition->nextNode == BH_AIR_JUMP || 
		transition->nextNode == BH_FALL)
	{
		if (layer->IsContains(TEXT("DMG")))
			return false;
		if (layer->IsPlaying(GAD_BREAK))
			return false;
	}

	return true;
}
