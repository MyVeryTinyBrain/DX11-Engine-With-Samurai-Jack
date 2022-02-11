#include "XmMathPlusPCH.h"
#include "V2.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

V2& XmMathPlus::V2::operator+=(const V2& rhs)
{
	return (V2&)Real2::operator+=(rhs);

	//x += rhs.x;
	//y += rhs.y;
	//return *this;
}

V2& XmMathPlus::V2::operator-=(const V2& rhs)
{
	return (V2&)Real2::operator-=(rhs);

	//x -= rhs.x;
	//y -= rhs.y;
	//return *this;
}

V2& XmMathPlus::V2::operator*=(Real rhs)
{
	return (V2&)Real2::operator*=(rhs);

	//x *= rhs;
	//y *= rhs;
	//return *this;
}

V2& XmMathPlus::V2::operator/=(Real rhs)
{
	return (V2&)Real2::operator/=(rhs);

	//x /= rhs;
	//y /= rhs;
	//return *this;
}

V2 XmMathPlus::V2::operator-() const
{
	return Real2::operator-();

	//return V2(-x, -y);
}

V2 XmMathPlus::V2::operator+() const
{
	return Real2::operator+();

	//return V2(+x, +y);
}

V2 XmMathPlus::V2::operator+(const V2& rhs) const
{
	return Real2::operator+(rhs);

	//return V2(x + rhs.x, y + rhs.y);
}

V2 XmMathPlus::V2::operator-(const V2& rhs) const
{
	return Real2::operator-(rhs);

	//return V2(x - rhs.x, y - rhs.y);
}

V2 XmMathPlus::V2::operator*(Real rhs) const
{
	return Real2::operator*(rhs);

	//return V2(x * rhs, y * rhs);
}

V2 XmMathPlus::V2::operator/(Real rhs) const
{
	return Real2::operator/(rhs);

	//return V2(x / rhs, y / rhs);
}

bool XmMathPlus::V2::operator==(const V2& rhs) const
{
	return Real2::operator==(rhs);

	//return x == rhs.x && y == rhs.y;
}

bool XmMathPlus::V2::operator!=(const V2& rhs) const
{
	return Real2::operator!=(rhs);

	//return x != rhs.x || y != rhs.y;
}

XmMathPlus::V2::operator Real2() const
{
	return Real2(xmv);
}

XmMathPlus::V2::operator Real3() const
{
	return Real3(xmv);
}

XmMathPlus::V2::operator Real4() const
{
	return Real4(xmv);
}

Real XmMathPlus::V2::SqrMagnitude() const
{
	return DirectX::XMVector2LengthSq(xmv).m128_f32[0];

	//return x * x + y * y;
}

Real XmMathPlus::V2::Magnitude() const
{
	return DirectX::XMVector2Length(xmv).m128_f32[0];

	//return sqrt(x * x + y * y);
}

V2 XmMathPlus::V2::Normalized() const
{
	return DirectX::XMVector2Normalize(xmv);

	//Real m = Magnitude();
	//
	//if (m > 0)
	//{
	//	return *this / m;
	//}
	//else
	//{
	//	return V2(0, 0);
	//}
}

void XmMathPlus::V2::Normalize()
{
	*this = DirectX::XMVector2Normalize(xmv);

	//Real m = Magnitude();

	//if (m > 0)
	//{
	//	*this /= m;
	//}
	//else
	//{
	//	x = 0;
	//	y = 0;
	//}
}

V2 XmMathPlus::V2::Scale(const V2& a, const V2& b)
{
	return V2(a.x * b.x, a.y * b.y);
}

Real XmMathPlus::V2::Dot(const V2& a, const V2& b)
{
	return DirectX::XMVector2Dot(a.xmv, b.xmv).m128_f32[0];

	//return a.x * b.x + a.y * b.y;
}

Real XmMathPlus::V2::Cross(const V2& lhs, const V2& rhs)
{
	return DirectX::XMVector2Cross(lhs.xmv, rhs.xmv).m128_f32[0];

	//return lhs.x * rhs.y - lhs.y * rhs.x;
}

V2 XmMathPlus::V2::Lerp(const V2& a, const V2& b, Real t)
{
	if (t < 0.0f)
	{
		t = 0.0f;
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
	}

	return V2::LerpUnclmaped(a, b, t);
}

V2 XmMathPlus::V2::LerpUnclmaped(const V2& a, const V2& b, Real t)
{
	return DirectX::XMVectorLerp(a.xmv, b.xmv, t);

	//V2 delta = b - a;
	//return a + delta * t;
}

V2 XmMathPlus::V2::Project(const V2& vector, const V2& onNormal)
{
	return Dot(onNormal, vector) / Dot(onNormal, onNormal) * onNormal;
}

V2 XmMathPlus::V2::Reflect(const V2& inDirection, const V2& inNormal)
{
	return inDirection + 2.0f * inNormal * Dot(-inDirection, inNormal);
}

V2 XmMathPlus::V2::Sliding(const V2& inDirection, const V2& inNormal)
{
	return inDirection - inNormal * V2::Dot(inDirection, inNormal);
}

Real XmMathPlus::V2::Cos(const V2& from, const V2& to)
{
	return Dot(from.Normalized(), to.Normalized());
}

Real XmMathPlus::V2::Radian(const V2& from, const V2& to)
{
	Real cos = V2::Cos(from, to);
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

Real XmMathPlus::V2::Angle(const V2& from, const V2& to)
{
	return Radian(from, to) * Rad2Deg;
}

Real XmMathPlus::V2::SignedAngle(const V2& from, const V2& to)
{
	Real angle = V2::Angle(from, to);

	if (to.y < from.y) angle *= -1;

	return angle;
}

Real XmMathPlus::V2::Angle180(const V2& dir)
{
	Real angle = V2::Angle(V2::right(), dir);

	if (dir.y < 0) angle *= -1;

	return angle;
}

Real XmMathPlus::V2::Angle360(const V2& dir)
{
	Real angle = V2::Angle(V2::right(), dir);

	if (dir.y < 0) angle = 360 - angle;

	return angle;
}

Real XmMathPlus::V2::Distance(const V2& a, const V2& b)
{
	V2 delta = b - a;
	return delta.Magnitude();
}

Real XmMathPlus::V2::SqrDistance(const V2& a, const V2& b)
{
	V2 delta = b - a;
	return delta.SqrMagnitude();
}

V2 XmMathPlus::V2::Abs(const V2& v)
{
	return V2(Ex::Abs(v.x), Ex::Abs(v.y));
}

V2 XmMathPlus::V2::Min(const V2& a, const V2& b)
{
	return V2(Ex::Min(a.x, b.x), Ex::Min(a.y, b.y));
}

V2 XmMathPlus::V2::Max(const V2& a, const V2& b)
{
	return V2(Ex::Max(a.x, b.x), Ex::Max(a.y, b.y));
}

Real XmMathPlus::V2::MinElement(const V2& v)
{
	Real maxElement = v.x;
	maxElement = Ex::Min(maxElement, v.y);
	return maxElement;
}

Real XmMathPlus::V2::MaxElement(const V2& v)
{
	Real maxElement = v.x;
	maxElement = Ex::Max(maxElement, v.y);
	return maxElement;
}

Real XmMathPlus::V2::MinAbsElement(const V2& v)
{
	return MinElement(Abs(v));
}

Real XmMathPlus::V2::MaxAbsElement(const V2& v)
{
	return MaxElement(Abs(v));
}

V2 XmMathPlus::operator*(Real scale, const V2& vector)
{
	return vector * scale;
}
