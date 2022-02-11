#pragma once

#include "Plane.h"
#include "Q.h"
#include "M4.h"
#include "Triangle.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API Frustum
	{
		inline Frustum() : m_frustum() {}
		inline Frustum(const M4& projection) : m_frustum(projection) {}
		inline Frustum(const V3& origin, Q& rotation, Real rightSlope, Real leftSlope, Real topSlope, Real bottomSlope, Real Near, Real Far) : 
			m_frustum(origin, rotation, rightSlope, leftSlope, topSlope, bottomSlope, Near, Far) {}
		inline Frustum(const Frustum& other) : m_frustum(other.m_frustum) {}
		inline Frustum(const BoundingFrustum& frustum) : m_frustum(frustum) {}

		inline operator const DirectX::BoundingFrustum& () const { return m_frustum; }

		inline const V3& GetOrigin() const { return m_frustum.Origin; }
		inline void SetOrigin(const V3& origin) { m_frustum.Origin = origin; }

		void GetCorners(V3* out_arrCorners) const;

		_declspec(property(get = GetOrigin, put = SetOrigin)) V3 origin;

		inline void Transform(Real scale, const Q& rotation, const V3& position) { m_frustum.Transform(m_frustum, scale, rotation, position); }
		inline void Transform(const M4& transformation) { m_frustum.Transform(m_frustum, transformation); }

		inline bool Raycast(const Ray& ray, Real& out_distance) const { return m_frustum.Intersects(ray.origin, ray.direction, out_distance); }

		ContainmentType Contains(const V3& point) const { return m_frustum.Contains(point); }
		ContainmentType Contains(const V3& p0, const V3& p1, const V3& p2) const { return m_frustum.Contains(p0, p1, p2); }
		ContainmentType Contains(const BoundingSphere& sh) const { return m_frustum.Contains(sh); }
		ContainmentType Contains(const BoundingBox& box) const { return m_frustum.Contains(box); }
		ContainmentType Contains(const BoundingOrientedBox& box) const { return m_frustum.Contains(box); }
		ContainmentType Contains(const BoundingFrustum& fr) const { return m_frustum.Contains(fr); }

		bool Intersects(const BoundingSphere& sh) const { return m_frustum.Intersects(sh); }
		bool Intersects(const BoundingBox& box) const { return m_frustum.Intersects(box); }
		bool Intersects(const BoundingOrientedBox& box) const { return m_frustum.Intersects(box); }
		bool Intersects(const BoundingFrustum& fr) const { return m_frustum.Intersects(fr); }
		bool Intersects(const Triangle& tr) const { return m_frustum.Intersects(tr.p0, tr.p1, tr.p2); }

	private:

		DirectX::BoundingFrustum m_frustum;
	};
}

