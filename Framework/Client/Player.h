#pragma once

class TPSCamera;
class JackAnimator;

class Player : public Component
{
private:

	virtual void Awake() override;

	virtual void Update() override;

	virtual void LateUpdate() override;

	void OnCollisionEnter(const Collision& collision);

	void OnCollisionExit(const Collision& collision);

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

	Rigidbody*				m_rigidbody;

	GameObject*				m_goCollider;
	CapsuleCollider*		m_collider;

	GameObject*				m_goWeaponTrigger;
	SphereCollider*			m_weaponTrigger;

	// Katana

	GameObject*				m_goKatana;
	GameObject*				m_goKatanaRenderer;
	MeshRenderer*			m_katanaRenderer;
};

