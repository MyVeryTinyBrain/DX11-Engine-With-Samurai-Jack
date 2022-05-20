#pragma once

#include "Character.h"

class TPSCamera;
class PlayerAnimator;

class Player : public Character
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:

	void SetupTPSCamera();
	void SetupCharacterRenderers();
	void SetupAnimator();
	void SetupWeapons();

	void UpdateCCT();
	void UpdateAttachmentObjects();

	void RotateToDirection(const V3& delta);

	V3 GetTranslateDirection();

private:

	// TPS Camera

	TPSCamera*				m_tpsCamera;

	// Character Renderers

	GameObject*				m_goCharacterRender;
	SkinnedMeshRenderer*	m_characterRenderer;
	Ref<NodeTransform>		m_rightHandWeaponCntNode;
	Ref<NodeTransform>		m_scabbardCntNode;

	GameObject*				m_goKatanaSheathRenderer;
	MeshRenderer*			m_katanaSheathRenderer;

	// Animator

	PlayerAnimator*			m_animator;

	// Katana

	GameObject*				m_goKatana;
	GameObject*				m_goKatanaRenderer;
	MeshRenderer*			m_katanaRenderer;
	GameObject*				m_goTrail;
	TrailRenderer*			m_trailRenderer;
};

