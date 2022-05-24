#pragma once

class EnemyBeetleDroneAnimator : public Animator, public AnimatorTransitionCallback
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

	bool IsPlayingAirAction() const;
	bool IsPlayingDamage() const;
	bool IsPlayingDamageDuringLookPlayerAnimation() const;
	bool IsPlayingGuardableAnimation() const;
	bool IsPlayingAttack() const;

public:

	ResourceRef<Mesh> Mesh;
	AnimatorLayer* Layer;

public:

	AnimatorProperty* MoveBProperty;
	AnimatorProperty* JumpTProperty;
	AnimatorProperty* ATKTProperty;

	// 0(ATK3A) 1(ATK3B)

	AnimatorProperty* ATK3TypeIProperty;
	AnimatorProperty* GuardBProperty;
	AnimatorProperty* GuardHitTProperty;
	AnimatorProperty* GuardBreakTProperty;

	// (Damaged)
	AnimatorProperty* DamageTProperty;

	// 0(Forward) 1(Back)
	AnimatorProperty* DamageDirectionFProperty;

	// 0(Normal) 1(Large) 2(Blow) 3(Blowup)
	AnimatorProperty* DamageTypeIProperty;

	// 0(No Grunded) 1(Grounded)
	AnimatorProperty* HasGroundBProperty;

	AnimatorProperty* HPFProperty;

public:

	AnimatorNode* BH_STD_IDLE; // Loop
	AnimatorNode* BH_STD_WALK; // Loop
	AnimatorNode* BH_STD_JUMP;
	AnimatorNode* BH_STD_FALL; // Loop
	AnimatorNode* BH_STD_LAND;

	AnimatorNode* STD_ATK1;
	AnimatorNode* STD_ATK2;
	AnimatorNode* STD_ATK3A;
	AnimatorNode* STD_ATK3B;

	AnimatorNode* DMG_STD_AIR;
	AnimatorNode* DMG_STD_LIGHT; // Blend
	AnimatorNode* DMG_STD_HEAVY; // Blend
	AnimatorNode* DMG_STD_BLOW; // Blend
	AnimatorNode* DMG_STD_GETUP; // Blend

	AnimatorNode* DMG_STD_BLOWUP_START;
	AnimatorNode* DMG_STD_BLOWUP_FALL; // Loop
	AnimatorNode* DMG_STD_BLOWUP_END;

	AnimatorNode* GAD_STD_LOOP;
	AnimatorNode* GAD_STD_HIT;
	AnimatorNode* GAD_STD_BREAK;

	AnimatorNode* ETC_APPEAR;

private:

	virtual bool Transferable(
		Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition, 
		AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const;
};

