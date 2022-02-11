#include "EnginePCH.h"
#include "PhysicsSimulationCallbackType.h"

Collision::Collision(const PxContactPair& pair, bool inverse)
{
	// =====================================================================================================================
	// pairs�� normal ������ shape1 -> shape0 �� ���ϴ� �����Դϴ�.
	// ���� shape1�� �浹�޴� ������� �����ϸ� shape1 -> shape0 ���� ���ϴ� ��� �����̹Ƿ� ���� �����ϴ�.
	// �ݴ��� ����� shape0�� �浹�޴� ������� �Ҷ���, shape1 -> shape0 �� ��� ������ �Ųٷ� ������ ����մϴ�.
	// inverse == false �϶� shape1 -> shape0 (shape0�� �浹 ���ϴ� ���, shape1�� �浹�� �ݶ��̴�) ������ ����ϰ�,
	// inverse == true  �϶� shape0 -> shape1 (shape1�� �浹 ���ϴ� ���, shape0�� �浹�� �ݶ��̴�) �������� ����մϴ�.
	// =====================================================================================================================

	m_contactCount = pair.contactCount;
	m_arrContacts = new PxContactPairPoint[m_contactCount]{};
	pair.extractContacts(m_arrContacts, m_contactCount);
	m_inverse = inverse;
	m_baseCollider = (Collider*)(m_inverse ? pair.shapes[1]->userData : pair.shapes[0]->userData);
	m_collider = (Collider*)(m_inverse ? pair.shapes[0]->userData : pair.shapes[1]->userData);
	
	// �ݶ��̴��� �����ϴ�.
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
