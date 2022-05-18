#pragma once

class TPSCamera;
class JackAnimator;

class Player : public Component
{
private:

	virtual void Awake() override;

	virtual void Update() override;

	virtual void LateUpdate() override;

private:

	void SetupTPSCamera();

	void SetupCharacterRenderers();

	void SetupAnimator();

	void SetupPhysics();

	void SetupWeapons();

	void UpdateAttachmentObjects();

private:

	// TPS Camera

	GameObject*				m_goTPSCamera;
	TPSCamera*				m_tpsCamera;

	// Character Renderers

	GameObject*				m_goCharacterRender;
	SkinnedMeshRenderer*	m_characterRenderer;
	Ref<NodeTransform>		m_rightHandWeaponCntNode;
	Ref<NodeTransform>		m_scabbardCntNode;

	GameObject*				m_goKatanaSheathRenderer;
	MeshRenderer*			m_katanaSheathRenderer;

	// Animator

	JackAnimator*			m_jackAnimator;

	// Physics

	CharacterController*	m_controller;

	GameObject*				m_goWeaponTrigger;
	SphereCollider*			m_weaponTrigger;

	// Katana

	GameObject*				m_goKatana;
	GameObject*				m_goKatanaRenderer;
	MeshRenderer*			m_katanaRenderer;
	GameObject*				m_goTrail;
	TrailRenderer*			m_trailRenderer;
};

