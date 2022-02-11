#include "EnginePCH.h"
#include "PhysicsSimulationCallbackType.h"

Collision::Collision(const PxContactPair& pair, bool inverse)
{
	// =====================================================================================================================
	// pairs의 normal 방향은 shape1 -> shape0 로 향하는 방향입니다.
	// 따라서 shape1을 충돌받는 대상으로 설정하면 shape1 -> shape0 으로 향하는 노멀 방향이므로 문제 없습니다.
	// 반대의 경우인 shape0을 충돌받는 대상으로 할때는, shape1 -> shape0 인 노멀 방향을 거꾸로 뒤집어 사용합니다.
	// inverse == false 일때 shape1 -> shape0 (shape0이 충돌 당하는 대상, shape1이 충돌한 콜라이더) 설정을 사용하고,
	// inverse == true  일때 shape0 -> shape1 (shape1이 충돌 당하는 대상, shape0이 충돌한 콜라이더) 설정으로 사용합니다.
	// =====================================================================================================================

	m_contactCount = pair.contactCount;
	m_arrContacts = new PxContactPairPoint[m_contactCount]{};
	pair.extractContacts(m_arrContacts, m_contactCount);
	m_inverse = inverse;
	m_baseCollider = (Collider*)(m_inverse ? pair.shapes[1]->userData : pair.shapes[0]->userData);
	m_collider = (Collider*)(m_inverse ? pair.shapes[0]->userData : pair.shapes[1]->userData);
	
	// 콜라이더가 없습니다.
	assert(m_collider != nullptr);
}

Collision::Collision(const Collision& rhs)
{
	m_contactCount = rhs.m_contactCount;
	m_arrContacts = new PxContactPairPoint[m_contactCount]{};
	memcpy(m_arrContacts, rhs.m_arrContacts, sizeof(PxContactPairPoint) * m_contactCount);
	m_inverse = rhs.m_inverse;
	m_baseCollider = rhs.m_baseCollider;
	m_collider = rhs.m_collider;
}

Collision::Collision(Collision&& rhs) noexcept
{
	m_contactCount = rhs.m_contactCount;
	m_arrContacts = rhs.m_arrContacts;
	m_inverse = rhs.m_inverse;
	m_baseCollider = rhs.m_baseCollider;
	m_collider = rhs.m_collider;

	rhs.m_contactCount = 0;
	rhs.m_arrContacts = nullptr;
	rhs.m_baseCollider;
	rhs.m_collider = nullptr;
}

Collision::~Collision()
{
	SafeDeleteArray(m_arrContacts);
}

PhysicsContact Collision::GetContact(uint index) const
{
	PhysicsContact contact;

	contact.point = FromPxVec3(m_arrContacts[index].position);
	contact.normal = FromPxVec3(m_arrContacts[index].normal);
	contact.impulse = FromPxVec3(m_arrContacts[index].impulse);
	contact.seperation = m_arrContacts[index].separation;

	if (m_inverse)
		contact.normal *= -1.0f;

	return contact;
}
