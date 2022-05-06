#pragma once

#include "Real.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API V4 : public Real4
	{
		// Constructors ================================================================================

		inline V4() : Real4() {};

		inline V4(Real x, Real y, Real z, Real w) : Real4(x, y, z, w) {};

		inline V4(const V4& other) : Real4(other) {};

		inline V4(const DirectX::FXMVECTOR xmvector) : Real4(xmvector) {};

		inline V4(const DirectX::XMFLOAT4 xmfloat4) : Real4(xmfloat4) {};

		inline V4(const Real2& other) : Real4(other.x, other.y, 0, 0) {};

		inline V4(const Real3& other) : Real4(other.x, other.y, other.z, 0) {};

		inline V4(const Real4& other) : Real4(other.x, other.y, other.z, other.w) {};

		// Operators ===================================================================================

		V4& operator += (const V4& rhs);

		V4& operator -= (const V4& rhs);

		V4& operator *= (Real rhs);

		V4& operator /= (Real rhs);

		V4 operator- () const;

		V4 operator+ () const;

		V4 operator + (const V4& rhs) const;

		V4 operator - (const V4& rhs) const;

		V4 operator * (Real rhs) const;

		V4 operator / (Real rhs)  const;

		bool operator == (const V4& rhs)  const;

		bool operator != (const V4& rhs)  const;

		operator Real2() const;

		operator Real3() const;

		operator Real4() const;

		// Member Variables	============================================================================

		Real SqrMagnitude() const;

		Real Magnitude() const;

		V4 Normalized() const;

		_declspec(property(get = SqrMagnitude)) Real sqrMagnitude;
		_declspec(property(get = Magnitude)) Real magnitude;
		_declspec(property(get = Normalized)) V4 normalized;

		// Member Functions ============================================================================

		void Normalize();

		// Static Functions ============================================================================

		static V4 Scale(const V4& a, const V4& b);

		static Real Dot(const V4& a, const V4& b);

		static V4 Lerp(const V4& a, const V4& b, Real t);

		static V4 LerpUnclmaped(const V4& a, const V4& b, Real t);

		static V4 Project(const V4& vector, const V4& onNormal);

		static V4 Reflect(const V4& inDirection, const V4& inNormal);

		static V4 Sliding(const V4& inDirection, const V4& inNormal);

		static Real Cos(const V4& from, const V4& to);

		static Real Radian(const V4& from, const V4& to);

		static Real Angle(const V4& from, const V4& to);

		static Real Distance(const V4& a, const V4& b);

		static Real SqrDistance(const V4& a, const V4& b);

		static V4 Abs(const V4& v);

		static V4 Min(const V4& a, const V4& b);

		static V4 Max(const V4& a, const V4& b);

		static Real MinElement(const V4& v);

		static Real MaxElement(const V4& v);

		static Real MinAbsElement(const V4& v);

		static Real MaxAbsElement(const V4& v);

		static V4 CatMulRom(const V4& a, const V4& b, const V4& c, const V4& d, Real t);

		// Static Variables	============================================================================

		inline static V4 zero() { return V4(0, 0, 0, 0); };

		inline static V4 one() { return V4(1, 1, 1, 1); };
	};

	XMMATHPLUS_API V4 operator * (Real scale, const V4& vector);
}

