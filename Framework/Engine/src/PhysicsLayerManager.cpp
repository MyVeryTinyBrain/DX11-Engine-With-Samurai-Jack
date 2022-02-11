#include "EnginePCH.h"
#include "PhysicsLayerManager.h"

PhysicsLayerManager::PhysicsLayerManager()
{
	ResetLayers();
}

PhysicsLayerManager::~PhysicsLayerManager()
{
}

void PhysicsLayerManager::ResetLayers()
{
	ZeroMemory(m_filter, sizeof(m_filter));
	SetCollision(0, 0, true);
}

void PhysicsLayerManager::SetCollision(uint8 layerIndexA, uint8 layerIndexB, bool collision)
{
	// ���̾� �ε����� ������ �ʰ��߽��ϴ�.
	assert(layerIndexA < PhysicsLayerMax);
	assert(layerIndexB < PhysicsLayerMax);

	if (collision)
	{
		m_filter[layerIndexA] |= (1 << layerIndexB);
		m_filter[layerIndexB] |= (1 << layerIndexA);
	}
	else
	{
		m_filter[layerIndexA] &= ~(1 << layerIndexB);
		m_filter[layerIndexB] &= ~(1 << layerIndexA);
	}
}

bool PhysicsLayerManager::IsCollision(uint8 layerIndexA, uint8 layerIndexB) const
{
	// ���̾� �ε����� ������ �ʰ��߽��ϴ�.
	assert(layerIndexA < PhysicsLayerMax);
	assert(layerIndexB < PhysicsLayerMax);

	return (m_filter[layerIndexA] & (1 << layerIndexB));
}

uint32 PhysicsLayerManager::GetCollisionBits(uint8 layerIndex) const
{
	// ���̾� �ε����� ������ �ʰ��߽��ϴ�.
	assert(layerIndex < PhysicsLayerMax);

	return m_filter[layerIndex];
}
