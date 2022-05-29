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

	ResourceRef<Mesh>	Mesh;
	AnimatorLayer*		Layer;

public:

	AnimatorProperty* GroundStateBProperty;

	AnimatorProperty* MoveStateFProperty;
	AnimatorProperty* DashStateFProperty;
	AnimatorProperty* LastDashStateFProperty;

	AnimatorProperty* JumpTProperty;
	AnimatorProperty* AirJumpTProperty;

	AnimatorProperty* RollTProperty;

	AnimatorProperty* GuardBProperty;
	AnimatorProperty* GuardHitTProperty;
	AnimatorProperty* GuardBreakTProperty;

	AnimatorProperty* DamageTProperty;	
	AnimatorProperty* DamageDirectionFProperty; // 0(Forward) 1(Back)
	AnimatorProperty* DamageTypeIProperty; // 0(Normal) 1(Large) 2(Blow)

	AnimatorProperty* LightAttackTProperty;
	AnimatorProperty* HeavyAttackTProperty;

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

	void AddCommonTransitions(AnimatorNode* startNode, float exitTime);

private:

	virtual bool Transferable(
		Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition,
		AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const;
};

