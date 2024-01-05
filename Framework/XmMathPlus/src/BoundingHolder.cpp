#include "XmMathPlusPCH.h"
#include "BoundingHolder.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

XmMathPlus::BoundingHolder::BoundingHolder(const BoundingHolder& other)
{
	Clear();

	switch (other.m_type)
	{
		case BoundingHolder::Type::AABB:
			m_aabb = other.m_aabb;
			break;
		case BoundingHolder::Type::OBB:
			m_obb = other.m_obb;
			break;
		case BoundingHolder::Type::Sphere:
			m_sphere = other.m_sphere;
			break;
		case BoundingHolder::Type::Frustum:
			m_frustum = other.m_frustum;
			break;
	}

	m_type = other.m_type;
}

void XmMathPlus::BoundingHolder::Clear()
{
	m_type = BoundingHolder::Type::None; 
	m_aabb = DirectX::BoundingBox();
	m_obb = DirectX::BoundingOrientedBox();
	m_sphere = DirectX::BoundingSphere();
	m_frustum = DirectX::BoundingFrustum();
}

bool XmMathPlus::BoundingHolder::Raycast(const Ray& ray, Real& out_distance) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Intersects(ray.origin, ray.direction, out_distance);
		case BoundingHolder::Type::OBB:
			return m_obb.Intersects(ray.origin, ray.direction, out_distance);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Intersects(ray.origin, ray.direction, out_distance);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Intersects(ray.origin, ray.direction, out_distance);
	}

	return false;
}

ContainmentType XmMathPlus::BoundingHolder::ContainsPoint(const V3& point) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Contains(point);
		case BoundingHolder::Type::OBB:
			return m_obb.Contains(point);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Contains(point);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Contains(point);
	}

	return ContainmentType::DISJOINT;
}

ContainmentType XmMathPlus::BoundingHolder::ContainsBoundingHolder(const BoundingHolder& bh) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_aabb.Contains(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_aabb.Contains(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_aabb.Contains(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_aabb.Contains(bh.m_frustum);
			}
			break;
		case BoundingHolder::Type::OBB:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_obb.Contains(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_obb.Contains(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_obb.Contains(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_obb.Contains(bh.m_frustum);
			}
			break;
		case BoundingHolder::Type::Sphere:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_sphere.Contains(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_sphere.Contains(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_sphere.Contains(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_sphere.Contains(bh.m_frustum);
			}
			break;
		case BoundingHolder::Type::Frustum:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_frustum.Contains(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_frustum.Contains(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_frustum.Contains(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_frustum.Contains(bh.m_frustum);
			}
			break;
	}

	return ContainmentType::DISJOINT;
}

ContainmentType XmMathPlus::BoundingHolder::Contains(const Triangle& tr) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Contains(tr.p0, tr.p1, tr.p2);
		case BoundingHolder::Type::OBB:
			return m_obb.Contains(tr.p0, tr.p1, tr.p2);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Contains(tr.p0, tr.p1, tr.p2);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Contains(tr.p0, tr.p1, tr.p2);
	}

	return ContainmentType::DISJOINT;
}

ContainmentType XmMathPlus::BoundingHolder::Contains(const BoundingSphere& sh) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Contains(sh);
		case BoundingHolder::Type::OBB:
			return m_obb.Contains(sh);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Contains(sh);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Contains(sh);
	}

	return ContainmentType::DISJOINT;
}

ContainmentType XmMathPlus::BoundingHolder::Contains(const BoundingBox& box) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Contains(box);
		case  BoundingHolder::Type::OBB:
			return m_obb.Contains(box);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Contains(box);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Contains(box);
	}

	return ContainmentType::DISJOINT;
}

ContainmentType XmMathPlus::BoundingHolder::Contains(const BoundingOrientedBox& box) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Contains(box);
		case BoundingHolder::Type::OBB:
			return m_obb.Contains(box);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Contains(box);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Contains(box);
	}

	return ContainmentType::DISJOINT;
}

ContainmentType XmMathPlus::BoundingHolder::Contains(const BoundingFrustum& fr) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Contains(fr);
		case BoundingHolder::Type::OBB:
			return m_obb.Contains(fr);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Contains(fr);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Contains(fr);
	}

	return ContainmentType::DISJOINT;
}

bool XmMathPlus::BoundingHolder::IntersectsBoundingHolder(const BoundingHolder& bh) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_aabb.Intersects(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_aabb.Intersects(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_aabb.Intersects(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_aabb.Intersects(bh.m_frustum);
			}
			break;
		case BoundingHolder::Type::OBB:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_obb.Intersects(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_obb.Intersects(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_obb.Intersects(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_obb.Intersects(bh.m_frustum);
			}
			break;
		case BoundingHolder::Type::Sphere:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_sphere.Intersects(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_sphere.Intersects(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_sphere.Intersects(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_sphere.Intersects(bh.m_frustum);
			}
			break;
		case BoundingHolder::Type::Frustum:
			switch (bh.m_type)
			{
				case BoundingHolder::Type::AABB:
					return m_frustum.Intersects(bh.m_aabb);
				case BoundingHolder::Type::OBB:
					return m_frustum.Intersects(bh.m_obb);
				case BoundingHolder::Type::Sphere:
					return m_frustum.Intersects(bh.m_sphere);
				case BoundingHolder::Type::Frustum:
					return m_frustum.Intersects(bh.m_frustum);
			}
			break;
	}
	return false;
}

bool XmMathPlus::BoundingHolder::Intersects(const BoundingSphere& sh) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Intersects(sh);
		case BoundingHolder::Type::OBB:
			return m_obb.Intersects(sh);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Intersects(sh);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Intersects(sh);
	}

	return false;
}

bool XmMathPlus::BoundingHolder::Intersects(const BoundingBox& box) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Intersects(box);
		case BoundingHolder::Type::OBB:
			return m_obb.Intersects(box);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Intersects(box);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Intersects(box);
	}

	return false;
}

bool XmMathPlus::BoundingHolder::Intersects(const BoundingOrientedBox& box) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Intersects(box);
		case BoundingHolder::Type::OBB:
			return m_obb.Intersects(box);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Intersects(box);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Intersects(box);
	}

	return false;
}

bool XmMathPlus::BoundingHolder::Intersects(const BoundingFrustum& fr) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Intersects(fr);
		case BoundingHolder::Type::OBB:
			return m_obb.Intersects(fr);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Intersects(fr);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Intersects(fr);
	}

	return false;
}

bool XmMathPlus::BoundingHolder::Intersects(const Triangle& tr) const
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			return m_aabb.Intersects(tr.p0, tr.p1, tr.p2);
		case BoundingHolder::Type::OBB:
			return m_obb.Intersects(tr.p0, tr.p1, tr.p2);
		case BoundingHolder::Type::Sphere:
			return m_sphere.Intersects(tr.p0, tr.p1, tr.p2);
		case BoundingHolder::Type::Frustum:
			return m_frustum.Intersects(tr.p0, tr.p1, tr.p2);
	}

	return false;
}

void XmMathPlus::BoundingHolder::Transform(Real scale, const Q& rotation, const V3& position)
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			m_aabb.Transform(m_aabb, scale, rotation, position);
		case BoundingHolder::Type::OBB:
			m_obb.Transform(m_obb, scale, rotation, position);
		case BoundingHolder::Type::Sphere:
			m_sphere.Transform(m_sphere, scale, rotation, position);
		case BoundingHolder::Type::Frustum:
			m_frustum.Transform(m_frustum, scale, rotation, position);
	}
}

void XmMathPlus::BoundingHolder::Transform(const M4& transformation)
{
	switch (m_type)
	{
		case BoundingHolder::Type::AABB:
			m_aabb.Transform(m_aabb, transformation);
		case BoundingHolder::Type::OBB:
			m_obb.Transform(m_obb, transformation);
		case BoundingHolder::Type::Sphere:
			m_sphere.Transform(m_sphere, transformation);
		case BoundingHolder::Type::Frustum:
			m_frustum.Transform(m_frustum, transformation);
	}
}

bool XmMathPlus::BoundingHolder::IsSameType(const BoundingSphere& sh) const
{
	return m_type == BoundingHolder::Type::Sphere;
}

bool XmMathPlus::BoundingHolder::IsSameType(const BoundingBox& box) const
{
	return m_type == BoundingHolder::Type::AABB;
}

bool XmMathPlus::BoundingHolder::IsSameType(const BoundingOrientedBox& box) const
{
	return m_type == BoundingHolder::Type::OBB;
}

bool XmMathPlus::BoundingHolder::IsSameType(const BoundingFrustum& fr) const
{
	return m_type == BoundingHolder::Type::Frustum;
}
