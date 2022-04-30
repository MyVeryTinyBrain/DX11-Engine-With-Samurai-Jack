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

XmMathPlus::Color::operator b128() const
{
    b128 B;

    uint32 dwR = (r >= 1.0f) ? 0xFFFFFFFF : ((r <= 0.0f) ? 0x00000000 : ((uint16)(r * 4294967295.0f + 0.5f)));
    uint32 dwG = (g >= 1.0f) ? 0xFFFFFFFF : ((g <= 0.0f) ? 0x00000000 : ((uint16)(g * 4294967295.0f + 0.5f)));
    uint32 dwB = (b >= 1.0f) ? 0xFFFFFFFF : ((b <= 0.0f) ? 0x00000000 : ((uint16)(b * 4294967295.0f + 0.5f)));
    uint32 dwA = (a >= 1.0f) ? 0xFFFFFFFF : ((a <= 0.0f) ? 0x00000000 : ((uint16)(a * 4294967295.0f + 0.5f)));

    B.u32[0] = dwR;
    B.u32[1] = dwG;
    B.u32[2] = dwB;
    B.u32[3] = dwA;

    return B;
}

XmMathPlus::Color::operator uint64() const
{
    uint16 B[4] = {};

    uint16 dwR = (r >= 1.0f) ? 0xFFFF : ((r <= 0.0f) ? 0x0000 : ((uint16)(r * 65535.0f + 0.5f)));
    uint16 dwG = (g >= 1.0f) ? 0xFFFF : ((g <= 0.0f) ? 0x0000 : ((uint16)(g * 65535.0f + 0.5f)));
    uint16 dwB = (b >= 1.0f) ? 0xFFFF : ((b <= 0.0f) ? 0x0000 : ((uint16)(b * 65535.0f + 0.5f)));
    uint16 dwA = (a >= 1.0f) ? 0xFFFF : ((a <= 0.0f) ? 0x0000 : ((uint16)(a * 65535.0f + 0.5f)));

    B[0] = dwR;
    B[1] = dwG;
    B[2] = dwB;
    B[3] = dwA;

    uint64 u64;
    memcpy(&u64, B, sizeof(uint64));
    return u64;
}

XmMathPlus::Color::operator uint32() const
{
    // AABBGGRR

    Byte dwR = (r >= 1.0f) ? 0xFF : ((r <= 0.0f) ? 0x00 : ((Byte)(r * 255.0f + 0.5f)));
    Byte dwG = (g >= 1.0f) ? 0xFF : ((g <= 0.0f) ? 0x00 : ((Byte)(g * 255.0f + 0.5f)));
    Byte dwB = (b >= 1.0f) ? 0xFF : ((b <= 0.0f) ? 0x00 : ((Byte)(b * 255.0f + 0.5f)));
    Byte dwA = (a >= 1.0f) ? 0xFF : ((a <= 0.0f) ? 0x00 : ((Byte)(a * 255.0f + 0.5f)));

    return (dwA << 24) | (dwB << 16) | (dwG << 8) | dwR;
}

XmMathPlus::Color::operator uint16() const
{
    Byte dwR = (r >= 1.0f) ? 0xF : r <= 0.0f ? 0x0 : (Byte)(r * 15.0f + 0.05f);
    Byte dwG = (g >= 1.0f) ? 0xF : g <= 0.0f ? 0x0 : (Byte)(g * 15.0f + 0.05f);
    Byte dwB = (b >= 1.0f) ? 0xF : b <= 0.0f ? 0x0 : (Byte)(b * 15.0f + 0.05f);
    Byte dwA = (a >= 1.0f) ? 0xF : a <= 0.0f ? 0x0 : (Byte)(a * 15.0f + 0.05f);

    return (dwA << 12) | (dwB << 8) | (dwG << 4) | dwR;
}

XmMathPlus::Color::operator uint8() const
{
    Byte dwR = (r >= 1.0f) ? 0x3 : r <= 0.0f ? 0x0 : (Byte)(r * 3.0f + 0.01f);
    Byte dwG = (g >= 1.0f) ? 0x3 : g <= 0.0f ? 0x0 : (Byte)(g * 3.0f + 0.01f);
    Byte dwB = (b >= 1.0f) ? 0x3 : b <= 0.0f ? 0x0 : (Byte)(b * 3.0f + 0.01f);
    Byte dwA = (a >= 1.0f) ? 0x3 : a <= 0.0f ? 0x0 : (Byte)(a * 3.0f + 0.01f);

    return (dwA << 6) | (dwB << 4) | (dwG << 2) | dwR;
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
