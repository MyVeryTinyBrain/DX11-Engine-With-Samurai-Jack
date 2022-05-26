#pragma once

class BossAncientKingAnimator : public Animator
{
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

public:

	AnimatorNode* ADDITIVE_EMPTY; 
	AnimatorNode* ADDITIVE_DMG_ADDITIVE; // DMG_ADDITIVE

	AnimatorNode* BH_IDLE; // Loop
	AnimatorNode* BH_TURN; // Loop
	AnimatorNode* BH_WLK_ST;
	AnimatorNode* BH_WLK_LP; // Loop
	AnimatorNode* BH_WLK_ED;

	AnimatorNode* DMG_GROGY_ST;
	AnimatorNode* DMG_GROGY_LP; // Loop
	AnimatorNode* DMG_GROGY_ED;
	AnimatorNode* DMG_GROGY_HIT;
	AnimatorNode* DMG_DIE;

	AnimatorNode* ATK_TURN;
	AnimatorNode* ATK_SWING_L; // SWING0
	AnimatorNode* ATK_SWING_R; // SWING1
	AnimatorNode* ATK_SWING_V; // VSWING0
	AnimatorNode* ATK_STOMP;
	AnimatorNode* ATK_STEPON_L; // STEPON0
	AnimatorNode* ATK_STEPON_R; // STEPON0
	AnimatorNode* ATK_RUSH_ST; // RUSH0
	AnimatorNode* ATK_RUSH_LP; // RUSH1, Loop
	AnimatorNode* ATK_RUSH_ED; // RUSH2
	AnimatorNode* ATK_JUMP;
	AnimatorNode* ATK_ELECTRIC;
	AnimatorNode* ATK_DOWNSTRIKE_L; // DOWNSTRIKE0
	AnimatorNode* ATK_DOWNSTRIKE_R; // DOWNSTRICT1
	AnimatorNode* ATK_BEAM;

	AnimatorNode* ETC_RAGE;
};

