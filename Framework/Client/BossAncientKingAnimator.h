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

	AnimatorNode* BH_IDLE; // Loop
};

