#pragma once

class ParticleElectricBeam : public Component
{
public:

	enum class Type { _1, _2, _3, _4, _5, _6, _7, NUM_TYPES, RANDOM };

private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

private:

	GameObject* m_goParent;
	MeshRenderer* m_renderer;
	float m_duration = 0.0f;
	float m_elapsed = 0.0f;
	float m_spinAngle = 0.0f;

public:

	static ParticleElectricBeam* Create(
		Scene* scene, 
		ParticleElectricBeam::Type type, 
		const V3& position, 
		const V3& forward, 
		float spinAngle,
		float duration);
};

