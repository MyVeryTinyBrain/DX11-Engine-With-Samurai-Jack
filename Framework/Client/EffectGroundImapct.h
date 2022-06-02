#pragma once

class EffectGroundImapct : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnEnable() override;

	void AttackTriggerQuery();
	void ClearHitBuffer();

private:

	GameObject* m_parent;
	MeshRenderer* m_renderer;

	// Attack

	unordered_set<Rigidbody*> m_hitBuffer;
	uint m_attackLayermask;
	BoxCollider* m_trigger;

	float m_elapsed = 0.0f;
	float m_duration = 1.0f;
	float m_powFactor = 1.0f;
	float m_startScale = 1.0f;
	float m_endScale = 5.0f;
	float m_distortion = 100.0f;
	Color m_initColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

public:

	static void Create(
		Scene* scene,
		const V3& position,
		float duration,
		float powFactor,
		float distortion,
		float startScale, float endScale,
		const Color& color
	);
};

