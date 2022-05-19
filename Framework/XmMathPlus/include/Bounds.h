#pragma once

#include "Plane.h"
#include "Q.h"
#include "M4.h"
#include "Triangle.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API Bounds
	{
		inline Bounds() : m_aabb() {}
		inline Bounds(const Bounds& other) : m_aabb(other.m_aabb) {}
		Bounds(const M4& orthographicMatrix);
		inline Bounds(const DirectX::BoundingBox& aabb) : m_aabb(aabb) {}
		inline Bounds(const V3& Center, const V3& Extents) : m_aabb(Center, V3::Abs(Extents)) {}

		inline operator const DirectX::BoundingBox& () const { return m_aabb; }

		inline V3 GetCenter() const { return m_aabb.Center; }
		inline V3 GetExtents() const { return m_aabb.Extents; }
		inline void SetCenter(const V3& center) { m_aabb.Center = center; }
		inline void SetExtents(const V3& extents) { m_aabb.Extents = V3::Abs(extents); }

		inline V3 GetMax() const { return center + extents; }
		inline V3 GetMin() const { return center - extents; }
		inline V3 GetSize() const { return GetExtents() * 2.0f; }

		void GetCorners(V3* out_arrCorners);

		_declspec(property(get = GetCenter, put = SetCenter)) V3 center;
		_declspec(property(get = GetExtents, put = SetExtents)) V3 extents;
		_declspec(property(get = GetSize)) V3 size;
		_declspec(property(get = GetMin)) V3 min;
		_declspec(property(get = GetMax)) V3 max;

		V3 ClosetPoint(V3 point) const;
		Real GetDistanceBetweenPoint(const V3& point) const;

		inline void Transform(Real scale, const Q& rotation, const V3& position) { m_aabb.Transform(m_aabb, scale, rotation, position); }
		inline void Transform(const M4& transformation) { m_aabb.Transform(m_aabb, transformation); }

		inline bool Raycast(const Ray& ray, Real& out_distance) const { return m_aabb.Intersects(ray.origin, ray.direction, out_distance); }
		bool RaycastIn90Degrees(const Ray& ray, Real& out_distance) const;

		ContainmentType Contains(const V3& point) const { return m_aabb.Contains(point); }
		ContainmentType Contains(const V3& p0, const V3& p1, const V3& p2) const { return m_aabb.Contains(p0, p1, p2); }
		ContainmentType Contains(const BoundingSphere& sh) const { return m_aabb.Contains(sh); }
		ContainmentType Contains(const BoundingBox& box) const { return m_aabb.Contains(box); }
		ContainmentType Contains(const BoundingOrientedBox& box) const { return m_aabb.Contains(box); }
		ContainmentType Contains(const BoundingFrustum& fr) const { return m_aabb.Contains(fr); }

		bool Intersects(const BoundingSphere& sh) const { return m_aabb.Intersects(sh); }
		bool Intersects(const BoundingBox& box) const { return m_aabb.Intersects(box); }
		bool Intersects(const BoundingOrientedBox& box) const { return m_aabb.Intersects(box); }
		bool Intersects(const BoundingFrustum& fr) const { return m_aabb.Intersects(fr); }
		bool Intersects(const Triangle& tr) const { return m_aabb.Intersects(tr.p0, tr.p1, tr.p2); }

	private:

		/*

			¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á    ¦³
			¡á         ¡á    | extents.y * 2.0 = size.y
			¡á         ¡á    |
			¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á    ¦º

			|---------|
			 extents.x * 2.0 = size.x

			 extents.x, extents.y, extents.z >= 0 

		*/

		DirectX::BoundingBox m_aabb;
	};
}

