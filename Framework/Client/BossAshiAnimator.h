#pragma once

class BossAshiAnimator : public Animator
{
public:

	enum IntContext : int
	{
	};

	enum UIntContext : uint
	{
		ATK_LIGHT = 0b1,
		ATK_HEAVY = 0b10,
		ATK_BLOW = 0b100,
		ATK_BLOWUP = 0b1000,
		ATK_BLOWDOWN = 0b10000,
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
	AnimatorProperty* WalkDirectionFProperty; // 0: F, 1: B, 2: L, 3: R

	AnimatorProperty* AdditiveDamageTProperty;

public:

	AnimatorNode* ADDITIVE_EMPTY;
	AnimatorNode* ADDITIVE_DMG_ADDITIVE; // DMG_ADDITIVE

	AnimatorNode* BH_IDLE; // Loop
	AnimatorNode* BH_WALK_ST; // Blend
	AnimatorNode* BH_WALK_LP; // Blend, Loop
	AnimatorNode* BH_WALK_ED; // Blend
	AnimatorNode* BH_RUN_ST;  
	AnimatorNode* BH_RUN_LP; // Loop
	AnimatorNode* BH_RUN_ED; 

	AnimatorNode* DMG_DIE_ST;
	AnimatorNode* DMG_DIE_ED;
};

