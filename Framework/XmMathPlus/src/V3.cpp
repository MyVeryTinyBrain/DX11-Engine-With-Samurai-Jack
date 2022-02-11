#include "XmMathPlusPCH.h"
#include "V3.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

V3& XmMathPlus::V3::operator+=(const V3& rhs)
{
	return (V3&)Real3::operator+=(rhs);

	//x += rhs.x;
	//y += rhs.y;
	//z += rhs.z;
	//return *this;
}

V3& XmMathPlus::V3::operator-=(const V3& rhs)
{
	return (V3&)Real3::operator-=(rhs);

	//x -= rhs.x;
	//y -= rhs.y;
	//z -= rhs.z;
	//return *this;
}

V3& XmMathPlus::V3::operator*=(Real rhs)
{
	return (V3&)Real3::operator*=(rhs);

	//x *= rhs;
	//y *= rhs;
	//z *= rhs;
	//return *this;
}

V3& XmMathPlus::V3::operator/=(Real rhs)
{
	return (V3&)Real3::operator/=(rhs);

	//x /= rhs;
	//y /= rhs;
	//z /= rhs;
	//return *this;
}

V3 XmMathPlus::V3::operator-() const
{
	return Real3::operator-();

	//return V3(-x, -y, -z);
}

V3 XmMathPlus::V3::operator+() const
{
	return Real3::operator+();

	//return V3(+x, +y, +z);
}

V3 XmMathPlus::V3::operator+(const V3& rhs) const
{
	return Real3::operator+(rhs);

	//return V3(x + rhs.x, y + rhs.y, z + rhs.z);
}

V3 XmMathPlus::V3::operator-(const V3& rhs) const
{
	return Real3::operator-(rhs);

	//return V3(x - rhs.x, y - rhs.y, z - rhs.z);
}

V3 XmMathPlus::V3::operator*(Real rhs) const
{
	return Real3::operator*(rhs);

	//return V3(x * rhs, y * rhs, z * rhs);
}

V3 XmMathPlus::V3::operator/(Real rhs) const
{
	return Real3::operator/(rhs);

	//return V3(x / rhs, y / rhs, z / rhs);
}

bool XmMathPlus::V3::operator==(const V3& rhs) const
{
	return Real3::operator==(rhs);

	//return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool XmMathPlus::V3::operator!=(const V3& rhs) const
{
	return Real3::operator!=(rhs);

	//return x != rhs.x || y != rhs.y || z != rhs.z;
}

XmMathPlus::V3::operator Real2() const
{
	return Real2(xmv);
}

XmMathPlus::V3::operator Real3() const
{
	return Real3(xmv);
}

XmMathPlus::V3::operator Real4() const
{
	return Real4(xmv);
}

Real XmMathPlus::V3::SqrMagnitude() const
{
	return DirectX::XMVector3LengthSq(xmv).m128_f32[0];

	//return x * x + y * y + z * z;
}

Real XmMathPlus::V3::Magnitude() const
{
	return DirectX::XMVector3Length(xmv).m128_f32[0];

	//return sqrt(x * x + y * y + z * z);
}

V3 XmMathPlus::V3::Normalized() const
{
	return DirectX::XMVector3Normalize(xmv);

	//Real m = Magnitude();

	//if (m > 0)
	//{
	//	return *this / m;
	//}
	//else
	//{
	//	return V3(0, 0, 0);
	//}
}

void XmMathPlus::V3::Normalize()
{
	*this = DirectX::XMVector3Normalize(xmv);

	//Real m = Magnitude();

	//if (m > 0)
	//{
	//	*this /= m;
	//}
	//else
	//{
	//	x = 0;
	//	y = 0;
	//	z = 0;
	//}
}

V3 XmMathPlus::V3::Scale(const V3& a, const V3& b)
{
	return V3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Real XmMathPlus::V3::Dot(const V3& a, const V3& b)
{
	return DirectX::XMVector3Dot(a.xmv, b.xmv).m128_f32[0];

	//return a.x * b.x + a.y * b.y + a.z * b.z;
}

V3 XmMathPlus::V3::Cross(const V3& lhs, const V3& rhs)
{
	return DirectX::XMVector3Cross(lhs.xmv, rhs.xmv);

	//return V3(
	//	lhs.y * rhs.z - lhs.z * rhs.y,
	//	lhs.z * rhs.x - lhs.x * rhs.z,
	//	lhs.x * rhs.y - lhs.y * rhs.x);
}

V3 XmMathPlus::V3::Lerp(const V3& a, const V3& b, Real t)
{
	if (t < 0.0f)
	{
		t = 0.0f;
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
	}

	return V3::LerpUnclmaped(a, b, t);
}

V3 XmMathPlus::V3::LerpUnclmaped(const V3& a, const V3& b, Real t)
{
	return DirectX::XMVectorLerp(a.xmv, b.xmv, t);

	//V3 delta = b - a;
	//return a + delta * t;
}

V3 XmMathPlus::V3::Project(const V3& vector, const V3& onNormal)
{
	return Dot(onNormal, vector) / Dot(onNormal, onNormal) * onNormal;
}

V3 XmMathPlus::V3::Reflect(const V3& inDirection, const V3& inNormal)
{
	return inDirection + 2.0f * inNormal * Dot(-inDirection, inNormal);
}

V3 XmMathPlus::V3::Sliding(const V3& inDirection, const V3& inNormal)
{
	return inDirection - inNormal * V3::Dot(inDirection, inNormal);
}

Real XmMathPlus::V3::Cos(const V3& from, const V3& to)
{
	return Dot(from.Normalized(), to.Normalized());
}

Real XmMathPlus::V3::Radian(const V3& from, const V3& to)
{
	Real cos = V3::Cos(from, to);
	Real ab = sqrt(from.SqrMagnitude() * to.SqrMagnitude());

	if (ab < 0)
	{
		return 0;
	}
	else
	{
		Real theta = cos / ab;
		return Ex::Acos(theta);
	}
}

Real XmMathPlus::V3::Angle(const V3& from, const V3& to)
{
	return Radian(from, to) * Rad2Deg;
}

Real XmMathPlus::V3::SignedAngle(const V3& from, const V3& to, const V3& axis)
{
	Real angle = V3::Angle(from, to);

	Real cross_x = from.y * to.z - from.z * to.y;
	Real cross_y = from.z * to.x - from.x * to.z;
	Real cross_z = from.x * to.y - from.y * to.x;
	Real sign = Sign(axis.x * cross_x + axis.y * cross_y + axis.z * cross_z);

	return angle * sign;
}

Real XmMathPlus::V3::Distance(const V3& a, const V3& b)
{
	V3 delta = b - a;
	return delta.Magnitude();
}

Real XmMathPlus::V3::SqrDistance(const V3& a, const V3& b)
{
	V3 delta = b - a;
	return delta.SqrMagnitude();
}

V3 XmMathPlus::V3::Abs(const V3& v)
{
	return V3(Ex::Abs(v.x), Ex::Abs(v.y), Ex::Abs(v.z));
}

V3 XmMathPlus::V3::Min(const V3& a, const V3& b)
{
	return V3(Ex::Min(a.x, b.x), Ex::Min(a.y, b.y), Ex::Min(a.z, b.z));
}

V3 XmMathPlus::V3::Max(const V3& a, const V3& b)
{
	return V3(Ex::Max(a.x, b.x), Ex::Max(a.y, b.y), Ex::Max(a.z, b.z));
}

Real XmMathPlus::V3::MinElement(const V3& v)
{
	Real maxElement = v.x;
	maxElement = Ex::Min(maxElement, v.y);
	maxElement = Ex::Min(maxElement, v.z);
	return maxElement;
}

Real XmMathPlus::V3::MaxElement(const V3& v)
{
	Real maxElement = v.x;
	maxElement = Ex::Max(maxElement, v.y);
	maxElement = Ex::Max(maxElement, v.z);
	return maxElement;
}

Real XmMathPlus::V3::MinAbsElement(const V3& v)
{
	return MinElement(Abs(v));
}

Real XmMathPlus::V3::MaxAbsElement(const V3& v)
{
	return MaxElement(Abs(v));
}

V3 XmMathPlus::operator*(Real scale, const V3& vector)
{
	return vector * scale;
}
