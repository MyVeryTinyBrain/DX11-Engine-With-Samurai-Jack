#pragma once
#include "Boss.h"

class BossAncientKingAnimator;

class BossAncientKing : public Boss
{
	enum class State
	{
		NONE = 0,

		IDLE,
		WAIT, // waitCounter�� �������ټ� �ֽ��ϴ�. ������ IDLE�� �����ϴ�. waitCounter�� �������� ������ ���ǵ� �⺻���� ����մϴ�.
		LOOK,
		TRACE,
		GROGY,
		DIE,

		SATK_TURN,

		ATK_RAND,

		ATK_BEGIN,

		ATK_SWING_H,
		ATK_SWING_V,
		ATK_STOMP,
		ATK_STEPON,
		ATK_RUSH,
		ATK_JUMP,
		ATK_ELECTRIC,
		ATK_DOWNSTRIKE,
		ATK_BEAM,

		ATK_END,

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
	void SetAttackType(int contextInt);

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

	void SetState(BossAncientKing::State state);
	void StateUpdate();
	void StateChanged(BossAncientKing::State before, BossAncientKing::State next);
	void StateEnded(BossAncientKing::State before, BossAncientKing::State current);

	bool RaycastToForwardInStage(float length) const;
	bool IsATKCondition() const;
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

	// Animator

	BossAncientKingAnimator* m_animator;

	// Light

	GameObject* m_goLight;
	PointLight* m_pointLight;

	// Hammer

	GameObject* m_goHammer;
	MeshRenderer* m_hammerRenderer;
	GameObject* m_goHammerTrail;
	TrailRenderer* m_hammerTrailRenderer;

	// Collider for push the player
	GameObject* m_goCollider;
	CapsuleCollider* m_collider;

	// Stat

	float m_hp = 100.0f;
	State m_state = State::NONE;
	float m_idleLeftCounter = 0.0f;
	float m_waitLeftCounter = 0.0f; // ���� �����ؼ� ����ϴ� ���Դϴ�.
	float m_traceOutAngleAccCounter = 0.0f;
	float m_traceOrLookAccCounter = 0.0f;
	// �ڵ��� ������ ����ϸ� Ȱ��ȭ�˴ϴ�.
	// �ٸ� ������ �� �� �ϸ� �ٽ� ��Ȱ��ȭ�˴ϴ�.
	bool m_usedSATK_TURN = false; 
};

