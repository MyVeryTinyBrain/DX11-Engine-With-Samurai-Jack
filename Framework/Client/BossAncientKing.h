#pragma once
#include "Boss.h"

class BossAncientKingAnimator;
class EffectElectricBeam;

class BossAncientKing : public Boss
{
	enum class State
	{
		NONE = 0,

		IDLE,
		LOOK,
		TRACE,
		GROGY,
		DIE,

		SATK_TURN,

		ATK_NEAR_RAND,
		ATK_FAR_RAND,

		__ATK_NEAR_BEGIN,
		ATK_SWING_H,
		ATK_SWING_V,
		ATK_STOMP,
		ATK_STEPON,
		ATK_JUMP,
		ATK_DOWNSTRIKE,
		ATK_NEAR_RUSH,
		//ATK_NEAR_BEAM,
		ATK_BACKJUMP,
		__ATK_NEAR_END,

		__ATK_FAR_BEGIN,
		ATK_RUSH,
		ATK_ELECTRIC,
		ATK_BEAM,
		__ATK_FAR_END,

		RAGE,
	};

private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

private:

	void SetupCollider();
	void SetupCharacterRenderers();
	void SetupAnimator();
	void SetupLight();
	void SetupHammer();
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

	virtual float GetMaxHP() const override;
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

	void SetState(BossAncientKing::State state);
	void StateUpdate();
	void StateChanged(BossAncientKing::State before, BossAncientKing::State next);
	void StateEnded(BossAncientKing::State before, BossAncientKing::State current);

	bool IsFarATKCondition() const;
	bool IsNearATKCondition() const;
	bool IsSATKTurnCondition() const;

private:

	// Character Renderers

	GameObject* m_goCharacterRender;
	SkinnedMeshRenderer* m_characterRenderer;
	Ref<NodeTransform> m_R_Hand_Weapon_cnt_tr;
	Ref<NodeTransform> m_L_Hand_Weapon_cnt_tr; // Hammer controll
	Ref<NodeTransform> m_Head;
	Ref<NodeTransform> m_RightFoot;
	Ref<NodeTransform> m_LeftFoot;
	Ref<NodeTransform> m_RightLeg;
	Ref<NodeTransform> m_LeftLeg;
	Ref<NodeTransform> m_Spine1;
	Ref<NodeTransform> m_RightArmRoll;
	Ref<NodeTransform> m_LeftArmRoll;
	Ref<NodeTransform> m_RightForeArmRoll;
	Ref<NodeTransform> m_LeftForeArmRoll;
	V3 m_toPlayerDirection;

	// Animator

	BossAncientKingAnimator* m_animator;

	// Light

	GameObject* m_goLight;
	PointLight* m_pointLight;

	// Hammer

	GameObject* m_goHammer;
	MeshRenderer* m_hammerRenderer;
	GameObject* m_goHammerTip;

	// Collider for push the player

	CapsuleCollider* m_collider;

	// Attack

	enum
	{
		HAMMER_TRIGGER = 0,
		FOOT_L_TRIGGER,
		FOOT_R_TRIGGER,
		HAMMER_TIP_TRIGGER,
		MAX_TRIGGERS,
	};

	GameObject* m_goAttackTrigger[MAX_TRIGGERS];
	Collider* m_attackTrigger[MAX_TRIGGERS];
	uint m_attackType = 0;
	unordered_set<Rigidbody*> m_hitBuffer;
	bool m_gadableAttack = false;

	EffectElectricBeam* m_electricBeam;

	// Audio

	AudioSource* m_audioSource;

	// State

	State m_state = State::NONE;

	// Stat

	float m_hp = GetMaxHP();

	// Idle

	float m_idleLeftCounter = 0.0f;
	float m_waitLeftCounter = 0.0f; // 직접 설정해서 사용하는 값입니다.

	// Trace

	float m_traceOutAngleAccCounter = 0.0f;
	float m_traceOrLookAccCounter = 0.0f;

	// Spin attack

	// 뒤돌기 공격을 사용하면 활성화됩니다.
	// 다른 공격을 한 번 하면 다시 비활성화됩니다.
	bool m_usedSATK_TURN = false; 
	
	// Near attack
	stack<int> m_nearAttackStack;

	// Far attack
	
	float m_farATKCounter = 0.0f;
	int m_farATKIndex = 0;
	int m_backJumpFarATKIndex = 0;

	// Aimming

	V3 m_aimPosition;

	// Look

	bool m_manualLook = false;
	
	// Rush

	bool m_rushDamageReady = false;
	
	// Grogy
	
	uint m_hitCount = 0;
};

