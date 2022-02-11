#pragma once

ENGINE_BEGIN

class ENGINE_API PhysicsLayerManager
{
public:

	PhysicsLayerManager();

	~PhysicsLayerManager();

public:

	void ResetLayers();

	void SetCollision(uint8 layerIndexA, uint8 layerIndexB, bool collision);

	bool IsCollision(uint8 layerIndexA, uint8 layerIndexB) const;

	uint32 GetCollisionBits(uint8 layerIndex) const;

public:

	const static uint8 PhysicsLayerMax = 32;

private:

	uint32 m_filter[PhysicsLayerMax];
};

ENGINE_END
