#pragma once

class EffectGlow : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	BillboardRenderer* m_billboardRenderer;

	float m_elapsed = 0.0f;
	float m_duration = 0.0f;
	V2 m_startScale = V2::one();
	V2 m_endScale = V2::one();
	float m_scalePowFactor = 1.0f;
	Color m_startColor = Color::white();
	Color m_endColor = Color::white();
	float m_colorPowFactor = 1.0f;

public:

	static EffectGlow* Create(
		Scene* scene,
		const V3& position,
		float duration,
		const V2& startScale, const V2& endScale, float scalePowFactor,
		const Color& startColor, const Color& endColor, float colorPowFactor
	);
};

