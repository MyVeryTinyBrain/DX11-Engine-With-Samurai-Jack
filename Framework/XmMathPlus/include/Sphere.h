#pragma once

#include "Plane.h"
#include "Q.h"
#include "M4.h"
#include "Triangle.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API Sphere
	{
		inline Sphere() : m_sphere() {}
		inline Sphere(const Sphere& other) : m_sphere(other.m_sphere) {}
		inline Sphere(const DirectX::BoundingSphere& sphere) : m_sphere(sphere) {}
		inline Sphere(const V3& Center, Real Radius) : m_sphere(Center, Ex::Abs(Radius)) {}

		inline operator const DirectX::BoundingSphere& () const { return m_sphere; }

		inline const V3& GetCenter() const { return m_sphere.Center; }
		inline void SetCenter(const V3& center) { m_sphere.Center = center; }
		inline Real GetRadius() const { return m_sphere.Radius; }
		inline void SetRadius(Real Radius) { m_sphere.Radius = Ex::Abs(Radius); }

		_declspec(property(get = GetCenter, put = SetCenter)) V3 center;
		_declspec(property(get = GetExtents, put = SetExtents)) V3 extents;

		inline void Transform(Real scale, const Q& rotation, const V3& position) { m_sphere.Transform(m_sphere, scale, rotation, position); }
		inline void Transform(const M4& transformation) { m_sphere.Transform(m_sphere, transformation); }

		inline bool Raycast(const Ray& ray, Real& out_distance) const { return m_sphere.Intersects(ray.origin, ray.direction, out_distance); }

		ContainmentType Contains(const V3& point) const { return m_sphere.Contains(point); }
		ContainmentType Contains(const V3& p0, const V3& p1, const V3& p2) const { return m_sphere.Contains(p0, p1, p2); }
		ContainmentType Contains(const BoundingSphere& sh) const { return m_sphere.Contains(sh); }
		ContainmentType Contains(const BoundingBox& box) const { return m_sphere.Contains(box); }
		ContainmentType Contains(const BoundingOrientedBox& box) const { return m_sphere.Contains(box); }
		ContainmentType Contains(const BoundingFrustum& fr) const { return m_sphere.Contains(fr); }

		bool Intersects(const BoundingSphere& sh) const { return m_sphere.Intersects(sh); }
		bool Intersects(const BoundingBox& box) const { return m_sphere.Intersects(box); }
		bool Intersects(const BoundingOrientedBox& box) const { return m_sphere.Intersects(box); }
		bool Intersects(const BoundingFrustum& fr) const { return m_sphere.Intersects(fr); }
		bool Intersects(const Triangle& tr) const { return m_sphere.Intersects(tr.p0, tr.p1, tr.p2); }

	private:

		DirectX::BoundingSphere m_sphere;
	};
}

