#pragma once

ENGINE_BEGIN

class Collider;

struct PhysicsContact
{
	V3		point;
	V3		normal;
	V3		impulse;
	float	seperation;
};

struct ENGINE_API Collision
{
public:

	Collision(const PxContactPair& pair, bool inverseNormals);

	Collision(const Collision& rhs);

	Collision(Collision&& rhs) noexcept;

	virtual ~Collision();

public:

	inline Collider* GetBaseCollider() const { return m_baseCollider; }

	inline Collider* GetHitCollider() const { return m_hitCollider; }

	inline uint GetContactCount() const { return m_contactCount; }

	PhysicsContact GetContact(uint index) const;

	_declspec(property(get = GetBaseCollider)) Collider* baseCollider;
	_declspec(property(get = GetHitCollider)) Collider* hitCollider;
	_declspec(property(get = GetContactCount)) uint contactCount;


private:

	uint					m_contactCount;

	PxContactPairPoint*		m_arrContacts;

	bool					m_inverse;

	// � ������ ��ü�� �Ǵ� �ݶ��̴��Դϴ�.
	// ���� ��� �浹 ���ۿ����� �� �ݶ��̴��� �浹 ���ϰ� �Ǵ� �ݶ��̴��� �˴ϴ�.
	Collider*				m_baseCollider;

	// Base Collider �� ������� �ݶ��̴��Դϴ�.
	// ���� ��� �浹 ���ۿ����� �� �ݶ��̴��� Base Collider�� �浹�� �ݶ��̴��� �˴ϴ�. 
	Collider*				m_hitCollider;
};

ENGINE_END
