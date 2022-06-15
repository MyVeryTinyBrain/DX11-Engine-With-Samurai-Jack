#pragma once

class BossAshiAnimator : public Animator
{
public:

	enum IntContext : int
	{
		SWORD_START = 0b1,
		SWORD_END = 0b10,
		RIGHT_FOOT_START = 0b100,
		RIGHT_FOOT_END = 0b1000,
	};

	enum UIntContext : uint
	{
		ATK_LIGHT = 0b1,
		ATK_HEAVY = 0b10,
		ATK_BLOW = 0b100,
		ATK_BLOWUP = 0b1000,
		ATK_BLOWDOWN = 0b10000,
		ATK_GADABLE = 0b100000,

		START_MANUAL_LOOK = 0b1000000,
		END_MANUAL_LOOK = 0b10000000,
		ATK_COMBO = 0b100000000,
	};

	enum ByteContext : byte
	{
	};

private:

	virtual void Awake() override;

private:

	SubResourceRef<AnimationClip> GetClip(const tstring& name);

	void SetupMesh();
	void SetupLayer();
	void SetupProperties();
	void SetupNodes();
	void SetupTransitions();

public:

	ResourceRef<Mesh> Mesh;
	AnimatorLayer* Layer;
	AnimatorLayer* AdditiveLayer;

public:

	AnimatorProperty* MoveFProperty; // 0: NoMove, 1: Walk, 2: Run
	AnimatorProperty* WalkDirectionFProperty; // 0.0: F, 0.25: R, 0.5: B, 0.75: L, 1.0: F
	AnimatorProperty* TurnBProperty;
	AnimatorProperty* BackjumpTProperty;
	AnimatorProperty* DieTProperty;

	AnimatorProperty* ATK_DOUBLEHAND_SLASH_TProperty;
	AnimatorProperty* ATK_H_SLASH_TProperty;
	AnimatorProperty* ATK_SHOLDER_SLASH_TProperty;
	AnimatorProperty* ATK_SLASHUP_TProperty;

	AnimatorProperty* ATK_RAGE_TProperty;

	AnimatorProperty* ATK_DROPKICK_TProperty;
	AnimatorProperty* ATK_LEGSTEP_TProperty;
	AnimatorProperty* ATK_SPINKICK_TProperty;

	AnimatorProperty* ATK_LASER_TProperty;

	AnimatorProperty* ATK_RUSH_Start_TProperty;
	AnimatorProperty* ATK_RUSH_End_TProperty;

	AnimatorProperty* ATK_ANGRY_TProperty;

	AnimatorProperty* AdditiveDamageTProperty;

public:

	AnimatorNode* ADDITIVE_EMPTY;
	AnimatorNode* ADDITIVE_DMG_ADDITIVE; // DMG_ADDITIVE

	AnimatorNode* BH_IDLE; // Loop
	AnimatorNode* BH_WALK_LP; // Blend, Loop
	AnimatorNode* BH_RUN_LP; // Loop
	AnimatorNode* BH_TURN; // Loop
	AnimatorNode* BH_BACKJUMP;

	AnimatorNode* ATK_DOUBLEHAND_SLASH;
	AnimatorNode* ATK_H_SLASH;
	AnimatorNode* ATK_SHOLDER_SLASH;
	AnimatorNode* ATK_SLASHUP;

	AnimatorNode* ATK_DROPKICK;
	AnimatorNode* ATK_LEGSTEP;
	AnimatorNode* ATK_SPINKICK;

	AnimatorNode* ATK_LASER_ST;
	AnimatorNode* ATK_LASER_ED;

	AnimatorNode* ATK_RAGE;

	AnimatorNode* ATK_ANGRY;

	AnimatorNode* ATK_RUSH_ST_1;
	AnimatorNode* ATK_RUSH_ST_2;
	AnimatorNode* ATK_RUSH_LP;
	AnimatorNode* ATK_RUSH_ED;

	AnimatorNode* DMG_DIE_ST;
	AnimatorNode* DMG_DIE_ED;
};

