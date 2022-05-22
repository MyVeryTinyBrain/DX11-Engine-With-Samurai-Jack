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

private:

	void UpdateKeyTimes();
	V3 GetTranslateDirection() const;
	bool IsShortKeyPressed(float pressedTime) const;
	bool IsLongKeyPressing(float pressingTime) const;

private:

	void OnBeginChanging(Ref<AnimatorLayer> layer, Ref<AnimatorNode> changing);
	void OnEndChanged(Ref<AnimatorLayer> layer, Ref<AnimatorNode> endChanged, Ref<AnimatorNode> prev);
	void OnAnimationEvent(Ref<AnimatorLayer> layer, const string& context);

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
	GameObject*				m_goKatanaRenderer;
	MeshRenderer*			m_katanaRenderer;
	GameObject*				m_goKatanaTrail;
	TrailRenderer*			m_katanaTrailRenderer;

	// Foot

	GameObject*				m_goFootCnt;
	TrailRenderer*			m_footTrailRenderer;

	// Attack

#define FORWARD_TRIGGER		0
#define KATANA_TRIGGER		1
#define FOOT_TRIGGER		2
	GameObject*				m_goAttackTrigger[3];
	SphereCollider*			m_attackTrigger[3];

	// Input

	float					m_lShiftPressedTime;
	float					m_lShiftPressingTime;
	float					m_leftMousePressedTime;
	float					m_leftMousePressingTime;
};

