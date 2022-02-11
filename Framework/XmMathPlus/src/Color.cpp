#include "XmMathPlusPCH.h"
#include "Color.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

XmMathPlus::Color::Color(uint dword)
{
    constexpr float f = 1.0f / 255.0f;
    // AABBGGAA
    float avalue = f * (float)(Byte)(dword >> 24);
    float bvalue = f * (float)(Byte)(dword >> 16);
    float gvalue = f * (float)(Byte)(dword >> 8);
    float rvalue = f * (float)(Byte)(dword >> 0);
    value = Real4(rvalue, gvalue, bvalue, avalue);
}

Color& XmMathPlus::Color::operator+=(const Color& rhs)
{
    value += rhs.value;
    return *this;
}

Color& XmMathPlus::Color::operator-=(const Color& rhs)
{
    value -= rhs.value;
    return *this;
}

Color& XmMathPlus::Color::operator*=(Real rhs)
{
    value.xmv *= rhs;
    return *this;
}

Color& XmMathPlus::Color::operator/=(Real rhs)
{
    value /= rhs;
    return *this;
}

Color XmMathPlus::Color::operator+() const
{
    return *this;
}

Color XmMathPlus::Color::operator-() const
{
    return Color(-value);
}

Color XmMathPlus::Color::operator+(const Color& rhs) const
{
    return Color(value + value);
}

Color XmMathPlus::Color::operator-(const Color& rhs) const
{
    return Color(value - rhs.value);
}

Color XmMathPlus::Color::operator*(Real rhs) const
{
    return Color(value * rhs);
}

Color XmMathPlus::Color::operator/(Real rhs) const
{
    return Color(value / rhs);
}

bool XmMathPlus::Color::operator==(const Color& rhs) const
{
    return value == rhs.value;
}

bool XmMathPlus::Color::operator!=(const Color& rhs) const
{
    return value != rhs.value;
}

XmMathPlus::Color::operator uint() const
{
    // AABBGGAA

    Byte dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (Byte)(r * 255.0f + 0.5f);
    Byte dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (Byte)(g * 255.0f + 0.5f);
    Byte dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (Byte)(b * 255.0f + 0.5f);
    Byte dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (Byte)(a * 255.0f + 0.5f);

    return (dwA << 24) | (dwB << 16) | (dwG << 8) | dwR;
}

XmMathPlus::Color::operator Real4() const
{
    return Real4(r, g, b, a);
}

XmMathPlus::Color::operator DirectX::XMVECTOR() const
{
    return value.xmv;
}

uint XmMathPlus::Color::ToRGBA() const
{
    Byte dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (Byte)(r * 255.0f + 0.5f);
    Byte dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (Byte)(g * 255.0f + 0.5f);
    Byte dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (Byte)(b * 255.0f + 0.5f);
    Byte dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (Byte)(a * 255.0f + 0.5f);

    // RGBA
    return (dwR << 24) | (dwG << 16) | (dwB << 8) | dwA;
}

uint XmMathPlus::Color::ToARGB() const
{
    Byte dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (Byte)(r * 255.0f + 0.5f);
    Byte dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (Byte)(g * 255.0f + 0.5f);
    Byte dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (Byte)(b * 255.0f + 0.5f);
    Byte dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (Byte)(a * 255.0f + 0.5f);

    // ARGB
    return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
}

Real XmMathPlus::Color::Magnitude() const
{
    return DirectX::XMVector4Length(value.xmv).m128_f32[0];
}

Color XmMathPlus::Color::GetGreyscale() const
{
    float m = Magnitude();
    return *this / m;
}

Color XmMathPlus::Color::Lerp(const Color& a, const Color& b, Real t)
{
    if (t < 0.0f)
    {
        t = 0.0f;
    }
    else if (t > 1.0f)
    {
        t = 1.0f;
    }

    return Color::LerpUnclamped(a, b, t);
}

Color XmMathPlus::Color::LerpUnclamped(const Color& a, const Color& b, Real t)
{
    return DirectX::XMVectorLerp(a, b, t);

    //Color delta = b - a;
    //return a + delta * t;
}

Color XmMathPlus::operator*(Real scala, const Color& color)
{
    return color * scala;
}
