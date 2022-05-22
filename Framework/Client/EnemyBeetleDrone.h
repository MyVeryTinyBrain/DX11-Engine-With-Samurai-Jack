#pragma once

#include "Enemy.h"

class EnemyBeetleDroneAnimator;

class EnemyBeetleDrone : public Enemy
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	void SetupCharacterRenderers();
	void SetupAnimator();

private:

	// Character Renderers

	GameObject* m_goCharacterRender;
	SkinnedMeshRenderer* m_characterRenderer;

	// Animator

	EnemyBeetleDroneAnimator* m_animator;
};

