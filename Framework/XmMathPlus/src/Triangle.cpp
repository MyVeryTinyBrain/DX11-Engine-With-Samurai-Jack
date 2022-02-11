#include "XmMathPlusPCH.h"
#include "Triangle.h"
#include "Plane.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

V3 XmMathPlus::Triangle::GetNormal() const
{
	V3 ab = p1 - p0;
	V3 bc = p2 - p1;

	return V3::Cross(ab, bc).normalized;
}

V3 XmMathPlus::Triangle::GetCenter() const
{
	return (p0 + p1 + p2) / 3.0f;
}

Real4 XmMathPlus::Triangle::ToPlane() const
{
	Plane plane = Plane(p0, p1, p2);
	return plane.v;
}

Real XmMathPlus::Triangle::GetAngleOfP0() const
{
	V3 p0p1 = p1 - p0;
	V3 p0p2 = p2 - p0;
	return V3::Angle(p0p1, p0p2);
}

Real XmMathPlus::Triangle::GetAngleOfP1() const
{
	V3 p1p0 = p0 - p1;
	V3 p1p2 = p2 - p1;
	return V3::Angle(p1p0, p1p2);
}

Real XmMathPlus::Triangle::GetAngleOfP2() const
{
	V3 p2p0 = p0 - p2;
	V3 p2p1 = p1 - p2;
	return V3::Angle(p2p0, p2p1);
}

bool XmMathPlus::Triangle::ContainsInSweep(const V3& point) const
{
	V3 _p_0 = p0 - point;
	V3 _0_1 = p1 - p0;
	V3 c0 = V3::Cross(_p_0, _0_1);

	V3 _p_1 = p1 - point;
	V3 _1_2 = p2 - p1;
	V3 c1 = V3::Cross(_p_1, _1_2);

	float d01 = V3::Dot(c0, c1);

	V3 _p_2 = p2 - point;
	V3 _2_0 = p0 - p2;
	V3 c2 = V3::Cross(_p_2, _2_0);

	float d02 = V3::Dot(c0, c2);
	float d12 = V3::Dot(c1, c2);

	if (Ex::Sign(d01) != Ex::Sign(d02))
		return false;

	if (Ex::Sign(d01) != Ex::Sign(d12))
		return false;

	if (Ex::Sign(d02) != Ex::Sign(d12))
		return false;

	return true;
}

bool XmMathPlus::Triangle::Raycast(const Ray& ray, Real& out_distance) const
{
	if (DirectX::TriangleTests::Intersects(ray.origin, ray.direction, p0, p1, p2, out_distance))
	{
		// out_distance가 음수라면 광선 역방향에 교점이 생성됩니다.
		return true;
	}
	return false;

	//Plane plane = ToPlane();
	//Real d;

	//if (plane.Raycast(ray, d))
	//{
	//	V3 hit = ray.GetPoint(d);
	//	if (Contains(hit))
	//		return true;
	//}

	//return false;
}

bool XmMathPlus::Triangle::RaycastIn90Degrees(const Ray& ray, Real& out_distance) const
{
	if (DirectX::TriangleTests::Intersects(ray.origin, ray.direction, p0, p1, p2, out_distance))
	{
		// 평면의 법선과 광선의 각도가 90도보다 크면 종료합니다.
		if (V3::Dot(GetNormal(), ray.direction) > 0)
			return false;

		// out_distance가 음수라면 광선 역방향에 교점이 생성됩니다.
		return true;
	}
	return false;
}

void XmMathPlus::Triangle::Transform(const V3& scale, const Q& rotation, const V3& position)
{
	M4 transformation = M4::SRT(scale, rotation, position);

	p0 = p0 * transformation;
	p1 = p1 * transformation;
	p2 = p2 * transformation;
}

void XmMathPlus::Triangle::Transform(const M4& transformation)
{
	p0 = p0 * transformation;
	p1 = p1 * transformation;
	p2 = p2 * transformation;
}
