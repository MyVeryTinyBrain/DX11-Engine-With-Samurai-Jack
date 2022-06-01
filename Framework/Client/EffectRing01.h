#pragma once

class EffectRing01 : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	BillboardRenderer* m_renderer;

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

