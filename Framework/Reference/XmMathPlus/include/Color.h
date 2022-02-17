#pragma once

#include "Real.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API Color
	{
		// Constructors ================================================================================

		inline Color() : value() {}

		inline Color(Real r, Real g, Real b, Real a) : value(r, g, b, a) {}

		inline Color(DirectX::FXMVECTOR xmvector) : value(xmvector) {}

		inline Color(const Real4& other) : value(other) {}

		explicit Color(uint dword);

		// Operators ===================================================================================

		Color& operator += (const Color& rhs);

		Color& operator -= (const Color& rhs);

		Color& operator *= (Real rhs);

		Color& operator /= (Real rhs);

		Color operator + () const;

		Color operator - () const;

		Color operator + (const Color& rhs) const;

		Color operator - (const Color& rhs) const;

		Color operator * (Real rhs) const;

		Color operator / (Real rhs) const;

		bool operator == (const Color& rhs) const;

		bool operator != (const Color& rhs) const;

		// RGBA Hex color
		operator uint() const;

		operator Real4() const;

		operator DirectX::XMVECTOR() const;

		// Member Functions ============================================================================

		// Member Variables	============================================================================

		uint ToRGBA() const;

		uint ToARGB() const;

		Real Magnitude() const;

		Color GetGreyscale() const;

		_declspec(property(get = ToRGBA)) uint rgba;
		_declspec(property(get = ToARGB)) uint argb;
		_declspec(property(get = Magnitude)) Real magnitude;
		_declspec(property(get = GetGreyscale)) Color greyscale;

		// Static Functions ============================================================================

		static Color Lerp(const Color& a, const Color& b, Real t);

		static Color LerpUnclamped(const Color& a, const Color& b, Real t);

		// Static Variables	============================================================================

		inline static Color clear() { return Color(0, 0, 0, 0); }

		inline static Color black() { return Color(0, 0, 0, 1); }

		inline static Color white() { return Color(1, 1, 1, 1); }

		inline static Color red() { return Color(1, 0, 0, 1); }

		inline static Color green() { return Color(0, 1, 0, 1); }

		inline static Color blue() { return Color(0, 0, 1, 1); }

		inline static Color cyan() { return Color(0, 1, 1, 1); }

		inline static Color magenta() { return Color(1, 0, 1, 1); }

		inline static Color grey() { return Color(0.5f, 0.5f, 0.5f, 1); }

		inline static Color yellow() { return Color(1, 0.92f, 0.016f, 1); }

		// Member Variables	============================================================================

	public:

		inline Real GetR() const { return value.GetX(); }
		inline Real GetG() const { return value.GetY(); }
		inline Real GetB() const { return value.GetZ(); }
		inline Real GetA() const { return value.GetW(); }

		inline void SetR(Real v) { value.SetX(v); }
		inline void SetG(Real v) { value.SetY(v); }
		inline void SetB(Real v) { value.SetZ(v); }
		inline void SetA(Real v) { value.SetW(v); }

		_declspec(property(get = GetR, put = SetR)) Real r;
		_declspec(property(get = GetG, put = SetG)) Real g;
		_declspec(property(get = GetB, put = SetB)) Real b;
		_declspec(property(get = GetA, put = SetA)) Real a;

		Real4 value;
	};

	Color operator * (Real scala, const Color& color);
}

