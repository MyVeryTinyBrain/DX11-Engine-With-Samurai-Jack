#pragma once

class ParticleElectricBeam;

class EffectElectricBeam : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnEnable() override;

private:

	ParticleElectricBeam* CreateParticle(float percent);

private:

	void AttackTriggerQuery();
	void ClearHitBuffer();

private:

	GameObject* m_goBox;
	BoxCollider* m_boxTrigger;
	float m_particleWaitCount = 0.0f;
	float m_particleDelay = 0.05f;
	float m_particleDensity = 1.0f;

	// Attack

	unordered_set<Rigidbody*> m_hitBuffer;
	uint m_attackLayermask;
};
