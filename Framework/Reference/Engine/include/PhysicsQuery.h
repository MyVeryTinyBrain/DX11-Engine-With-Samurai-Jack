#pragma once

// QueryTest: 쿼리의 성공 여부만을 반환합니다. (매우 빠름)
// Query: 가장 가까운 쿼리된 결과를 반환합니다. (빠름)
// QueryAll: 거리에 따라 정렬된 쿼리된 결과들을 반환합니다. (느림)
//
// HIT_MAX: QueryAll로 검출 가능한 최대 쿼리 결과의 갯수입니다.
//
// Sweep 요청에 사용될수 있는 Geometry는 Box, Sphere, Capusle, Convex 입니다.

#include "PhysicsQueryType.h"

ENGINE_BEGIN
class Rigidbody;
class Collider;
class ENGINE_API PhysicsQuery
{
	enum : uint
	{
		LAYER_ZERO = 0x00000000,
		LAYER_ALL = 0xFFFFFFFF,
		TYPE_ZERO = (uint)PhysicsQueryTypes::None,
		TYPE_COLLIDER = (uint)PhysicsQueryTypes::Collider,
		TYPE_TRIGGER = (uint)PhysicsQueryTypes::Trigger,
		TYPE_ALL = (uint)PhysicsQueryTypes::All,
		HIT_MAX = 128,
	};

public:

	PhysicsQuery(PxScene* pxScene);

	~PhysicsQuery() = default;

public:

	bool RaycastTest(const PhysicsRay& ray, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	bool Raycast(PhysicsHit& hit, const PhysicsRay& ray, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<PhysicsHit> RaycastAll(const PhysicsRay& ray, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool OverlapGeometryTest(const PxGeometryHolder& g, const V3& p, const Q& r, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	Collider* OverlapGeometry(const PxGeometryHolder& g, const V3& p, const Q& r, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<Collider*> OverlapGeometryAll(const PxGeometryHolder& g, const V3& p, const Q& r, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool OverlapTest(Collider* c, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL) const;
	Collider* Overlap(Collider* c, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL) const;
	vector<Collider*> OverlapAll(Collider* c, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL) const;

	bool OverlapPointTest(const V3& p, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	Collider* OverlapPoint(const V3& p, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<Collider*> OverlapPointAll(const V3& p, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool OverlapSphereTest(const V3& p, float r, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	Collider* OverlapSphere(const V3& p, float r, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<Collider*> OverlapSphereAll(const V3& p, float r, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool OverlapBoxTest(const V3& p, const Q& r, const V3& ex, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	Collider* OverlapBox(const V3& p, const Q& r, const V3& ex, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<Collider*> OverlapBoxAll(const V3& p, const Q& r, const V3& ex, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool OverlapCapsuleTest(const V3& p, const Q& r, float rad, float halfHeight, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	Collider* OverlapCapsule(const V3& p, const Q& r, float rad, float halfHeight, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<Collider*> OverlapCapsuleAll(const V3& p, const Q& r, float rad, float halfHeight, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool SweepGeometryTest(const PxGeometryHolder& g, const V3& p, const Q& r, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	bool SweepGeometry(PhysicsHit& hit, const PxGeometryHolder& g, const V3& p, const Q& r, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<PhysicsHit> SweepGeometryAll(const PxGeometryHolder& g, const V3& p, const Q& r, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool SweepTest(Collider* c, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	bool Sweep(PhysicsHit& hit, Collider* c, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<PhysicsHit> SweepAll(Collider* c, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool SweepSphereTest(const V3& p, float r, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	bool SweepSphere(PhysicsHit& hit, const V3& p, float r, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<PhysicsHit> SweepSphereAll(const V3& p, float r, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool SweepBoxTest(const V3& p, const Q& r, const V3& ex, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	bool SweepBox(PhysicsHit& hit, const V3& p, const Q& r, const V3& ex, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<PhysicsHit> SweepBoxAll(const V3& p, const Q& r, const V3& ex, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

	bool SweepCapsuleTest(const V3& p, const Q& r, float rad, float halfHeight, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	bool SweepCapsule(PhysicsHit& hit, const V3& p, const Q& r, float rad, float halfHeight, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;
	vector<PhysicsHit> SweepCapsuleAll(const V3& p, const Q& r, float rad, float halfHeight, const V3& dir, float length, uint32 layermask = LAYER_ALL, PhysicsQueryType type = TYPE_ALL, Rigidbody* ignoreRigidbody = NULL) const;

private:

	PxScene*						m_scene = nullptr;

	PxHitFlags						m_flags;
	PxQueryFilterData				m_testFilter;
	PxQueryFilterData				m_queryFilter;

	Q								m_qZ180 = Q::Euler(0.0f, 0.0f, 90.0f);
	Q								m_qI = Q::identity();
};

ENGINE_END
