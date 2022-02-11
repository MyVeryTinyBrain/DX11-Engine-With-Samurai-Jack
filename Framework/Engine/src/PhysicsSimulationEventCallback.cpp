#include "EnginePCH.h"
#include "PhysicsSimulationEventCallback.h"
#include "Collider.h"

void PhysicsSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (uint i = 0; i < pairHeader.nbPairs; ++i)
	{
		const PxContactPair& pair = pairs[i];

		if (pair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			Collision enterB2A(pair, false);
			Collision enterA2B(pair, true);
			m_enterings.push_back(std::move(enterB2A));
			m_enterings.push_back(std::move(enterA2B));
		}
		else if (pair.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			Collision exitB2A(pair, false);
			Collision exitA2B(pair, true);
			m_exitings.push_back(std::move(exitA2B));
			m_exitings.push_back(std::move(exitB2A));
		}
		else if (pair.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			Collision stayB2A(pair, false);
			Collision stayA2B(pair, true);
			m_stayings.push_back(std::move(stayB2A));
			m_stayings.push_back(std::move(stayA2B));
		}
	}
}

void PhysicsSimulationEventCallback::ClearBuffers()
{
	m_enterings.clear();
	m_exitings.clear();
	m_stayings.clear();
}

void PhysicsSimulationEventCallback::ExecuteNotify()
{
	for (auto& pair : m_enterings)
	{
		pair.baseCollider->OnCollisionEnter(pair);
	}

	for (auto& pair : m_exitings)
	{
		pair.baseCollider->OnCollisionExit(pair);
	}

	for (auto& pair : m_stayings)
	{
		pair.baseCollider->OnCollisionStay(pair);
	}

	ClearBuffers();
}
