#pragma once

class ProjectileWaveBeam : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnDestroyed() override;

private:

	MeshRenderer* m_renderer;
	SphereCollider* m_trigger;
	float m_elapsed = 0.0f;
	float m_speed = 40.0f;
	V3 m_hitNormal = {};
};

