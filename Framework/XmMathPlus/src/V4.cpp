#include "XmMathPlusPCH.h"
#include "V4.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

V4& XmMathPlus::V4::operator+=(const V4& rhs)
{
	return (V4&)Real4::operator+=(rhs);

	//x += rhs.x;
	//y += rhs.y;
	//z += rhs.z;
	//w += rhs.w;
	//return *this;
}

V4& XmMathPlus::V4::operator-=(const V4& rhs)
{
	return (V4&)Real4::operator-=(rhs);

	//x -= rhs.x;
	//y -= rhs.y;
	//z -= rhs.z;
	//w -= rhs.w;
	//return *this;
}

V4& XmMathPlus::V4::operator*=(Real rhs)
{
	return (V4&)Real4::operator*=(rhs);

	//x *= rhs;
	//y *= rhs;
	//z *= rhs;
	//w *= rhs;
	//return *this;
}

V4& XmMathPlus::V4::operator/=(Real rhs)
{
	return (V4&)Real4::operator/=(rhs);

	//x /= rhs;
	//y /= rhs;
	//z /= rhs;
	//w /= rhs;
	//return *this;
}

V4 XmMathPlus::V4::operator-() const
{
	return Real4::operator-();

	//return V4(-x, -y, -z, -w);
}

V4 XmMathPlus::V4::operator+() const
{
	return Real4::operator+();

	//return V4(+x, +y, +z, +w);
}

V4 XmMathPlus::V4::operator+(const V4& rhs) const
{
	return Real4::operator+(rhs);

	//return V4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

V4 XmMathPlus::V4::operator-(const V4& rhs) const
{
	return Real4::operator-(rhs);

	//return V4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

V4 XmMathPlus::V4::operator*(Real rhs) const
{
	return Real4::operator*(rhs);

	//return V4(x * rhs, y * rhs, z * rhs, w * rhs);
}

V4 XmMathPlus::V4::operator/(Real rhs) const
{
	return Real4::operator/(rhs);

	//return V4(x / rhs, y / rhs, z / rhs, w / rhs);
}

bool XmMathPlus::V4::operator==(const V4& rhs) const
{
	return Real4::operator==(rhs);

	//return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

bool XmMathPlus::V4::operator!=(const V4& rhs) const
{
	return Real4::operator!=(rhs);

	//return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
}

XmMathPlus::V4::operator Real2() const
{
	return Real2(xmv);
}

XmMathPlus::V4::operator Real3() const
{
	return Real3(xmv);
}

XmMathPlus::V4::operator Real4() const
{
	return Real4(xmv);
}

Real XmMathPlus::V4::SqrMagnitude() const
{
	return DirectX::XMVector4LengthSq(xmv).m128_f32[0];

	//return x * x + y * y + z * z + w * w;
}

Real XmMathPlus::V4::Magnitude() const
{
	return DirectX::XMVector4Length(xmv).m128_f32[0];

	//return sqrt(x * x + y * y + z * z + w * w);
}

V4 XmMathPlus::V4::Normalized() const
{
	return DirectX::XMVector4Normalize(xmv);

	//Real m = Magnitude();

	//if (m > 0)
	//{
	//	return *this / m;
	//}
	//else
	//{
	//	return V4(0, 0, 0, 0);
	//}
}

void XmMathPlus::V4::Normalize()
{
	*this = DirectX::XMVector4Normalize(xmv);

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
	//	w = 0;
	//}
}

V4 XmMathPlus::V4::Scale(const V4& a, const V4& b)
{
	return V4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Real XmMathPlus::V4::Dot(const V4& a, const V4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

V4 XmMathPlus::V4::Lerp(const V4& a, const V4& b, Real t)
{
	if (t < 0.0f)
	{
		t = 0.0f;
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
	}

	return V4::LerpUnclmaped(a, b, t);
}

V4 XmMathPlus::V4::LerpUnclmaped(const V4& a, const V4& b, Real t)
{
	return DirectX::XMVectorLerp(a.xmv, b.xmv, t);

	//V4 delta = b - a;
	//return a + delta * t;
}

V4 XmMathPlus::V4::Project(const V4& vector, const V4& onNormal)
{
	return Dot(onNormal, vector) / Dot(onNormal, onNormal) * onNormal;
}

V4 XmMathPlus::V4::Reflect(const V4& inDirection, const V4& inNormal)
{
	return inDirection + 2.0f * inNormal * Dot(-inDirection, inNormal);
}

V4 XmMathPlus::V4::Sliding(const V4& inDirection, const V4& inNormal)
{
	return inDirection - inNormal * V4::Dot(inDirection, inNormal);
}

Real XmMathPlus::V4::Cos(const V4& from, const V4& to)
{
	return Dot(from.Normalized(), to.Normalized());
}

Real XmMathPlus::V4::Radian(const V4& from, const V4& to)
{
	Real cos = V4::Cos(from, to);
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

Real XmMathPlus::V4::Angle(const V4& from, const V4& to)
{
	return Radian(from, to) * Rad2Deg;
}

Real XmMathPlus::V4::Distance(const V4& a, const V4& b)
{
	V4 delta = b - a;
	return delta.Magnitude();
}

Real XmMathPlus::V4::SqrDistance(const V4& a, const V4& b)
{
	V4 delta = b - a;
	return delta.SqrMagnitude();
}

V4 XmMathPlus::V4::Abs(const V4& v)
{
	return V4(Ex::Abs(v.x), Ex::Abs(v.y), Ex::Abs(v.z), Ex::Abs(v.w));
}

V4 XmMathPlus::V4::Min(const V4& a, const V4& b)
{
	return V4(Ex::Min(a.x, b.x), Ex::Min(a.y, b.y), Ex::Min(a.z, b.z), Ex::Min(a.w, b.w));
}

V4 XmMathPlus::V4::Max(const V4& a, const V4& b)
{
	return V4(Ex::Max(a.x, b.x), Ex::Max(a.y, b.y), Ex::Max(a.z, b.z), Ex::Max(a.w, b.w));
}

Real XmMathPlus::V4::MinElement(const V4& v)
{
	Real maxElement = v.x;
	maxElement = Ex::Min(maxElement, v.y);
	maxElement = Ex::Min(maxElement, v.z);
	maxElement = Ex::Min(maxElement, v.w);
	return maxElement;
}

Real XmMathPlus::V4::MaxElement(const V4& v)
{
	Real maxElement = v.x;
	maxElement = Ex::Max(maxElement, v.y);
	maxElement = Ex::Max(maxElement, v.z);
	maxElement = Ex::Max(maxElement, v.w);
	return maxElement;
}

Real XmMathPlus::V4::MinAbsElement(const V4& v)
{
	return MinElement(Abs(v));
}

Real XmMathPlus::V4::MaxAbsElement(const V4& v)
{
	return MaxElement(Abs(v));
}

V4 XmMathPlus::operator*(Real scale, const V4& vector)
{
	return vector * scale;
}
