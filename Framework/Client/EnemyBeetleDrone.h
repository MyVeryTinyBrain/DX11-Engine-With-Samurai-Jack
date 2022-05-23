#pragma once

#include "Enemy.h"

class EnemyBeetleDroneAnimator;

class EnemyBeetleDrone : public Enemy
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

private:

	void SetupCharacterRenderers();
	void SetupAnimator();

private:

	void UpdateCCT();

private:

	void OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing);
	void OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev);
	void OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc);

public:

	virtual DamageResult Damage(const DamageDesc& desc) override;

private:

	// Character Renderers

	GameObject* m_goCharacterRender;
	SkinnedMeshRenderer* m_characterRenderer;

	// Animator

	EnemyBeetleDroneAnimator* m_animator;
};

