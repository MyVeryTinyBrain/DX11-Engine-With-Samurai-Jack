#pragma once

#include "Plane.h"
#include "Q.h"
#include "M4.h"
#include "Triangle.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API OrientedBounds
	{
		inline OrientedBounds() : m_obb() {}
		inline OrientedBounds(const OrientedBounds& other) : m_obb(other.m_obb) {}
		OrientedBounds(const M4& orthographicMatrix);
		inline OrientedBounds(const BoundingOrientedBox& other) : m_obb(m_obb) {}
		inline OrientedBounds(const V3& Center, const V3& Extents, const Q& Rotation) : m_obb(Center, V3::Abs(Extents), Rotation) {}

		inline operator const DirectX::BoundingOrientedBox& () const { return m_obb; }

		inline V3 GetCenter() const { return m_obb.Center; }
		inline V3 GetExtents() const { return m_obb.Extents; }
		inline void SetCenter(const V3& center) { m_obb.Center = center; }
		inline void SetExtents(const V3& extents) { m_obb.Extents = extents; }

		inline const V3& GetMin() const { return center - Q(m_obb.Orientation).MultiplyVector(extents); }
		inline const V3& GetMax() const { return center + Q(m_obb.Orientation).MultiplyVector(extents); }
		inline V3 GetSize() const { return GetExtents() * 2.0f; }
		
		void GetCorners(V3* out_arrCorners) const;

		_declspec(property(get = GetCenter, put = SetCenter)) V3 center;
		_declspec(property(get = GetExtents, put = SetExtents)) V3 extents;
		_declspec(property(get = GetSize)) V3 size;
		_declspec(property(get = GetMin)) V3 min;
		_declspec(property(get = GetMax)) V3 max;

		V3 ClosetPoint(V3 point) const;

		inline bool Raycast(const Ray& ray, Real& out_distance) const { return m_obb.Intersects(ray.origin, ray.direction, out_distance); }

		bool RaycastIn90Degrees(const Ray& ray, Real& out_distance) const;

		inline void Transform(Real scale, const Q& rotation, const V3& position) { m_obb.Transform(m_obb, scale, rotation, position); }
		inline void Transform(const M4& transformation) { m_obb.Transform(m_obb, transformation); }

		ContainmentType Contains(const V3& point) const { return m_obb.Contains(point); }
		ContainmentType Contains(const V3& p0, const V3& p1, const V3& p2) const { return m_obb.Contains(p0, p1, p2); }
		ContainmentType Contains(const BoundingSphere& sh) const { return m_obb.Contains(sh); }
		ContainmentType Contains(const BoundingBox& box) const { return m_obb.Contains(box); }
		ContainmentType Contains(const BoundingOrientedBox& box) const { return m_obb.Contains(box); }
		ContainmentType Contains(const BoundingFrustum& fr) const { return m_obb.Contains(fr); }

		bool Intersects(const BoundingSphere& sh) const { return m_obb.Intersects(sh); }
		bool Intersects(const BoundingBox& box) const { return m_obb.Intersects(box); }
		bool Intersects(const BoundingOrientedBox& box) const { return m_obb.Intersects(box); }
		bool Intersects(const BoundingFrustum& fr) const { return m_obb.Intersects(fr); }
		bool Intersects(const Triangle& tr) const { return m_obb.Intersects(tr.p0, tr.p1, tr.p2); }

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

		DirectX::BoundingOrientedBox m_obb;
	};
}

