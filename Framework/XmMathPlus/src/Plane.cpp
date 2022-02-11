#include "XmMathPlusPCH.h"
#include "Plane.h"
#include "V3.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

XmMathPlus::Plane::Plane(const Real3& inNormal, const Real3& inPoint)
{
	// n * (x - p) = 0
	// x * n = p * n
	// NxX + NyY + NzZ = PxNx + PyNy + PzNz
	// ax + by + cz + d = 0

	v = DirectX::XMPlaneFromPointNormal(inPoint, inNormal);

	//const V3& n = V3(inNormal).normalized;
	//const V3& p = inPoint;
	//v.x = n.x;
	//v.y = n.y;
	//v.z = n.z;
	//v.w = -V3::Dot(n, p);
	//Normalize();
}

XmMathPlus::Plane::Plane(const Real3& inNormal, Real d)
{
	const V3& n = inNormal;
	v.x = n.x;
	v.y = n.y;
	v.z = n.z;
	v.w = d;
	Normalize();
}

XmMathPlus::Plane::Plane(const Real3& a, const Real3& b, const Real3& c)
{
	// 2차원 평면에서 a, b, c가 시계 방향이면 노멀의 y는 양의 값입니다.

	v = DirectX::XMPlaneFromPoints(a, b, c);

	//V3 ab = b - a;
	//V3 bc = c - b;
	//V3 n = V3::Cross(ab, bc).normalized;
	//V3 p = a;
	//v.x = n.x;
	//v.y = n.y;
	//v.z = n.z;
	//v.w = -V3::Dot(n, p);
	//Normalize();
}

Real3 XmMathPlus::Plane::GetNormal() const
{
	return Real3(v.xmv);
}

void XmMathPlus::Plane::SetNormal(const Real3& normal)
{
	v.x = normal.x;
	v.y = normal.y;
	v.z = normal.z;
}

Real XmMathPlus::Plane::GetDistanceToPoint(const Real3& point) const
{
	return V3::Dot(normal, point) + v.w;
}

Real XmMathPlus::Plane::GetSide(const Real3& point) const
{
	return V3::Dot(normal, point) + v.w > 0.0F;
}

bool XmMathPlus::Plane::Raycast(const Ray& ray, Real& out_distance) const
{
	// 평면의 노멀과 평면상의 한 점의 내적은 평면이 원점에서 떨어진 거리 d 입니다.
	// https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
	
	if (ray.direction.sqrMagnitude < Epsilon)
	{
		if (GetDistanceToPoint(ray.origin) < Epsilon)
			return true;
		else
			return false;
	}

	V3 n = normal;

	if (n.sqrMagnitude < Epsilon)
		return false;

	Real nDotd = V3::Dot(n, ray.direction);

	Real top = -(d + V3::Dot(n, ray.origin));

	// out_distance가 음수라면 광선 역방향에 교점이 생성됩니다.
	out_distance = top / nDotd;

	return true;
}

bool XmMathPlus::Plane::RaycastIn90Degree(const Ray& ray, Real& out_distance) const
{
	// 평면의 노멀과 평면상의 한 점의 내적은 평면이 원점에서 떨어진 거리 d 입니다.
	// https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
	
	const V3& dir = ray.direction;

	if (dir.sqrMagnitude < Epsilon)
	{
		if (GetDistanceToPoint(ray.origin) < Epsilon)
			return true;
		else
			return false;
	}

	const V3& n = normal;

	if (n.sqrMagnitude < Epsilon)
		return false;

	Real nDotd = V3::Dot(n, dir);

	// 평면의 법선과 광선의 각도가 90도보다 크면 종료합니다.
	if (nDotd > Epsilon)
		return false;
	
	Real top = -(d + V3::Dot(n, ray.origin));

	// out_distance가 음수라면 광선 역방향에 교점이 생성됩니다.
	out_distance = top / nDotd;

	return true;
}

bool XmMathPlus::Plane::Intersect(const V3& a, const V3& b, V3& out_hit) const
{
	out_hit = DirectX::XMPlaneIntersectLine(v, a, b);
	if (DirectX::XMVector4Equal(out_hit, g_XMQNaN.operator DirectX::XMVECTOR()))
		return false;
	return true;
}

bool XmMathPlus::Plane::SameSide(const Real3& inPt0, const Real3& inPt1) const
{
	Real d0 = GetDistanceToPoint(inPt0);
	Real d1 = GetDistanceToPoint(inPt1);
	return (d0 > 0.0f && d1 > 0.0f) ||
		(d0 <= 0.0f && d1 <= 0.0f);
}

Real3 XmMathPlus::Plane::ClosetPoint(const Real3& point) const
{
	const V3& n = normal;

	Real pointToPlaneDistance = V3::Dot(n, point) + d;
	return point - (n * pointToPlaneDistance);
}

void XmMathPlus::Plane::Transform(const V3& scale, const Q& rotation, const V3& position)
{
	M4 transformation = M4::SRT(scale, rotation, position);

	v = DirectX::XMPlaneTransform(v, transformation);
}

void XmMathPlus::Plane::Transform(const M4& transformation)
{
	v = DirectX::XMPlaneTransform(v, transformation);
}

Plane XmMathPlus::Plane::Flipped() const
{
	return Plane(-v.x, -v.y, -v.z, -v.w);
}

void XmMathPlus::Plane::Flip()
{
	*this = Flipped();
}

Plane XmMathPlus::Plane::Normalized() const
{
	Real m = DirectX::XMVector3Length(v.xmv).m128_f32[0];

	if (m == 0)
	{
		return Plane(0, 0, 0, 0);
	}
	else
	{
		Real4 np = v / m;
		return Plane(np.x, np.y, np.z, np.w);
	}
}

void XmMathPlus::Plane::Normalize()
{
	*this = Normalized();
}
