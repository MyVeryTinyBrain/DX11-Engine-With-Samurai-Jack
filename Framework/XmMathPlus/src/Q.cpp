#include "XmMathPlusPCH.h"
#include "Q.h"
#include "V3.h"
#include "M4.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

Q& XmMathPlus::Q::operator+=(const Q& rhs)
{
	return (Q&)Real4::operator+=(rhs);

	//x += rhs.x;
	//y += rhs.y;
	//z += rhs.z;
	//w += rhs.w;
	//return *this;
}

Q& XmMathPlus::Q::operator-=(const Q& rhs)
{
	return (Q&)Real4::operator-=(rhs);

	//x -= rhs.x;
	//y -= rhs.y;
	//z -= rhs.z;
	//w -= rhs.w;
	//return *this;
}

Q& XmMathPlus::Q::operator*=(const Q& rhs)
{
	const Q& lhs = *this;
	*this = lhs * rhs;
	return *this;
}

Q& XmMathPlus::Q::operator*=(Real rhs)
{
	return (Q&)Real4::operator*=(rhs);

	//x *= rhs;
	//y *= rhs;
	//z *= rhs;
	//w *= rhs;
	//return *this;
}

Q& XmMathPlus::Q::operator/=(Real rhs)
{
	return (Q&)Real4::operator/=(rhs);

	//x /= rhs;
	//y /= rhs;
	//z /= rhs;
	//w /= rhs;
	//return *this;
}

Q XmMathPlus::Q::operator+() const
{
	return Real4::operator+();

	//return Q(+x, +y, +z, +w);
}

Q XmMathPlus::Q::operator-() const
{
	return Real4::operator-();

	//return Q(-x, -y, -z, -w);
}

Q XmMathPlus::Q::operator+(const Q& rhs) const
{
	return Real4::operator+(rhs);

	//return Q(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Q XmMathPlus::Q::operator-(const Q& rhs) const
{
	return Real4::operator-(rhs);

	//return Q(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Q XmMathPlus::Q::operator*(const Q& rhs) const
{
	// 회전 순서가 변경되었습니다.
	// Q0 * Q1 * Q2 에 대해서
	// 기존에서는 Q2, Q1, Q0 순서로 회전했습니다.
	// 이제는 Q0, Q1, Q2 순서로 회전하게 됩니다.
	return DirectX::XMQuaternionMultiply(this->xmv, rhs.xmv);

	//const Q& lhs = *this;
	//return Q(
	//	lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
	//	lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
	//	lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
	//	lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z);
}

Q XmMathPlus::Q::operator*(Real rhs) const
{
	return Real4::operator*(rhs);

	//return Q(x * rhs, y * rhs, z * rhs, w * rhs);
}

V3 XmMathPlus::Q::operator*(const V3& vector) const
{
	return Q::MultiplyVector(vector);
}

Q XmMathPlus::Q::operator/(Real rhs) const
{
	return Real4::operator/(rhs);

	//return Q(x / rhs, y / rhs, z / rhs, w / rhs);
}

bool XmMathPlus::Q::operator==(const Q& rhs) const
{
	return Real4::operator==(rhs);

	//return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

bool XmMathPlus::Q::operator!=(const Q& rhs) const
{
	return Real4::operator!=(rhs);

	//return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
}

XmMathPlus::Q::operator Real4() const
{
	return Real4(x, y, z, w);
}

V3 XmMathPlus::Q::MultiplyVector(const V3& vector) const
{
	return DirectX::XMVector3Rotate(vector.xmv, xmv);

	////Q q = *this;

	////Q qv = Q(vector, 0.0f);

	////Q qInv = Inverse();

	////Q res = q * qv * qInv;

	////return V3(res.x, res.y, res.z);

	//const Q& rotation = *this;

	//float x = rotation.x * 2.0F;
	//float y = rotation.y * 2.0F;
	//float z = rotation.z * 2.0F;
	//float xx = rotation.x * x;
	//float yy = rotation.y * y;
	//float zz = rotation.z * z;
	//float xy = rotation.x * y;
	//float xz = rotation.x * z;
	//float yz = rotation.y * z;
	//float wx = rotation.w * x;
	//float wy = rotation.w * y;
	//float wz = rotation.w * z;

	//V3 res;
	//res.x = (1.0F - (yy + zz)) * vector.x + (xy - wz) * vector.y + (xz + wy) * vector.z;
	//res.y = (xy + wz) * vector.x + (1.0F - (xx + zz)) * vector.y + (yz - wx) * vector.z;
	//res.z = (xz - wy) * vector.x + (yz + wx) * vector.y + (1.0F - (xx + yy)) * vector.z;
	//return res;
}

V3 XmMathPlus::Q::GetAxis() const
{
	Real m = Magnitude();

	if (m == 0)
	{
		return V3(0, 0, 0);
	}
	else
	{
		// Normalized Vector
		V3 axis = xmv;
		return axis.normalized;
	}
}

void XmMathPlus::Q::SetAxis(const V3& vector)
{
	Real m = vector.SqrMagnitude();
	if (m == 0)
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}

	Real cos = w;
	Real angle = Ex::Acos(cos) * Rad2Deg;
	V3 axis = V3(vector / m);

	*this = Q::AxisAngle(axis, angle);
}

void XmMathPlus::Q::Normalize()
{
	*this = DirectX::XMQuaternionNormalize(xmv);

	//Real m = Magnitude();

	//if (m > 0)
	//{
	//	*this /= m;
	//}
	//else
	//{
	//	*this = Q(0, 0, 0, 1);
	//}
}

Q XmMathPlus::Q::Conjugate() const
{
	return DirectX::XMQuaternionConjugate(xmv);

	//return Q(-x, -y, -z, w);
}

Q XmMathPlus::Q::Inverse() const
{
	return DirectX::XMQuaternionInverse(xmv);

	//Q top = Conjugate();
	//Real bottom = (x * x + y * y + z * z) + (w * w);

	//if (bottom == 0)
	//{
	//	return Q(0, 0, 0, 1);
	//}

	//return top / bottom;
}

Real XmMathPlus::Q::SqrMagnitude() const
{
	return DirectX::XMVector4LengthSq(xmv).m128_f32[0];

	//return x * x + y * y + z * z + w * w;
}

Real XmMathPlus::Q::Magnitude() const
{
	return DirectX::XMVector4Length(xmv).m128_f32[0];

	//return sqrt(x * x + y * y + z * z + w * w);
}

Q XmMathPlus::Q::Normalized() const
{
	return DirectX::XMQuaternionNormalize(xmv);

	//Real m = Magnitude();

	//if (m > 0)
	//{
	//	return *this / m;
	//}
	//else
	//{
	//	return Q(0, 0, 0, 1);
	//}
}

V3 XmMathPlus::Q::EulerAngles() const
{
	const Q& q = *this;
	V3 euler;

	// if the input quaternion is normalized, this is exactly one. Otherwise, this acts as a correction factor for the quaternion's not-normalizedness
	// Real unit = (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
	Real unit = q.SqrMagnitude();

	// this will have a magnitude of 0.5 or greater if and only if this is a singularity case
	Real lightBlend = q.x * q.w - q.y * q.z;

	if (lightBlend > 0.49999f * unit) // singularity at north pole
	{
		euler.x = PI / 2;
		euler.y = 2.0f * Ex::Atan2(q.y, q.x);
		euler.z = 0;
	}
	else if (lightBlend < -0.49999f * unit) // singularity at south pole
	{
		euler.x = -PI / 2;
		euler.y = -2.0f * Ex::Atan2(q.y, q.x);
		euler.z = 0;
	}
	else // no singularity - this is the majority of cases
	{
		euler.x = Ex::Asin(2.0f * (q.w * q.x - q.y * q.z));
		euler.y = Ex::Atan2(2.0f * q.w * q.y + 2.0f * q.z * q.x, 1 - 2.0f * (q.x * q.x + q.y * q.y));
		euler.z = Ex::Atan2(2.0f * q.w * q.z + 2.0f * q.x * q.y, 1 - 2.0f * (q.z * q.z + q.x * q.x));
	}

	// all the math so far has been done in radians. Before returning, we convert to degrees...
	euler *= Rad2Deg;

	//...and then ensure the degree values are between 0 and 360
	euler.x = Ex::Mod(euler.x, 360.0f);
	euler.y = Ex::Mod(euler.y, 360.0f);
	euler.z = Ex::Mod(euler.z, 360.0f);

	return euler;
}

Real4x4 XmMathPlus::Q::GetRowRotationMatrix() const
{
	M4 rhs = GetColumnRotationMatrix();
	return rhs.transposed;
}

Real4x4 XmMathPlus::Q::GetColumnRotationMatrix() const
{
	return Real4x4(
		1.0f - 2.0f * y * y - 2.0f * z * z, 2.0f * x * y - 2.0f * z * w, 2.0f * x * z + 2.0f * y * w, 0.0f,
		2.0f * x * y + 2.0f * z * w, 1.0f - 2.0f * x * x - 2.0f * z * z, 2.0f * y * z - 2.0f * x * w, 0.0f,
		2.0f * x * z - 2.0f * y * w, 2.0f * y * z + 2.0f * x * w, 1.0f - 2.0f * x * x - 2.0f * y * y, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Real XmMathPlus::Q::Dot(const Q& lhs, const Q& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

Real XmMathPlus::Q::Radian(const Q& lhs, const Q& rhs)
{
	Real dot = Q::Dot(lhs, rhs);
	return Ex::Acos(Ex::Min(Ex::Abs(dot), 1.0f)) * 2.0f;
}

Real XmMathPlus::Q::Angle(const Q& lhs, const Q& rhs)
{	
	return Q::Radian(lhs, rhs) * Rad2Deg;
}

Q XmMathPlus::Q::Euler(Real x, Real y, Real z)
{
	Real xOver2 = x * Deg2Rad * 0.5f;
	Real yOver2 = y * Deg2Rad * 0.5f;
	Real zOver2 = z * Deg2Rad * 0.5f;

	Real sinXOver2 = Ex::Sin(xOver2);
	Real cosXOver2 = Ex::Cos(xOver2);
	Real sinYOver2 = Ex::Sin(yOver2);
	Real cosYOver2 = Ex::Cos(yOver2);
	Real sinZOver2 = Ex::Sin(zOver2);
	Real cosZOver2 = Ex::Cos(zOver2);

	Q result;
	result.x = cosYOver2 * sinXOver2 * cosZOver2 + sinYOver2 * cosXOver2 * sinZOver2;
	result.y = sinYOver2 * cosXOver2 * cosZOver2 - cosYOver2 * sinXOver2 * sinZOver2;
	result.z = cosYOver2 * cosXOver2 * sinZOver2 - sinYOver2 * sinXOver2 * cosZOver2;
	result.w = cosYOver2 * cosXOver2 * cosZOver2 + sinYOver2 * sinXOver2 * sinZOver2;

	return result;
}

Q XmMathPlus::Q::Euler(const V3& euler)
{
	return Euler(euler.x, euler.y, euler.z);
}

Q XmMathPlus::Q::AxisAngle(const V3& axis, Real angle)
{
	return DirectX::XMQuaternionRotationAxis(axis.xmv, angle * Deg2Rad);

	//V3 vAxis = axis;

	//if (vAxis.sqrMagnitude == 0.0f)
	//	return Q::identity();

	//Q result = Q::identity();
	//Real radians = angle * Deg2Rad;
	//radians *= 0.5f;
	//vAxis.Normalize();
	//vAxis = vAxis * Ex::Sin(radians);
	//result.x = vAxis.x;
	//result.y = vAxis.y;
	//result.z = vAxis.z;
	//result.w = Ex::Cos(radians);

	//result.Normalize();
	//return result;
}

Q XmMathPlus::Q::FromToRotation(const V3& from, const V3& to)
{
	// https://answers.unity.com/questions/1668856/whats-the-source-code-of-quaternionfromtorotation.html
	V3 axis = V3::Cross(from, to);
	float angle = V3::Angle(from, to);
	if (angle >= 179.9196f)
	{
		V3 r = V3::Cross(from, V3::right());
		axis = V3::Cross(r, from);
		if (axis.sqrMagnitude < 0.000001f)
			axis = V3::up();
	}

	if (axis.sqrMagnitude == 0)
		return Q::identity();

	return Q::AxisAngle(axis.normalized, angle);

	//// https://answers.unity.com/questions/1668856/whats-the-source-code-of-quaternionfromtorotation.html

	//if (V3::SqrDistance(from, to) < Epsilon)
	//	return Q::identity();

	//V3 vFrom = from;
	//V3 vTo = to;
	//V3 vAxis = V3::Cross(from, to);

	//Real dot = V3::Dot(from, to);
	//Real denominator = vFrom.Magnitude() * vTo.Magnitude();
	//Real cos;
	//if (denominator == 0) cos = 0;
	//else cos = dot / denominator;
	//if (cos < -1.0f) cos = -1.0f;
	//else if (cos > +1.0f) cos = +1.0f;
	//Real radian = Ex::Acos(cos);
	//Real angle = radian * 180.0f / PI;

	//if (angle >= 179.9196f)
	//{
	//	V3 right(1, 0, 0);
	//	V3 r = V3::Cross(from, right);
	//	vAxis = V3::Cross(r, from);

	//	Real axisSqrtMagnitude = vAxis.x * vAxis.x + vAxis.y * vAxis.y + vAxis.z * vAxis.z;

	//	if (axisSqrtMagnitude < 0.000001f)
	//		vAxis = V3(0, 0, 1);
	//}

	//vAxis.Normalize();
	//return AxisAngle(vAxis, angle);
}

Q XmMathPlus::Q::SLerp(const Q& a, const Q& b, Real t)
{
	if (t < 0.0f)
	{
		t = 0.0f;
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
	}

	return Q::SLerpUnclamped(a, b, t);
}

Q XmMathPlus::Q::SLerpUnclamped(Q a, Q b, Real t)
{
	return DirectX::XMQuaternionSlerp(a.xmv, b.xmv, t);

	//// if either input is zero, return the other.
	//if (a.Magnitude() == 0.0f)
	//{
	//	if (b.Magnitude() == 0.0f)
	//	{
	//		return Q::identity();
	//	}
	//	return b;
	//}
	//else if (b.Magnitude() == 0.0f)
	//{
	//	return a;
	//}

	//Real cosHalfAngle = a.w * b.w + V3::Dot(V3(a.x, a.y, a.z), V3(b.x, b.y, b.z));

	//if (cosHalfAngle >= 1.0f || cosHalfAngle <= -1.0f)
	//{
	//	// angle = 0.0f, so just return one input.
	//	return a;
	//}
	//else if (cosHalfAngle < 0.0f)
	//{
	//	b.x = -b.x;
	//	b.y = -b.y;
	//	b.z = -b.z;
	//	b.w = -b.w;
	//	cosHalfAngle = -cosHalfAngle;
	//}

	//Real blendA;
	//Real blendB;
	//if (cosHalfAngle < 0.99f)
	//{
	//	// do proper slerp for big angles
	//	Real halfAngle = Ex::Acos(cosHalfAngle);
	//	Real sinHalfAngle = Ex::Sin(halfAngle);
	//	Real oneOverSinHalfAngle = 1.0f / sinHalfAngle;
	//	blendA = Ex::Sin(halfAngle * (1.0f - t)) * oneOverSinHalfAngle;
	//	blendB = Ex::Sin(halfAngle * t) * oneOverSinHalfAngle;
	//}
	//else
	//{
	//	// do lerp if angle is really small.
	//	blendA = 1.0f - t;
	//	blendB = t;
	//}

	//V3 vA = V3(a.x, a.y, a.z);
	//V3 vB = V3(b.x, b.y, b.z);
	//V3 v = blendA * vA + blendB * b;

	//Q result = Q(v, blendA * a.w + blendB * b.w);
	//if (result.Magnitude() > 0.0f)
	//	return result.Normalized();
	//else
	//	return Q::identity();
}

Q XmMathPlus::Q::Lerp(const Q& a, const Q& b, Real t)
{
	if (t < 0.0f)
	{
		t = 0.0f;
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
	}

	return Q::LerpUnclamped(a, b, t);
}

Q XmMathPlus::Q::LerpUnclamped(const Q& a, const Q& b, Real t)
{
	return DirectX::XMVectorLerp(a.xmv, b.xmv, t);

	//Q delta = b - a;
	//return a + delta * t;
}

Q XmMathPlus::Q::RotateTowards(const Q& from, const Q& to, Real maxDegreeDelta)
{
	Real angle = Angle(from, to);
	if (angle == 0) return to;
	Real t = Ex::Min(1.0f, maxDegreeDelta / angle);
	return SLerp(from, to, t);
}

Q XmMathPlus::Q::LookRotation(const V3& forward, V3& up)
{
	V3 vForward = forward;

	if (forward == up)
		up *= -1.0f;

	if (forward == -up)
	{
		Q q = FromToRotation(V3::forward(), forward);
		up = q.GetAxis();
		return q;
	}

	vForward.Normalize();
	V3 right = (V3::Cross(up, vForward)).Normalized();
	up = V3::Cross(vForward, right);
	auto m00 = right.x;
	auto m01 = right.y;
	auto m02 = right.z;
	auto m10 = up.x;
	auto m11 = up.y;
	auto m12 = up.z;
	auto m20 = vForward.x;
	auto m21 = vForward.y;
	auto m22 = vForward.z;

	Real num8 = (m00 + m11) + m22;
	Q quaternion;
	if (num8 > 0.0f)
	{
		auto num = sqrt(num8 + 1.0f);
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		auto num7 = sqrt(((1.0f + m00) - m11) - m22);
		auto num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = (m02 + m20) * num4;
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		auto num6 = sqrt(((1.0f + m11) - m00) - m22);
		auto num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = (m21 + m12) * num3;
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	auto num5 = sqrt(((1.0f + m22) - m00) - m11);
	auto num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = 0.5f * num5;
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}

Q XmMathPlus::Q::RightHandedToLeftHanded(const Q& rh)
{
	return Q(-rh.x, -rh.z, -rh.y, rh.w);
}

Q XmMathPlus::operator*(Real scala, const Q& quaternion)
{
	return quaternion * scala;
}
