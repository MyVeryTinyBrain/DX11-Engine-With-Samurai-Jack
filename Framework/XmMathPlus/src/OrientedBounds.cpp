#include "XmMathPlusPCH.h"
#include "OrientedBounds.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

XmMathPlus::OrientedBounds::OrientedBounds(const M4& orthographicMatrix)
{
	float m00 = orthographicMatrix.m00;
	float m11 = orthographicMatrix.m11;
	float m22 = orthographicMatrix.m22;

	float width = 1.0f / m00 * 2.0f;
	float height = 1.0f / m11 * 2.0f;
	float depth = 1.0f / m22;
	
	V3 extents = V3(width, height, depth);

	m_obb = DirectX::BoundingOrientedBox(V3::zero(), extents, Q::identity());
}

void XmMathPlus::OrientedBounds::GetCorners(V3* out_arrCorners) const
{
	XMFLOAT3 xmCorners[8] = {};
	m_obb.GetCorners(xmCorners);

	out_arrCorners[0] = xmCorners[0];
	out_arrCorners[1] = xmCorners[1];
	out_arrCorners[2] = xmCorners[2];
	out_arrCorners[3] = xmCorners[3];
	out_arrCorners[4] = xmCorners[4];
	out_arrCorners[5] = xmCorners[5];
	out_arrCorners[6] = xmCorners[6];
	out_arrCorners[7] = xmCorners[7];
}

V3 XmMathPlus::OrientedBounds::ClosetPoint(V3 point) const
{
	point -= center;

	V3 min(FLT_MAX, FLT_MAX, FLT_MAX);

	Q rotation = m_obb.Orientation;

	Plane right(rotation.MultiplyVector(V3(1, 0, 0)), extents.x);
	Plane left(rotation.MultiplyVector(V3(-1, 0, 0)), extents.x);
	Plane up(rotation.MultiplyVector(V3(0, 1, 0)), extents.y);
	Plane down(rotation.MultiplyVector(V3(0, -1, 0)), extents.y);
	Plane forward(rotation.MultiplyVector(V3(0, 0, 1)), extents.z);
	Plane back(rotation.MultiplyVector(V3(0, 0, -1)), extents.z);

	min = V3::Min(min, right.ClosetPoint(point));
	min = V3::Min(min, left.ClosetPoint(point));
	min = V3::Min(min, up.ClosetPoint(point));
	min = V3::Min(min, down.ClosetPoint(point));
	min = V3::Min(min, forward.ClosetPoint(point));
	min = V3::Min(min, back.ClosetPoint(point));

	return min + center;
}

bool XmMathPlus::OrientedBounds::RaycastIn90Degrees(const Ray& ray, Real& out_distance) const
{
	if (m_obb.Intersects(ray.origin, ray.direction, out_distance))
	{
		V3 hit = ray.GetPoint(out_distance);
		V3 hitInLocal = hit;
		Q rotation = m_obb.Orientation;
		hitInLocal = rotation.inversed.MultiplyVector(rotation);
		hitInLocal = V3::Scale(hit - center, extents); // x,y,z = [-1 ~ +1]
		V3 normalOfPlane = hitInLocal.normalized;

		// 평면의 법선과 광선의 각도가 90도보다 크면 종료합니다.
		if (V3::Dot(normalOfPlane, ray.direction) > 0)
			return false;
		else
			return true;
	}
	return false;
}
