#pragma once

class BillboardAnimation;

class EffectExplosion : public Component
{
private:

	virtual void Awake() override;
	void OnLooped();

private:

	BillboardAnimation* m_billboardAnimation;
};

