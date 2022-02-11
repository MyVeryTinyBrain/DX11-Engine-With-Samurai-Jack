#pragma once

#include "Real.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API V2 : public Real2
	{
		// Constructors ================================================================================

		inline V2() : Real2() {};

		inline V2(Real x, Real y) : Real2(x, y) {};

		inline V2(const V2& other) : Real2(other) {};

		inline V2(const DirectX::FXMVECTOR xmvector) : Real2(xmvector) {};

		inline V2(const DirectX::XMFLOAT2 xmfloat2) : Real2(xmfloat2) {};

		inline V2(const Real2& other) : Real2(other.x, other.y) {};

		inline V2(const Real3& other) : Real2(other.x, other.y) {};

		inline V2(const Real4& other) : Real2(other.x, other.y) {};

		// Operators ===================================================================================

		V2& operator += (const V2& rhs);

		V2& operator -= (const V2& rhs);

		V2& operator *= (Real rhs);

		V2& operator /= (Real rhs);

		V2 operator- () const;

		V2 operator+ () const;

		V2 operator + (const V2& rhs) const;

		V2 operator - (const V2& rhs) const;

		V2 operator * (Real rhs) const;

		V2 operator / (Real rhs)  const;

		bool operator == (const V2& rhs)  const;

		bool operator != (const V2& rhs)  const;

		operator Real2() const;

		operator Real3() const;

		operator Real4() const;

		// Member Variables	============================================================================

		Real SqrMagnitude() const;

		Real Magnitude() const;

		V2 Normalized() const;

		_declspec(property(get = SqrMagnitude)) Real sqrMagnitude;
		_declspec(property(get = Magnitude)) Real magnitude;
		_declspec(property(get = Normalized)) V2 normalized;

		// Member Functions ============================================================================

		void Normalize();

		// Static Functions ============================================================================

		static V2 Scale(const V2& a, const V2& b);

		static Real Dot(const V2& a, const V2& b);

		static Real Cross(const V2& lhs, const V2& rhs);

		static V2 Lerp(const V2& a, const V2& b, Real t);

		static V2 LerpUnclmaped(const V2& a, const V2& b, Real t);

		static V2 Project(const V2& vector, const V2& onNormal);

		static V2 Reflect(const V2& inDirection, const V2& inNormal);

		static V2 Sliding(const V2& inDirection, const V2& inNormal);

		static Real Cos(const V2& from, const V2& to);

		static Real Radian(const V2& from, const V2& to);

		static Real Angle(const V2& from, const V2& to);

		static Real SignedAngle(const V2& from, const V2& to);

		static Real Angle180(const V2& dir);

		static Real Angle360(const V2& dir);

		static Real Distance(const V2& a, const V2& b);

		static Real SqrDistance(const V2& a, const V2& b);

		static V2 Abs(const V2& v);

		static V2 Min(const V2& a, const V2& b);

		static V2 Max(const V2& a, const V2& b);

		static Real MinElement(const V2& v);

		static Real MaxElement(const V2& v);

		static Real MinAbsElement(const V2& v);

		static Real MaxAbsElement(const V2& v);

		// Static Variables	============================================================================

		inline static V2 up() { return V2(0, +1); };

		inline static V2 down() { return V2(0, -1); };

		inline static V2 left() { return V2(-1, 0); };

		inline static V2 right() { return V2(+1, 0); };

		inline static V2 zero() { return V2(0, 0); };

		inline static V2 one() { return V2(1, 1); };
	};

	XMMATHPLUS_API V2 operator * (Real scale, const V2& vector);
}