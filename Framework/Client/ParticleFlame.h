#pragma once

class ParticleFlame : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	BillboardRenderer* m_billboardRenderer;
	
	float m_duration = 3.0f;
	float m_elapsed = 0.0f;
	float m_randomElapsedMultiplier = 1.0f;
	V3 m_velocity;
	float m_damping = 1.0f;
	float m_startScale = 0.0f;
	float m_bridgePercent = 0.2f;
	float m_bridgeScale = 1.0f;
	float m_endScale = 0.0f;
	float m_scalePowFactor = 1.0f;
	Color m_minColor = Color::RGBA255(240, 60, 5, 255);
	Color m_maxColor = Color::RGBA255(255, 240, 20, 255);
	Color m_startColor = Color::white();
	Color m_endColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
	float m_colorPowFactor = 1.0f;

public:

	static ParticleFlame* Create(
		Scene* scene,
		const V3& position,
		float duration,
		const V3& velocity, float damping,
		float startScale, float bridgePrecent, float bridgeScale, float endScale, float scalePowFactor,
		Color startColor, Color endColor, float colorPowFactor
	);
};

