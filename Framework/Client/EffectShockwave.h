#pragma once

class EffectShockwave : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

public:

	static void Create(
		Scene* scene,
		const V3& groundPosition,
		float duration,
		float height,
		float startScale, float endScale,
		const Color& startColor = Color::RGBA255(205, 167, 121, 255), const Color& endColor = Color::RGBA255(205, 167, 121, 0));

private:

	GameObject* m_goCnt;
	MeshRenderer* m_renderer;
	ResourceRef<Material> material;
	V4 m_uvAdjust;

	float m_elapsed = 0.0f;
	float m_duration = 1.0f;
	float m_startScale = 0.5f;
	float m_endScale = 5.0f;
	Color m_startColor = Color::RGBA255(205, 167, 121, 255);
	Color m_endColor = Color(m_startColor.r, m_startColor.g, m_startColor.b, 0.0f);
};

