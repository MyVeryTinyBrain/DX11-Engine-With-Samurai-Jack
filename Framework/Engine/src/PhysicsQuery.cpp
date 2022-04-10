#include "EnginePCH.h"
#include "PhysicsQuery.h"
#include "PhysicsQueryFilterCallback.h"
#include "Collider.h"
#include "Transform.h"

PhysicsQuery::PhysicsQuery(PxScene* pxScene)
{
	m_scene = pxScene;

	m_flags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eFACE_INDEX | PxHitFlag::eMESH_BOTH_SIDES;
	m_testFilter = PxQueryFilterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eANY_HIT);
	m_queryFilter = PxQueryFilterData(PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER);
}

bool PhysicsQuery::RaycastTest(const PhysicsRay& ray, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxRaycastBuffer buffer;

	return m_scene->raycast(ToPxVec3(ray.Point), ToPxVec3(ray.Direction), ray.Length, buffer, m_flags, m_testFilter, &callback);
}

bool PhysicsQuery::Raycast(PhysicsHit& hit, const PhysicsRay& ray, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxRaycastBuffer buffer;

	bool result = m_scene->raycast(ToPxVec3(ray.Point), ToPxVec3(ray.Direction), ray.Length, buffer, m_flags, m_queryFilter, &callback);
	if (!result) return false;

	const PxRaycastHit& data = buffer.getAnyHit(0);
	hit.Collider = (Collider*)data.actor;
	hit.Distance = data.distance;
	hit.Normal = FromPxVec3(data.normal);
	hit.Point = FromPxVec3(data.position);
	return true;
}

vector<PhysicsHit> PhysicsQuery::RaycastAll(const PhysicsRay& ray, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = false;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxRaycastHit datas[HIT_MAX];
	PxRaycastBuffer buffer(datas, HIT_MAX);

	m_scene->raycast(ToPxVec3(ray.Point), ToPxVec3(ray.Direction), ray.Length, buffer, m_flags, m_queryFilter, &callback);

	uint numHits = buffer.getNbAnyHits();
	vector<PhysicsHit> hits;
	hits.resize(numHits);

	for(uint i = 0; i < numHits; ++i)
	{
		hits[i].Collider = (Collider*)datas[i].actor;
		hits[i].Distance = datas[i].distance;
		hits[i].Normal = FromPxVec3(datas[i].normal);
		hits[i].Point = FromPxVec3(datas[i].position);
	}

	return hits;
}

bool PhysicsQuery::OverlapGeometryTest(const PxGeometryHolder& g, const V3& p, const Q& r, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxOverlapHit data;

	PxTransform t(ToPxVec3(p), ToPxQuat(r));

	return PxSceneQueryExt::overlapAny(*m_scene, g.any(), t, data, m_testFilter, &callback);
	//return m_scene->overlap(g.any(), t, buffer, m_testFilter, &callback);
}

Collider* PhysicsQuery::OverlapGeometry(const PxGeometryHolder& g, const V3& p, const Q& r, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxOverlapHit data;

	PxTransform t(ToPxVec3(p), ToPxQuat(r));

	//bool result = m_scene->overlap(g.any(), t, buffer, m_queryFilter, &callback);
	bool result = PxSceneQueryExt::overlapAny(*m_scene, g.any(), t, data, m_queryFilter, &callback);
	if (!result) return nullptr;

	return (Collider*)data.shape->userData;
}

vector<Collider*> PhysicsQuery::OverlapGeometryAll(const PxGeometryHolder& g, const V3& p, const Q& r, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = false;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxOverlapHit datas[HIT_MAX];
	PxOverlapBuffer buffer(datas, HIT_MAX);

	PxTransform t(ToPxVec3(p), ToPxQuat(r));

	m_scene->overlap(g.any(), t, buffer, m_queryFilter, &callback);

	uint numHits = buffer.getNbAnyHits();
	vector<Collider*> hits;
	hits.resize(numHits);

	for (uint i = 0; i < numHits; ++i)
	{
		hits[i] = (Collider*)datas[i].shape->userData;
	}

	return hits;
}

bool PhysicsQuery::OverlapTest(Collider* c, uint32 layermask, PhysicsQueryType type) const
{
	ICollider* ic = c;
	PxShape* shape = ic->GetPxShape();
	return OverlapGeometryTest(shape->getGeometry(), c->transform->position, c->transform->rotation, layermask, type);
}

Collider* PhysicsQuery::Overlap(Collider* c, uint32 layermask, PhysicsQueryType type) const
{
	ICollider* ic = c;
	PxShape* shape = ic->GetPxShape();
	return OverlapGeometry(shape->getGeometry(), c->transform->position, c->transform->rotation, layermask, type);
}

vector<Collider*> PhysicsQuery::OverlapAll(Collider* c, uint32 layermask, PhysicsQueryType type) const
{
	ICollider* ic = c;
	PxShape* shape = ic->GetPxShape();
	return OverlapGeometryAll(shape->getGeometry(), c->transform->position, c->transform->rotation, layermask, type);
}

bool PhysicsQuery::OverlapPointTest(const V3& p, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(0.001f);
	return OverlapGeometryTest(g, p, m_qI, layermask, type, ignoreRigidbody);
}

Collider* PhysicsQuery::OverlapPoint(const V3& p, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(0.001f);
	return OverlapGeometry(g, p, m_qI, layermask, type, ignoreRigidbody);
}

vector<Collider*> PhysicsQuery::OverlapPointAll(const V3& p, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(0.001f);
	return OverlapGeometryAll(g, p, m_qI, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::OverlapSphereTest(const V3& p, float r, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(r);
	return OverlapGeometryTest(g, p, m_qI, layermask, type, ignoreRigidbody);
}

Collider* PhysicsQuery::OverlapSphere(const V3& p, float r, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(r);
	return OverlapGeometry(g, p, m_qI, layermask, type, ignoreRigidbody);
}

vector<Collider*> PhysicsQuery::OverlapSphereAll(const V3& p, float r, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(r);
	return OverlapGeometryAll(g, p, m_qI, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::OverlapBoxTest(const V3& p, const Q& r, const V3& ex, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxBoxGeometry g(ToPxVec3(ex));
	return OverlapGeometryTest(g, p, r, layermask, type, ignoreRigidbody);
}

Collider* PhysicsQuery::OverlapBox(const V3& p, const Q& r, const V3& ex, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxBoxGeometry g(ToPxVec3(ex));
	return OverlapGeometry(g, p, r, layermask, type, ignoreRigidbody);
}

vector<Collider*> PhysicsQuery::OverlapBoxAll(const V3& p, const Q& r, const V3& ex, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxBoxGeometry g(ToPxVec3(ex));
	return OverlapGeometryAll(g, p, r, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::OverlapCapsuleTest(const V3& p, const Q& r, float rad, float halfHeight, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxCapsuleGeometry g(rad, halfHeight);
	return OverlapGeometryTest(g, p, m_qZ180 * r, layermask, type, ignoreRigidbody);
}

Collider* PhysicsQuery::OverlapCapsule(const V3& p, const Q& r, float rad, float halfHeight, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxCapsuleGeometry g(rad, halfHeight);
	return OverlapGeometry(g, p, m_qZ180 * r, layermask, type, ignoreRigidbody);
}

vector<Collider*> PhysicsQuery::OverlapCapsuleAll(const V3& p, const Q& r, float rad, float halfHeight, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxCapsuleGeometry g(rad, halfHeight);
	return OverlapGeometryAll(g, p, m_qZ180 * r, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::SweepGeometryTest(const PxGeometryHolder& g, const V3& p, const Q& r, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxSweepBuffer buffer;

	PxTransform t(ToPxVec3(p), ToPxQuat(r));

	return m_scene->sweep(g.any(), t, ToPxVec3(dir), length, buffer, m_flags, m_testFilter, &callback);
}

bool PhysicsQuery::SweepGeometry(PhysicsHit& hit, const PxGeometryHolder& g, const V3& p, const Q& r, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = true;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxSweepBuffer buffer;

	PxTransform t(ToPxVec3(p), ToPxQuat(r));

	bool result = m_scene->sweep(g.any(), t, ToPxVec3(dir), length, buffer, m_flags, m_queryFilter, &callback);
	if (!result) return false;

	const PxSweepHit& data = buffer.getAnyHit(0);
	hit.Collider = (Collider*)data.shape;
	hit.Distance = data.distance;
	hit.Normal = FromPxVec3(data.normal);
	hit.Point = FromPxVec3(data.position);
	return true;
}

vector<PhysicsHit> PhysicsQuery::SweepGeometryAll(const PxGeometryHolder& g, const V3& p, const Q& r, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PhysicsQueryFilterCallbackDesc desc;
	desc.LayerBitmask = layermask;
	desc.Type = type;
	desc.QueryOnce = false;
	desc.IgnoreRigidbody = ignoreRigidbody;
	PhysicsQueryFilterCallback callback(desc);
	PxSweepHit datas[HIT_MAX];
	PxSweepBuffer buffer(datas, HIT_MAX);

	PxTransform t(ToPxVec3(p), ToPxQuat(r));

	m_scene->sweep(g.any(), t, ToPxVec3(dir), length, buffer, m_flags, m_queryFilter, &callback);

	uint numHits = buffer.getNbAnyHits();
	vector<PhysicsHit> hits;
	hits.resize(numHits);

	for (uint i = 0; i < numHits; ++i)
	{
		hits[i].Collider = (Collider*)datas[i].actor;
		hits[i].Distance = datas[i].distance;
		hits[i].Normal = FromPxVec3(datas[i].normal);
		hits[i].Point = FromPxVec3(datas[i].position);
	}

	return hits;
}

bool PhysicsQuery::SweepTest(Collider* c, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	ICollider* ic = c;
	PxShape* shape = ic->GetPxShape();
	return SweepGeometryTest(shape->getGeometry(), c->transform->position, c->transform->rotation, dir, length, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::Sweep(PhysicsHit& hit, Collider* c, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	ICollider* ic = c;
	PxShape* shape = ic->GetPxShape();
	return SweepGeometry(hit, shape->getGeometry(), c->transform->position, c->transform->rotation, dir, length, layermask, type, ignoreRigidbody);
}

vector<PhysicsHit> PhysicsQuery::SweepAll(Collider* c, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	ICollider* ic = c;
	PxShape* shape = ic->GetPxShape();
	return SweepGeometryAll(shape->getGeometry(), c->transform->position, c->transform->rotation, dir, length, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::SweepSphereTest(const V3& p, float r, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(r);
	return SweepGeometryTest(g, p, m_qI, dir, length, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::SweepSphere(PhysicsHit& hit, const V3& p, float r, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(r);
	return SweepGeometry(hit, g, p, m_qI, dir, length, layermask, type, ignoreRigidbody);
}

vector<PhysicsHit> PhysicsQuery::SweepSphereAll(const V3& p, float r, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxSphereGeometry g(r);
	return SweepGeometryAll(g, p, m_qI, dir, length, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::SweepBoxTest(const V3& p, const Q& r, const V3& ex, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxBoxGeometry g(ToPxVec3(ex));
	return SweepGeometryTest(g, p, r, dir, length, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::SweepBox(PhysicsHit& hit, const V3& p, const Q& r, const V3& ex, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxBoxGeometry g(ToPxVec3(ex));
	return SweepGeometry(hit, g, p, r, dir, length, layermask, type, ignoreRigidbody);
}

vector<PhysicsHit> PhysicsQuery::SweepBoxAll(const V3& p, const Q& r, const V3& ex, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxBoxGeometry g(ToPxVec3(ex));
	return SweepGeometryAll(g, p, r, dir, length, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::SweepCapsuleTest(const V3& p, const Q& r, float rad, float halfHeight, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxCapsuleGeometry g(rad, halfHeight);
	return SweepGeometryTest(g, p, m_qZ180 * r, dir, length, layermask, type, ignoreRigidbody);
}

bool PhysicsQuery::SweepCapsule(PhysicsHit& hit, const V3& p, const Q& r, float rad, float halfHeight, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxCapsuleGeometry g(rad, halfHeight);
	return SweepGeometry(hit, g, p, m_qZ180 * r, dir, length, layermask, type, ignoreRigidbody);
}

vector<PhysicsHit> PhysicsQuery::SweepCapsuleAll(const V3& p, const Q& r, float rad, float halfHeight, const V3& dir, float length, uint32 layermask, PhysicsQueryType type, Rigidbody* ignoreRigidbody) const
{
	PxCapsuleGeometry g(rad, halfHeight);
	return SweepGeometryAll(g, p, m_qZ180 * r, dir, length, layermask, type, ignoreRigidbody);
}
