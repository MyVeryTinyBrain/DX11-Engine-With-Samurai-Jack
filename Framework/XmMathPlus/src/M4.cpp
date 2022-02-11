#include "XmMathPlusPCH.h"
#include "M4.h"
#include "V3.h"
#include "Q.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

M4& XmMathPlus::M4::operator*=(const M4& rhs)
{
    *this = M4::MultiplyMatrix(*this, rhs);
    return *this;
}

M4& XmMathPlus::M4::operator+=(const M4& rhs)
{
    xmm += rhs.xmm;
    return *this;

    //for (int i = 0; i < 9; ++i)
    //{
    //    a[i] += rhs.a[i];
    //}

    //return *this;
}

M4& XmMathPlus::M4::operator-=(const M4& rhs)
{
    xmm -= rhs.xmm;
    return *this;

    //for (int i = 0; i < 9; ++i)
    //{
    //    a[i] -= rhs.a[i];
    //}

    //return *this;
}

M4& XmMathPlus::M4::operator*=(Real rhs)
{
    xmm *= rhs;
    return *this;

    //for (int i = 0; i < 9; ++i)
    //{
    //    a[i] *= rhs;
    //}

    //return *this;
}

M4& XmMathPlus::M4::operator/=(Real rhs)
{
    xmm /= rhs;
    return *this;

    //for (int i = 0; i < 9; ++i)
    //{
    //    a[i] /= rhs;
    //}

    //return *this;
}

M4 XmMathPlus::M4::operator+() const
{
    return *this;

    //return *this;
}

M4 XmMathPlus::M4::operator-() const
{
    return M4(-xmm);

    //M4 copy = *this;

    //for (int i = 0; i < 9; ++i)
    //{
    //    copy.a[i] *= -1.0f;
    //}

    //return copy;
}

M4 XmMathPlus::M4::operator*(const M4& rhs) const
{
    const M4& lhs = *this;
    return M4::MultiplyMatrix(lhs, rhs);
}

M4 XmMathPlus::M4::operator+(const M4& rhs) const
{
    return M4(xmm + rhs.xmm);

    //M4 m;

    //for (int i = 0; i < 9; ++i)
    //{
    //    m.a[i] = this->a[i] + rhs.a[i];
    //}

    //return m;
}

M4 XmMathPlus::M4::operator-(const M4& rhs) const
{
    return M4(xmm - rhs.xmm);

    //M4 m;

    //for (int i = 0; i < 9; ++i)
    //{
    //    m.a[i] = this->a[i] - rhs.a[i];
    //}

    //return m;
}

M4 XmMathPlus::M4::operator*(Real rhs) const
{
    return M4(xmm * rhs);

    //M4 copy = *this;

    //for (int i = 0; i < 9; ++i)
    //{
    //    copy.a[i] *= rhs;
    //}

    //return copy;
}

M4 XmMathPlus::M4::operator/(Real rhs) const
{
    return M4(xmm / rhs);

    //M4 copy = *this;

    //for (int i = 0; i < 9; ++i)
    //{
    //    copy.a[i] /= rhs;
    //}

    //return copy;
}

bool XmMathPlus::M4::operator==(const M4& rhs) const
{
    return Real4x4::operator==(rhs);

    //for (int i = 0; i < 9; ++i)
    //{
    //    if (this->a[i] != rhs.a[i])
    //    {
    //        return false;
    //    }
    //}

    //return true;
}

bool XmMathPlus::M4::operator!=(const M4& rhs) const
{
    return Real4x4::operator!=(rhs);

    //for (int i = 0; i < 9; ++i)
    //{
    //    if (this->a[i] != rhs.a[i])
    //    {
    //        return true;
    //    }
    //}

    //return false;
}

Real4 XmMathPlus::M4::operator*(const Real4& rhs) const
{
    return M4::MultiplyRHSReal4(*this, rhs);
}

XmMathPlus::M4::operator Real4x4() const
{
    return Real4x4(*this);
}

XmMathPlus::M4::operator DirectX::XMMATRIX() const
{
    return xmm;
}

Real3 XmMathPlus::M4::GetTranslation() const
{
    return Real3(row[3]);
}

Real4 XmMathPlus::M4::GetRotation() const
{
    V3 row0 = GetRow(0); row0.Normalize();
    V3 row1 = GetRow(1); row1.Normalize();
    V3 row2 = GetRow(2); row2.Normalize();

    Q q;
    q.w = sqrt(Max(0.0f, 1 + row0.x + row1.y + row2.z)) / 2;
    q.x = sqrt(Max(0.0f, 1 + row0.x - row1.y - row2.z)) / 2;
    q.y = sqrt(Max(0.0f, 1 - row0.x + row1.y - row2.z)) / 2;
    q.z = sqrt(Max(0.0f, 1 - row0.x - row1.y + row2.z)) / 2;
    q.x *= Sign(q.x * (m[1][2] - m[2][1]));
    q.y *= Sign(q.y * (m[2][0] - m[0][2]));
    q.z *= Sign(q.z * (m[0][1] - m[1][0]));

    return q.normalized;
}

Real4 XmMathPlus::M4::GetEulerAngles() const
{
    return ((Q)GetRotation()).eulerAngles;
}

Real3 XmMathPlus::M4::GetScale() const
{
    float sx = ((V3)GetRow(0)).magnitude;
    float sy = ((V3)GetRow(1)).magnitude;
    float sz = ((V3)GetRow(2)).magnitude;

    return V3(sx, sy, sz);
}

Real4 XmMathPlus::M4::GetRow(int i) const
{
    return Real4(row[i]);
}

Real4 XmMathPlus::M4::GetColumn(int i) const
{
    return Real4(m[0][i], m[1][i], m[2][i], m[3][i]);
}

void XmMathPlus::M4::SetRow(int i, const Real4& value)
{
    row[i] = value.xmv;
}

void XmMathPlus::M4::SetColumn(int i, const Real4& value)
{
    m[0][i] = value.x;
    m[1][i] = value.y;
    m[2][i] = value.z;
    m[3][i] = value.w;
}

Real3 XmMathPlus::M4::MultiplyPoint(const Real3& lhs) const
{
    return Real3(DirectX::XMVector3TransformCoord(lhs.xmv, this->xmm));

    //Real4 point = Real4(lhs.x, lhs.y, lhs.z, 1);
    //Real4 result = M4::MultiplyLHSReal4(point, *this);
    //if (result.w != 0)
    //{
    //    result.x /= result.w;
    //    result.y /= result.w;
    //    result.z /= result.w;
    //    result.w /= result.w;
    //}
    //return Real3(result.x, result.y, result.z);
}

Real3 XmMathPlus::M4::MultiplyVector(const Real3& lhs) const
{
    return Real3(DirectX::XMVector3TransformNormal(lhs.xmv, this->xmm));

    //Real4 vector = Real4(lhs.x, lhs.y, lhs.z, 0);
    //Real4 result = M4::MultiplyLHSReal4(vector, *this);
    //if (result.w != 0)
    //{
    //    result.x /= result.w;
    //    result.y /= result.w;
    //    result.z /= result.w;
    //    result.w /= result.w;
    //}
    //return Real3(result.x, result.y, result.z);
}

Real XmMathPlus::M4::GetDeterminant() const
{
    return DirectX::XMMatrixDeterminant(xmm).m128_f32[0];

    //float a = m00, b = m01, c = m02, d = m03;
    //float e = m10, f = m11, g = m12, h = m13;
    //float i = m20, j = m21, k = m22, l = m23;
    //float m = m30, n = m31, o = m32, p = m33;

    //float kp_lo = k * p - l * o;
    //float jp_ln = j * p - l * n;
    //float jo_kn = j * o - k * n;
    //float ip_lm = i * p - l * m;
    //float io_km = i * o - k * m;
    //float in_jm = i * n - j * m;

    //return a * (f * kp_lo - g * jp_ln + h * jo_kn) -
    //    b * (e * kp_lo - g * ip_lm + h * io_km) +
    //    c * (e * jp_ln - f * ip_lm + h * in_jm) -
    //    d * (e * jo_kn - f * io_km + g * in_jm);
}

M4 XmMathPlus::M4::GetInversed() const
{
    return DirectX::XMMatrixInverse(nullptr, xmm);

    //float a = m00, b = m01, c = m02, d = m03;
    //float e = m10, f = m11, g = m12, h = m13;
    //float i = m20, j = m21, k = m22, l = m23;
    //float m = m30, n = m31, o = m32, p = m33;

    //float kp_lo = k * p - l * o;
    //float jp_ln = j * p - l * n;
    //float jo_kn = j * o - k * n;
    //float ip_lm = i * p - l * m;
    //float io_km = i * o - k * m;
    //float in_jm = i * n - j * m;

    //float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
    //float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
    //float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
    //float a14 = -(e * jo_kn - f * io_km + g * in_jm);

    //float det = a * a11 + b * a12 + c * a13 + d * a14;

    //if (Abs(det) < Epsilon)
    //{
    //    return M4(
    //        NaN, NaN, NaN, NaN,
    //        NaN, NaN, NaN, NaN,
    //        NaN, NaN, NaN, NaN,
    //        NaN, NaN, NaN, NaN);
    //}

    //float invDet = 1.0f / det;

    //M4 result;
    //result.m00 = a11 * invDet;
    //result.m10 = a12 * invDet;
    //result.m20 = a13 * invDet;
    //result.m30 = a14 * invDet;

    //result.m01 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
    //result.m11 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
    //result.m21 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
    //result.m31 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

    //float gp_ho = g * p - h * o;
    //float fp_hn = f * p - h * n;
    //float fo_gn = f * o - g * n;
    //float ep_hm = e * p - h * m;
    //float eo_gm = e * o - g * m;
    //float en_fm = e * n - f * m;

    //result.m02 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
    //result.m12 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
    //result.m22 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
    //result.m32 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

    //float gl_hk = g * l - h * k;
    //float fl_hj = f * l - h * j;
    //float fk_gj = f * k - g * j;
    //float el_hi = e * l - h * i;
    //float ek_gi = e * k - g * i;
    //float ej_fi = e * j - f * i;

    //result.m03 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
    //result.m13 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
    //result.m23 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
    //result.m33 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

    //return result;
}

void XmMathPlus::M4::Inverse()
{
    *this = GetInversed();
}

bool XmMathPlus::M4::IsIdentity() const
{
    return DirectX::XMMatrixIsIdentity(xmm);

    //return
    //    m00 == 0 && m11 == 0 && m22 == 0 && m33 == 0 && // Check diagonal element first for early out.
    //    m01 == 0 && m02 == 0 && m03 == 0 &&
    //    m10 == 0 && m12 == 0 && m13 == 0 &&
    //    m20 == 0 && m21 == 0 && m23 == 0 &&
    //    m30 == 0 && m31 == 0 && m32 == 0;
}

M4 XmMathPlus::M4::GetTransposed() const
{
    return DirectX::XMMatrixTranspose(xmm);

    //M4 result;

    //result.m00 = m00;
    //result.m01 = m10;
    //result.m02 = m20;
    //result.m03 = m30;
    //result.m10 = m01;
    //result.m11 = m11;
    //result.m12 = m21;
    //result.m13 = m31;
    //result.m20 = m02;
    //result.m21 = m12;
    //result.m22 = m22;
    //result.m23 = m32;
    //result.m30 = m03;
    //result.m31 = m13;
    //result.m32 = m23;
    //result.m33 = m33;

    //return result;
}

void XmMathPlus::M4::Transpose()
{
    xmm = DirectX::XMMatrixTranspose(xmm);
}

M4 XmMathPlus::M4::MultiplyMatrix(const M4& lhs, const M4& rhs)
{
    return DirectX::XMMatrixMultiply(lhs.xmm, rhs.xmm);

    //M4 res;

    //res.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
    //res.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
    //res.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
    //res.m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;

    //res.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
    //res.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
    //res.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
    //res.m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;

    //res.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
    //res.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
    //res.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
    //res.m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;

    //res.m30 = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30;
    //res.m31 = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31;
    //res.m32 = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32;
    //res.m33 = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33;

    //return res;
}

Real4 XmMathPlus::M4::MultiplyLHSReal4(const Real4& lhs, const M4& rhs)
{
    return Real4(DirectX::XMVector4Transform(lhs.xmv, rhs.xmm));

    //Real x = lhs.x * rhs.m[0][0] + lhs.y * rhs.m[1][0] + lhs.z * rhs.m[2][0] + lhs.w * rhs.m[3][0];
    //Real y = lhs.x * rhs.m[0][1] + lhs.y * rhs.m[1][1] + lhs.z * rhs.m[2][1] + lhs.w * rhs.m[3][1];
    //Real z = lhs.x * rhs.m[0][2] + lhs.y * rhs.m[1][2] + lhs.z * rhs.m[2][2] + lhs.w * rhs.m[3][2];
    //Real w = lhs.x * rhs.m[0][3] + lhs.y * rhs.m[1][3] + lhs.z * rhs.m[2][3] + lhs.w * rhs.m[3][3];

    //return Real4(x, y, z, w);
}

Real4 XmMathPlus::M4::MultiplyRHSReal4(const M4& lhs, const Real4& rhs)
{
    return Real4(DirectX::XMVector4Transform(rhs.xmv, lhs.transposed.xmm));

    //Real x = lhs.m[0][0] * rhs.x + lhs.m[0][1] * rhs.y + lhs.m[0][2] * rhs.z + lhs.m[0][3] * rhs.w;
    //Real y = lhs.m[1][0] * rhs.x + lhs.m[1][1] * rhs.y + lhs.m[1][2] * rhs.z + lhs.m[1][3] * rhs.w;
    //Real z = lhs.m[2][0] * rhs.x + lhs.m[2][1] * rhs.y + lhs.m[2][2] * rhs.z + lhs.m[2][3] * rhs.w;
    //Real w = lhs.m[3][0] * rhs.x + lhs.m[3][1] * rhs.y + lhs.m[3][2] * rhs.z + lhs.m[3][3] * rhs.w;

    //return Real4();
}

M4 XmMathPlus::M4::identity()
{
    return M4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

M4 XmMathPlus::M4::Translate(const Real3& translation)
{
    return DirectX::XMMatrixTranslationFromVector(translation.xmv);
}

M4 XmMathPlus::M4::Rotate(const Real4& quaternion)
{
    return DirectX::XMMatrixRotationQuaternion(quaternion.xmv);
}

M4 XmMathPlus::M4::Rotate(const Real3& eulerAngles)
{
    Real3 radians = eulerAngles * Deg2Rad;
    return DirectX::XMMatrixRotationRollPitchYawFromVector(radians.xmv);
}

M4 XmMathPlus::M4::Scale(const Real3& scale)
{
    return DirectX::XMMatrixScalingFromVector(scale.xmv);
}

M4 XmMathPlus::M4::SRT(const Real3& scale, const Real4& quaternion, const Real3& translation)
{
    //return Scale(scale) * Rotate(quaternion) * Translate(translation);
    return DirectX::XMMatrixAffineTransformation(scale.xmv, Q::identity().xmv, quaternion.xmv, translation.xmv);
    //return DirectX::XMMatrixTransformation(V3::one().xmv, Q::identity().xmv, scale.xmv, Q::identity().xmv, quaternion.xmv, translation.xmv);
}

M4 XmMathPlus::M4::SRT(const Real3& scale, const Real3& eulerAngles, const Real3& translation)
{
    return Scale(scale) * Rotate(eulerAngles) * Translate(translation);
}

M4 XmMathPlus::operator*(Real scala, const M4& matrix)
{
    return matrix * scala;
}

Real4 XmMathPlus::operator*(const Real4& lhs, const M4& rhs)
{
    return M4::MultiplyLHSReal4(lhs, rhs);
}
