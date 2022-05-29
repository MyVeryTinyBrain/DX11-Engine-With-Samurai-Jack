#pragma once

class EffectSpark : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	void OnCollisionEnter(const Collision& collision);

public:

	void SetVelocity(const V3& velocity);

private:

	BillboardRenderer* m_renderer;
	Rigidbody* m_rigidbody;
	SphereCollider* m_collider;
	float m_duration = 0.0f;
	float m_elapsed = 0.0f;
	float m_initScale = 0.0f;

public:

	static void Create(
		Scene* scene, 
		const V3& position, 
		const V3& normal,
		float minAngle, float maxAngle,
		float minVSpeed, float maxVSpeed, 
		float minHSpeed, float maxHSpeed, 
		float minScale, float maxScale, 
		float minDuration, float maxDuration,
		uint count);
};

