#pragma once

#include "Boss.h"

class BossAshiAnimator;

class BossAshi : public Boss
{
	enum class State
	{
		NONE = 0,

		IDLE,
		WAIT, // waitCounter를 설정해줄수 있습니다. 동작은 IDLE과 같습니다. waitCounter를 설정하지 않으면 정의된 기본값을 사용합니다.
		LOOK,
		WALK_ANYWHERE,
		WALK_TRACE,
		RUN_TRACE,
		DIE,

		APPEAR,
	};

private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

private:

	void SetupCollider();
	void SetupCharacterRenderer();
	void SetupAnimator();
	void SetupSword();
	void SetupAttackTrigger();
	void SetupAudioSource();

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
	void OnCCTCollision(const CCTCollision& collision);

private:

	void SetState(BossAshi::State state);
	void StateUpdate();
	void StateChanged(BossAshi::State before, BossAshi::State next);
	void StateEnded(BossAshi::State before, BossAshi::State current);

private:

	// Character Renderers

	GameObject* m_goCharacterRender;
	SkinnedMeshRenderer* m_characterRenderer;
	Ref<NodeTransform> m_R_Hand_Weapon_cnt_tr;
	Ref<NodeTransform> m_RightFoot;

	// Sword

	GameObject* m_goSword;
	MeshRenderer* m_swordRenderer;
	GameObject* m_goSwordTip;

	// Animator

	BossAshiAnimator* m_animator;

	// Collider for push the player

	CapsuleCollider* m_collider;

	// Attack

	enum
	{
		SWORD_TRIGGER = 0,
		RIGHT_FOOT_TRIGGER,
		MAX_TRIGGERS,
	};

	GameObject* m_goAttackTrigger[MAX_TRIGGERS];
	Collider* m_attackTrigger[MAX_TRIGGERS];
	uint m_attackType = 0;
	unordered_set<Rigidbody*> m_hitBuffer;
	bool m_gadableAttack = false;

	// Audio

	AudioSource* m_audioSource;

	float m_idleLeftCounter = 0.0f;

	// Stat

	float m_hp = 1000.0f;
};

