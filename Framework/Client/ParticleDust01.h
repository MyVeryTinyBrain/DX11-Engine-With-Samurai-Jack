#pragma once

class BillboardSpritesheet;

class ParticleDust01 : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	BillboardRenderer* m_billboard;
	
	float m_duration = 0.0f;
	float m_elapsed = 0.0f;
	float m_startScale = 1.0f;
	float m_endScale = 0.0f;
	float m_scalePowFactor = 1.0f;
	Color m_startColor = Color::white();
	Color m_endColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
	float m_colorPowFactor = 1.0f;
	V3 m_velocity = V3::zero();
	float m_damping = 0.0f;

public:

	static ParticleDust01* CreateOnce(
		Scene* scene,
		const V3& position,
		const V3& velocity, float damping,
		float duration,
		float startScale, float endScale, float scalePowFactor,
		Color startColor, Color endColor, float colorPowFactor);

	static void CreateAroundAxis(
		Scene* scene,
		const V3& position,
		const V3& axis, float damping,
		float minRandomSpeed, float maxRandomSpeed,
		float minRandomDuration, float maxRandomDuration,
		float startScale, float endScale, float scalePowFactor,
		Color startColor, Color endColor, float colorPowFactor,
		uint count);

	static void CreateInSphere(
		Scene* scene,
		const V3& position,
		float damping,
		float minRandomSpeed, float maxRandomSpeed,
		float minRandomDuration, float maxRandomDuration,
		float startScale, float endScale, float scalePowFactor,
		Color startColor, Color endColor, float colorPowFactor,
		uint numX, uint numY
	);

	static void CreateWithNormal(
		Scene* scene,
		const V3& position,
		const V3& normal, float minAngle, float maxAngle, float damping,
		float minRandomSpeed, float maxRandomSpeed,
		float minRandomDuration, float maxRandomDuration,
		float startScale, float endScale, float scalePowFactor,
		Color startColor, Color endColor, float colorPowFactor,
		uint count
	);
};

