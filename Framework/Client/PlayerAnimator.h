#pragma once

class PlayerAnimator : public Animator, public AnimatorTransitionCallback
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

	bool IsPlayingJump() const;
	bool IsPlayingGuard() const;
	bool IsPlayingAttack() const;
	bool IsPlayingDamage() const;
	bool IsPlayingAirAction() const;

	bool IsPlayingGuardableAnimation(bool withoutGuardHit = false) const;
	bool IsPlayingNonAttackableAnimation() const;
	bool IsPlayingNonJumpableAnimation() const;
	bool IsPlayingNonMovableAnimation() const;
	bool IsPlayingNonRollableAnimation() const;
	bool IsPlayingNonGuardableAnimation() const;

public:

	ResourceRef<Mesh>	Mesh;
	AnimatorLayer*		Layer;

public:

	// 0(Stop) ~ 1(Move)
	AnimatorProperty* MoveStateFProperty;

	// 0(Run) 1(Dash)
	AnimatorProperty* DashStateFProperty;

	// 0(Run) 1(Dash)
	AnimatorProperty* LastDashStateFProperty;

	// (Damaged)
	AnimatorProperty* DamageTProperty;

	// 0(Forward) 1(Back)
	AnimatorProperty* DamageDirectionFProperty;

	// 0(Normal) 1(Large) 2(Blow)
	AnimatorProperty* DamageTypeIProperty;

	// (Roll)
	AnimatorProperty* RollTProperty;

	// (Light Attack)
	AnimatorProperty* LightAttackTProperty;

	// (Heavy Attack)
	AnimatorProperty* HeavyAttackTProperty;

	// (Guard)
	AnimatorProperty* GuardStateBProperty;

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
	AnimatorNode* BH_RUN_BH_DASH; // Blend, Loop
	AnimatorNode* BH_JUMP;
	AnimatorNode* BH_AIR_JUMP;
	AnimatorNode* BH_FALL; // Loop
	AnimatorNode* BH_LAND;
	AnimatorNode* BH_ROLL;

	AnimatorNode* GAD_START; // Blend
	AnimatorNode* GAD_LOOP; // Loop
	AnimatorNode* GAD_HIT;
	AnimatorNode* GAD_BREAK;

	AnimatorNode* DMG_LIGHT; // Blend
	AnimatorNode* DMG_HEAVY; // Blend
	AnimatorNode* DMG_BLOW_START;
	AnimatorNode* DMG_BLOW_LOOP; // Loop
	AnimatorNode* DMG_BLOW_END;
	AnimatorNode* DMG_BLOW_GETUP;

	AnimatorNode* ATK_X;
	AnimatorNode* ATK_XX;
	AnimatorNode* ATK_XXX;
	AnimatorNode* ATK_XXXX;
	AnimatorNode* ATK_XXXXX;

	AnimatorNode* ATK_AIR_X;
	AnimatorNode* ATK_AIR_XX;
	AnimatorNode* ATK_AIR_XXX;

	AnimatorNode* ATK_Y;
	AnimatorNode* ATK_YY;
	AnimatorNode* ATK_YYY;
	// YYY -> XXXXXY

	AnimatorNode* ATK_XY;
	AnimatorNode* ATK_XXXY;
	AnimatorNode* ATK_XXXXY;
	AnimatorNode* ATK_XXXXXY;

	AnimatorNode* ATK_AIR_Y_START;
	AnimatorNode* ATK_AIR_Y_LOOP; // Loop
	AnimatorNode* ATK_AIR_Y_END;

private:

	virtual bool Transferable(
		Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition,
		AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const;
};

