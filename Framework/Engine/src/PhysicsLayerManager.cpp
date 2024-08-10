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
	assert(layerIndexA < PhysicsLayerMax); // ���̾� �ε����� ������ �ʰ��߽��ϴ�.
	assert(layerIndexB < PhysicsLayerMax); // ���̾� �ε����� ������ �ʰ��߽��ϴ�.
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
	assert(layerIndexA < PhysicsLayerMax); // ���̾� �ε����� ������ �ʰ��߽��ϴ�.
	assert(layerIndexB < PhysicsLayerMax); // ���̾� �ε����� ������ �ʰ��߽��ϴ�.
	return (m_filter[layerIndexA] & (1 << layerIndexB));
}

uint32 PhysicsLayerManager::GetCollisionBits(uint8 layerIndex) const
{
	assert(layerIndex < PhysicsLayerMax); // ���̾� �ε����� ������ �ʰ��߽��ϴ�.
	return m_filter[layerIndex];
}
