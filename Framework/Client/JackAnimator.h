#pragma once

class JackAnimator : public Animator
{
private:

	virtual void Awake() override;

private:

	bool Initialize();
	bool FindJackMesh();
	void SetupProperties();
	void SetupMainLayer();

private:

	ResourceRef<Mesh> m_jackMesh;

	AnimatorLayer* m_layer = nullptr;

public:

	AnimatorProperty* weaponTypeProperty; // float
	AnimatorProperty* moveTypeProperty; // float
	AnimatorProperty* moveProperty; // float
	AnimatorProperty* attackProperty; // trigger
	AnimatorProperty* comboProperty; // trigger
	AnimatorProperty* jumpProperty; // trigger
	AnimatorProperty* jumpingProperty; // trigger
	AnimatorProperty* hasGroundProperty; // bool
};

