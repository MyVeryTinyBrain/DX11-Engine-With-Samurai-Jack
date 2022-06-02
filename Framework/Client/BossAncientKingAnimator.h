#pragma once

class BossAncientKingAnimator : public Animator
{
public:

	enum IntContext : int
	{
		HAMMER_START = 0b1,
		HAMMER_END = 0b10,
		LF_START = 0b100,
		LF_END = 0b1000,
		RF_START = 0b10000,
		RF_END = 0b100000,
		HAMMER_TIP_START = 0b1000000,
		HAMMER_TIP_END = 0b10000000,
	};

	enum UIntContext : uint
	{
		ATK_LIGHT = 0b1,
		ATK_HEAVY = 0b10,
		ATK_BLOW = 0b100,
		ATK_BLOWUP = 0b1000, 
		ATK_BLOWDOWN = 0b10000,
		ATK_SHOOT_BEAM = 0b100000,
		ATK_ELECTRIC_START = 0b1000000,
		ATK_ELECTRIC_END = 0b10000000,
		ATK_GADABLE = 0b100000000,
		SET_AIM_ON_HEAD = 0b1000000000,
		SET_AIM_ON_RH = 0b10000000000,
		MANUALLOOK_START = 0b100000000000,
		MANUALLOOK_END = 0b1000000000000, 
		HAMMER_GROUND_IMPACT = 0b10000000000000,
		LF_GROUND_IMPACT = 0b100000000000000,
		RF_GROUND_IMPACT = 0b1000000000000000,
		HAMMER_DUST = 0b10000000000000000,
		LF_DUST = 0b100000000000000000,
		RF_DUST = 0b1000000000000000000,
	};

	enum ByteContext : byte
	{
		CAM_SHAKE = 0b1,
		LF_SHOCKWAVE = 0b10,
		RF_SHOCKWAVE = 0b100,
		HAMMER_SHOCKWAVE = 0b1000,
		BACKJUMP_END = 0b10000,
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

	//bool IsPlaying...() const;

public:

	ResourceRef<Mesh> Mesh;
	AnimatorLayer* Layer;
	AnimatorLayer* AdditiveLayer;

public:

	AnimatorProperty* AdditiveDamageTProperty;
	AnimatorProperty* TurnBProperty;
	AnimatorProperty* WalkBProperty;

	AnimatorProperty* KeepAttackBProperty;

	AnimatorProperty* ATK_TURN_TProperty;
	AnimatorProperty* ATK_SWING_H_TProperty;
	AnimatorProperty* ATK_SWING_V_TProperty;
	AnimatorProperty* ATK_STOMP_TProperty;
	AnimatorProperty* ATK_STEPON_TProperty; 
	AnimatorProperty* ATK_RUSH_TProperty;
	AnimatorProperty* ATK_JUMP_TProperty;
	AnimatorProperty* ATK_BACKJUMP_TProperty;
	AnimatorProperty* ATK_ELECTRIC_TProperty;
	AnimatorProperty* ATK_DOWNSTRIKE_TProperty; 
	AnimatorProperty* ATK_BEAM_TProperty;

	AnimatorProperty* GROGY_TProperty;
	AnimatorProperty* DIE_TProperty;
	AnimatorProperty* RAGE_TProperty;

public:

	AnimatorNode* ADDITIVE_EMPTY; 
	AnimatorNode* ADDITIVE_DMG_ADDITIVE; // DMG_ADDITIVE

	AnimatorNode* BH_IDLE; // Loop
	AnimatorNode* BH_TURN_ROTATE; // TURN0
	AnimatorNode* BH_TURN_WAIT; // TURN1
	AnimatorNode* BH_WLK_ST;
	AnimatorNode* BH_WLK_LP; // Loop
	AnimatorNode* BH_WLK_ED;

	AnimatorNode* DMG_GROGY_ST;
	AnimatorNode* DMG_GROGY_LP; // Loop
	AnimatorNode* DMG_GROGY_ED;
	AnimatorNode* DMG_GROGY_HIT;
	AnimatorNode* DMG_DIE;

	AnimatorNode* ATK_TURN; 
	AnimatorNode* ATK_SWING_L; // HSWING0
	AnimatorNode* ATK_SWING_R; // HSWING1 
	AnimatorNode* ATK_SWING_V; // VSWING0
	AnimatorNode* ATK_STOMP;
	AnimatorNode* ATK_STEPON_R; // STEPON0
	AnimatorNode* ATK_STEPON_L; // STEPON1 
	AnimatorNode* ATK_RUSH_ST; // RUSH0 
	AnimatorNode* ATK_RUSH_LP; // RUSH1, Loop 
	AnimatorNode* ATK_RUSH_ED; // RUSH2 
	AnimatorNode* ATK_JUMP;
	AnimatorNode* ATK_BACKJUMP;
	AnimatorNode* ATK_ELECTRIC;
	AnimatorNode* ATK_DOWNSTRIKE_L; // DOWNSTRIKE0 
	AnimatorNode* ATK_DOWNSTRIKE_R; // DOWNSTRICT1 
	AnimatorNode* ATK_BEAM;

	AnimatorNode* ETC_RAGE;
};

