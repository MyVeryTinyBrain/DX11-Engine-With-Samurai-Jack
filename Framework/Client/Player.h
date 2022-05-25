#pragma once

#include "Character.h"

class TPSCamera;
class PlayerAnimator;

class Player : public Character
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate() override;
	virtual void OnDestroyed() override;

private:

	void SetupTPSCamera();
	void SetupCharacterRenderers();
	void SetupAnimator();
	void SetupKatanaSheet();
	void SetupKatana();
	void SetupFootCnt();
	void SetupAttackTrigger();

	void UpdateCCT();
	void UpdateAttachmentObjects();

	void XZInput();
	void DashInput();
	void JumpInput();
	void RollInput();
	void AttackInput();
	void GuardInput();

	void AttackTriggerQuery();
	void OffAttackTriggers();
	void ClearHitBuffer();

private:

	void UpdateKeyTimes();
	V3 GetTranslateDirection() const;
	bool IsShortKeyPressed(float pressedTime) const;
	bool IsLongKeyPressing(float pressingTime) const;

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

public:

	TPSCamera* GetTPSCamera() const { return m_tpsCamera; }

private:

	// TPS Camera

	TPSCamera*				m_tpsCamera;

	// Character Renderers

	GameObject*				m_goCharacterRender;
	SkinnedMeshRenderer*	m_characterRenderer;
	Ref<NodeTransform>		m_rightHandWeaponCntNode;
	Ref<NodeTransform>		m_scabbardCntNode;
	Ref<NodeTransform>		m_leftToeBaseNode;

	GameObject*				m_goKatanaSheathRenderer;
	MeshRenderer*			m_katanaSheathRenderer;

	// Animator

	PlayerAnimator*			m_animator;

	// Katana

	GameObject*				m_goKatana;
	MeshRenderer*			m_katanaRenderer;
	GameObject*				m_goKatanaTrail;
	TrailRenderer*			m_katanaTrailRenderer;

	// Foot

	GameObject*				m_goFootCnt;
	TrailRenderer*			m_footTrailRenderer;

	// Attack

	enum
	{
		KATANA_TRIGGER = 0,
		FOOT_TRIGGER = 1,
		MAX_TRIGGERS,
	};

	GameObject*					m_goAttackTrigger[2];
	SphereCollider*				m_attackTrigger[2];
	int							m_attackType = 0;
	unordered_set<Rigidbody*>	m_hitBuffer;

	// Input

	float					m_lShiftPressedTime;
	float					m_lShiftPressingTime;
	float					m_leftMousePressedTime;
	float					m_leftMousePressingTime;

	// Static

	static Player*			g_player;

public:

	static Player* GetInstance() { return g_player; }
};

