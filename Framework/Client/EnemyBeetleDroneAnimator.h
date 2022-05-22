#pragma once

class EnemyBeetleDroneAnimator : public Animator
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
	AnimatorLayer* Layer;

public:

public:

	AnimatorNode* BH_STD_IDLE;
};

