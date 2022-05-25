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

public:

	AnimatorNode* ADDITIVE_EMPTY; 
	AnimatorNode* ADDITIVE_DMG_ADDITIVE; // DMG_ADDITIVE

	AnimatorNode* BH_IDLE; // Loop
};

