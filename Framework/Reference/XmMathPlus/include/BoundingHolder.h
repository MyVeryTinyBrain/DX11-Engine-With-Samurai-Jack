#pragma once

#include "Ray.h"
#include "Triangle.h"
#include "Bounds.h"
#include "OrientedBounds.h"
#include "Sphere.h"
#include "Frustum.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API BoundingHolder
	{
		enum class Type { None, AABB, OBB, Sphere, Frustum };

		inline BoundingHolder() { Clear(); }
		inline BoundingHolder(const DirectX::BoundingBox& aabb) { Set(aabb); }
		inline BoundingHolder(const DirectX::BoundingOrientedBox& obb) { Set(obb); }
		inline BoundingHolder(const DirectX::BoundingSphere& sphere) { Set(sphere); }
		inline BoundingHolder(const DirectX::BoundingFrustum& frustum) { Set(frustum); }
		inline BoundingHolder(const Bounds& aabb) { Set(aabb); }
		inline BoundingHolder(const OrientedBounds& obb) { Set(obb); }
		inline BoundingHolder(const Sphere& sphere) { Set(sphere); }
		inline BoundingHolder(const Frustum& frustum) { Set(frustum); }
		BoundingHolder(const BoundingHolder& other);

		inline operator const DirectX::BoundingBox& () { return m_aabb; }
		inline operator const DirectX::BoundingOrientedBox& () { return m_obb; }
		inline operator const DirectX::BoundingSphere& () { return m_sphere; }
		inline operator const DirectX::BoundingFrustum& () { return m_frustum; }
		inline operator Bounds () { return m_aabb; }
		inline operator OrientedBounds () { return m_obb; }
		inline operator Sphere () { return m_sphere; }
		inline operator Frustum () { return m_frustum; }

		inline void Set(const DirectX::BoundingBox& aabb) { Clear();  m_type = BoundingHolder::Type::AABB; m_aabb = aabb; }
		inline void Set(const DirectX::BoundingOrientedBox& obb) { Clear();  m_type = BoundingHolder::Type::OBB; m_obb = obb; }
		inline void Set(const DirectX::BoundingSphere& sphere) { Clear(); m_type = BoundingHolder::Type::Sphere; m_sphere = sphere; }
		inline void Set(const DirectX::BoundingFrustum& frustum) { Clear();  m_type = BoundingHolder::Type::Frustum; m_frustum = frustum; }

		void Clear();

		BoundingHolder::Type GetType() const { return m_type; }

		bool Raycast(const Ray& ray, Real& out_distance) const;

		ContainmentType ContainsPoint(const V3& point) const;
		ContainmentType ContainsBoundingHolder(const BoundingHolder& bh) const;
		ContainmentType Contains(const Triangle& tr) const;
		ContainmentType Contains(const BoundingSphere& sh) const;
		ContainmentType Contains(const BoundingBox& box) const;
		ContainmentType Contains(const BoundingOrientedBox& box) const;
		ContainmentType Contains(const BoundingFrustum& fr) const;

		bool IntersectsBoundingHolder(const BoundingHolder& bh) const;
		bool Intersects(const BoundingSphere& sh) const;
		bool Intersects(const BoundingBox& box) const;
		bool Intersects(const BoundingOrientedBox& box) const;
		bool Intersects(const BoundingFrustum& fr) const;
		bool Intersects(const Triangle& tr) const;

		void Transform(Real scale, const Q& rotation, const V3& position);
		void Transform(const M4& transformation);

		bool IsSameType(const BoundingSphere& sh) const;
		bool IsSameType(const BoundingBox& box) const;
		bool IsSameType(const BoundingOrientedBox& box) const;
		bool IsSameType(const BoundingFrustum& fr) const;

	private:

		BoundingHolder::Type m_type;
		DirectX::BoundingBox m_aabb;
		DirectX::BoundingOrientedBox m_obb;
		DirectX::BoundingSphere m_sphere;
		DirectX::BoundingFrustum m_frustum;
	};
}

