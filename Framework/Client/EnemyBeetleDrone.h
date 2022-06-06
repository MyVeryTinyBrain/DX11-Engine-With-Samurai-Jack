#pragma once

#include "Enemy.h"

class EnemyBeetleDroneAnimator;

class EnemyBeetleDrone : public Enemy
{
	enum class State
	{
		NONE = 0,
		HIDE,
		APPEAR,
		IDLE,
		TRACE,
		MOVEAROUND,
		ATK,
		DMG,
		DIE,
		DIE_END,
	};

private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

private:

	void SetupCharacterRenderers();
	void SetupAnimator();
	void SetupAttackTrigger();

private:

	void UpdateCCT();
	void UpdateAttachmentObjects();

	void AttackTriggerQuery();
	void OffAttackTriggers();
	void ClearHitBuffer();

private:

	void OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing);
	void OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev);
	void OnAnimationEvent(Ref<AnimatorLayer> layer, const AnimationEventDesc& desc);
	void SetAttackType(uint contextUInt);

private:

	void SetState(EnemyBeetleDrone::State state);
	void StateUpdate();
	void StateChanged(EnemyBeetleDrone::State before, EnemyBeetleDrone::State next);
	void StateEnded(EnemyBeetleDrone::State before, EnemyBeetleDrone::State current);

public:

	virtual float GetHP() const override;
	virtual void SetHP(float value) override;

	virtual V3 GetDirection() const override;
	virtual void SetDirection(const V3& direction) override;
	virtual bool IsGuarding() const override;
	virtual bool IsGuardBreakableByBackattack() const override;
	virtual bool IsInvisible() const override;
	virtual bool IsSuperarmor() const override;
	virtual DamageOutType OnDamage(const DamageOut& out) override;

private:

	// Character Renderers

	GameObject* m_goCharacterRender;
	SkinnedMeshRenderer* m_characterRenderer;
	Ref<NodeTransform> m_rightFoot1Node;
	Ref<NodeTransform> m_leftFoot1Node;

	// Animator

	EnemyBeetleDroneAnimator* m_animator;

	// Attack

	enum
	{
		RH_TRIGGER = 0,
		LH_TRIGGER = 1,
		MAX_TRIGGERS,
	};

	GameObject* m_goAttackTrigger[2];
	SphereCollider* m_attackTrigger[2];
	uint m_attackType = 0;
	unordered_set<Rigidbody*> m_hitBuffer;

	// Stat

	bool m_appeared = false;
	float m_hp = 20.0f;
	float m_idleLeftCount = 0.0f;
	float m_moveAroundAngle = 0.0f;
	float m_moveLeftCount = 0.0f;

	EnemyBeetleDrone::State m_state = EnemyBeetleDrone::State::NONE;
};

