#pragma once

class PlayerAnimator : public Animator
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

	ResourceRef<Mesh>	Mesh;
	AnimatorLayer*		Layer;

public:

	// 0(Stop) ~ 1(Move)
	AnimatorProperty* MoveStateFProperty;

	// 0(Run) 1(Dash)
	AnimatorProperty* DashStateFProperty;

	// (Damaged)
	AnimatorProperty* DamageTProperty;

	// 0(Back) 1(Back Large) 2(Blow) 3(Forward) 4(Forward Large)
	AnimatorProperty* DamageTypeIProperty;

	// (Dead)
	AnimatorProperty* DeadBProperty;

	// (Roll)
	AnimatorProperty* RollTProperty;

	// (Light Attack)
	AnimatorProperty* LightAttackTProperty;

	// (Heavy Attack)
	AnimatorProperty* HeavyAttackTProperty;

	// 0(No Guard) 1(Guard)
	AnimatorProperty* GuardBProperty;

	// (Guard Break)
	AnimatorProperty* GuardBreakTProperty;

	// (Guard Hit)
	AnimatorProperty* GuardHitTProperty;

	// (Jump)
	AnimatorProperty* JumpTProperty;

	// (AirJump)
	AnimatorProperty* AirJumpTProperty;

	// 0(No Grunded) 1(Grounded)
	AnimatorProperty* HasGroundBProperty;

public:

	AnimatorNode* BH_IDLE;
	AnimatorNode* BH_RUN_BH_DASH; // Loop
	AnimatorNode* BH_JUMP;
	AnimatorNode* BH_AIR_JUMP;
	AnimatorNode* BH_FALL; // Loop
	AnimatorNode* BH_LAND;
	AnimatorNode* BH_ROLL;

};

