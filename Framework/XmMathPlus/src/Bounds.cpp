#include "XmMathPlusPCH.h"
#include "Bounds.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

V3 XmMathPlus::Bounds::ClosetPoint(V3 point) const
{
	point -= center;

	V3 min(FLT_MAX, FLT_MAX, FLT_MAX);

	Plane right(1, 0, 0, extents.x);
	Plane left(-1, 0, 0, extents.x);
	Plane up(0, 1, 0, extents.y);
	Plane down(0, -1, 0, extents.y);
	Plane forward(0, 0, 1, extents.z);
	Plane back(0, 0, -1, extents.z);
	
	min = V3::Min(min, right.ClosetPoint(point));
	min = V3::Min(min, left.ClosetPoint(point));
	min = V3::Min(min, up.ClosetPoint(point));
	min = V3::Min(min, down.ClosetPoint(point));
	min = V3::Min(min, forward.ClosetPoint(point));
	min = V3::Min(min, back.ClosetPoint(point));

	return min + center;
}

Real XmMathPlus::Bounds::GetDistanceBetweenPoint(const V3& point) const
{
	Real dx = Max(min.x - point.x, point.x - max.x);
	Real dy = Max(min.y - point.y, point.y - max.y);
	dx = Max(dx, 0.0f);
	dy = Max(dy, 0.0f);
	Real distance = sqrt(dx * dx + dy * dy);
	return distance;
}

bool XmMathPlus::Bounds::RaycastIn90Degrees(const Ray& ray, Real& out_distance) const
{
	if (m_aabb.Intersects(ray.origin, ray.direction, out_distance))
	{
		V3 hit = ray.GetPoint(out_distance);
		V3 hitInLocal = V3::Scale(hit - center, extents); // x,y,z = [-1 ~ +1]
		V3 normalOfPlane = hitInLocal.normalized;
		
		// 평면의 법선과 광선의 각도가 90도보다 크면 종료합니다.
		if (V3::Dot(normalOfPlane, ray.direction) > 0)
			return false;
		else
			return true;
	}
	return false;
}

XmMathPlus::Bounds::Bounds(const M4& orthographicMatrix)
{
	float m00 = orthographicMatrix.m00;
	float m11 = orthographicMatrix.m11;
	float m22 = orthographicMatrix.m22;

	float width = 1.0f / m00 * 2.0f;
	float height = 1.0f / m11 * 2.0f;
	float depth = 1.0f / m22;

	V3 extents = V3(width, height, depth);

	m_aabb = DirectX::BoundingBox(V3::zero(), extents);
}

void XmMathPlus::Bounds::GetCorners(V3* out_arrCorners)
{
	XMFLOAT3 xmCorners[8];
	m_aabb.GetCorners(xmCorners);

	out_arrCorners[0] = xmCorners[0];
	out_arrCorners[1] = xmCorners[1];
	out_arrCorners[2] = xmCorners[2];
	out_arrCorners[3] = xmCorners[3];
	out_arrCorners[4] = xmCorners[4];
	out_arrCorners[5] = xmCorners[5];
	out_arrCorners[6] = xmCorners[6];
	out_arrCorners[7] = xmCorners[7];
}
