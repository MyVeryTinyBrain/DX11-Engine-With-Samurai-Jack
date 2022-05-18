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

	// 어떤 현상의 주체가 되는 콜라이더입니다.
	// 예를 들어 충돌 시작에서는 이 콜라이더가 충돌 당하게 되는 콜라이더가 됩니다.
	Collider*				m_baseCollider;

	// Base Collider 에 상대적인 콜라이더입니다.
	// 예를 들어 충돌 시작에서는 이 콜라이더가 Base Collider에 충돌한 콜라이더가 됩니다. 
	Collider*				m_hitCollider;
};

ENGINE_END
