#pragma once

#include "Boss.h"

class BossAshiAnimator;

class BossAshi : public Boss
{
	enum class State
	{
		NONE = 0,

		IDLE,
		TURN,
		WALK_ANYWHERE,
		WALK_TRACE,
		RUN_TRACE,
		DIE,

		ATK_NEAR_RAND,
		ATK_FAR_RAND,

		__ATK_NEAR_BEGIN,
		ATK_DOUBLEHAND_SLASH,
		ATK_H_SLASH,
		ATK_SHOLDER_SLASH,
		ATK_SLASHUP,

		ATK_DROPKICK,
		ATK_LEGSTEP,
		ATK_SPINKICK,
		__ATK_NEAR_END,

		__ATK_FAR_BEGIN,
		ATK_LASER,
		ATK_RAGE,
		ATK_RUSH,
		__ATK_FAR_END,

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

	void WalkDirectionLerp();
	void SetWalkDirectionLerp(float angle, float duration);
	void StopWalkDirectionLerp();
	float RandomWalkDirectionChangeTime() const;

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
	int m_numComboAttack = 0;

	// Audio

	AudioSource* m_audioSource;

	float m_idleLeftCounter = 0.0f;

	// Stat

	float m_hp = 1000.0f;

	// State

	BossAshi::State m_state = BossAshi::State::NONE;

	// Walk Anywhere

	float m_walkLeftTime = 0.0f;
	float m_startWalkAngle = 0.0f;
	float m_targetWalkAngle = 0.0f;
	float m_walkDirectionLerpDuration = 1.0f;
	bool m_walkDirectionLerp = false;
	float m_walkDirectionLerpAcc = 0.0f;
	float m_walkDirectionChangeLeftTime = 0.0f;

	// Move

	float m_moveAcc = 0.0f;

	// Manual Look

	bool m_manualLook = false;
};

