#pragma once

class BillboardSpritesheet;

class ParticleDust01 : public Component
{
private:

	virtual void Awake() override;

private:

	BillboardSpritesheet* m_billboardSpritesheet;
};

