#pragma once
#include "Boss.h"

class BossAncientKingAnimator;

class BossAncientKing : public Boss
{
private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

private:

	void SetupCharacterRenderers();
	void SetupAnimator();
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

	// Character Renderers

	GameObject* m_goCharacterRender;
	SkinnedMeshRenderer* m_characterRenderer;
	Ref<NodeTransform> m_L_Hand_Weapon_cnt_tr;

	// Animator

	BossAncientKingAnimator* m_animator;

	// Hammer

	GameObject* m_goHammer;
	MeshRenderer* m_hammerRenderer;
	GameObject* m_goHammerTrail;
	TrailRenderer* m_hammerTrailRenderer;

	// Stat

	float m_hp = 100.0f;
};

