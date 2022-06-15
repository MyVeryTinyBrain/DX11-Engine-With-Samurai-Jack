#pragma once

class ParticleSpark : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	void OnCollisionEnter(const Collision& collision);

private:

	void Init(bool useRigidbody);

public:

	void SetVelocity(const V3& velocity);

private:

	BillboardRenderer* m_renderer;
	Rigidbody* m_rigidbody;
	SphereCollider* m_collider;
	bool m_useRigidbody = true;
	V3 m_fakeVelocity = V3::zero();

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
		uint count,
		bool useRigidbody,
		const Color& color = Color(1.0f, 0.9764f, 0.466f, 1.0f));
};

