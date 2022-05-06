#pragma once

#include "Real.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API V3 : public Real3
	{		
		// Constructors ================================================================================

		inline V3() : Real3() {};

		inline V3(Real x, Real y, Real z) : Real3(x, y, z) {};

		inline V3(const V3& other) : Real3(other) {};

		inline V3(const DirectX::FXMVECTOR xmvector) : Real3(xmvector) {};

		inline V3(const DirectX::XMFLOAT3 xmfloat3) : Real3(xmfloat3) {};

		inline V3(const Real2& other) : Real3(other.x, other.y, 0) {};

		inline V3(const Real3& other) : Real3(other.x, other.y, other.z) {};

		inline V3(const Real4& other) : Real3(other.x, other.y, other.z) {};

		// Operators ===================================================================================

		V3& operator += (const V3& rhs);

		V3& operator -= (const V3& rhs);

		V3& operator *= (Real rhs);

		V3& operator /= (Real rhs);

		V3 operator- () const;

		V3 operator+ () const;

		V3 operator + (const V3& rhs) const;

		V3 operator - (const V3& rhs) const;

		V3 operator * (Real rhs) const;

		V3 operator / (Real rhs)  const;

		bool operator == (const V3& rhs)  const;

		bool operator != (const V3& rhs)  const;

		operator Real2() const;

		operator Real3() const;

		operator Real4() const;

		// Member Variables	============================================================================

		Real SqrMagnitude() const;

		Real Magnitude() const;

		V3 Normalized() const;

		_declspec(property(get = SqrMagnitude)) Real sqrMagnitude;
		_declspec(property(get = Magnitude)) Real magnitude;
		_declspec(property(get = Normalized)) V3 normalized;

		// Member Functions ============================================================================

		void Normalize();

		// Static Functions ============================================================================

		static V3 Scale(const V3& a, const V3& b);

		static Real Dot(const V3& a, const V3& b);

		static V3 Cross(const V3& lhs, const V3& rhs);

		static V3 Lerp(const V3& a, const V3& b, Real t);

		static V3 LerpUnclmaped(const V3& a, const V3& b, Real t);

		static V3 Project(const V3& vector, const V3& onNormal);

		static V3 Reflect(const V3& inDirection, const V3& inNormal);

		static V3 Sliding(const V3& inDirection, const V3& inNormal);

		static Real Cos(const V3& from, const V3& to);

		static Real Radian(const V3& from, const V3& to);

		static Real Angle(const V3& from, const V3& to);

		// 두 벡터 사이의 가능한 두 각도 중 더 작은 각도가 반환되므로 결과는 -180 ~ +180도 이내입니다.
		// 두 벡터 사이의 측정된 각도는 시계 방향에서 양수, 시계 반대 방향에서 음수입니다.
		static Real SignedAngle(const V3& from, const V3& to, const V3& axis);

		static Real Distance(const V3& a, const V3& b);

		static Real SqrDistance(const V3& a, const V3& b);

		static V3 Abs(const V3& v);

		static V3 Min(const V3& a, const V3& b);

		static V3 Max(const V3& a, const V3& b);

		static Real MinElement(const V3& v);

		static Real MaxElement(const V3& v);

		static Real MinAbsElement(const V3& v);

		static Real MaxAbsElement(const V3& v);

		static V3 CatMulRom(const V3& a, const V3& b, const V3& c, const V3& d, Real t);

		// Static Variables	============================================================================

		inline static V3 up() { return V3(0, +1, 0); };

		inline static V3 down() { return V3(0, -1, 0); };

		inline static V3 left() { return V3(-1, 0, 0); };

		inline static V3 right() { return V3(+1, 0, 0); };

		inline static V3 forward() { return V3(0, 0, +1); };

		inline static V3 back() { return V3(0, 0, -1); };

		inline static V3 zero() { return V3(0, 0, 0); };

		inline static V3 one() { return V3(1, 1, 1); };
	};

	XMMATHPLUS_API V3 operator * (Real scale, const V3& vector);
}

