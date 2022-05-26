#pragma once

#include "Real.h"
#include "V3.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API Q : public Real4
	{
		// Constructors ================================================================================

		inline Q() : Real4() {};

		inline Q(Real x, Real y, Real z, Real w) : Real4(x, y, z, w) {}

		inline Q(const V3& vector, Real w) : Real4(vector.x, vector.y, vector.z, w) {};

		inline Q(const Q& other) : Real4(other) {};

		inline Q(DirectX::FXMVECTOR xmvector) : Real4(xmvector) {}

		inline Q(const DirectX::XMFLOAT4 xmfloat4) : Real4(xmfloat4) {};

		inline Q(const Real4& other) : Real4(other.x, other.y, other.z, other.w) {};

		// Operators ===================================================================================

		Q& operator += (const Q& rhs);

		Q& operator -= (const Q& rhs);

		Q& operator *= (const Q& rhs);

		Q& operator *= (Real rhs);

		Q& operator /= (Real rhs);

		Q  operator + () const;

		Q  operator - () const;

		Q operator + (const Q& rhs) const;

		Q operator - (const Q& rhs) const;

		Q operator * (const Q& rhs) const;

		Q operator * (Real rhs) const;

		V3 operator * (const V3& vector) const;

		Q operator / (Real rhs) const;

		bool operator == (const Q& rhs) const;

		bool operator != (const Q& rhs) const;

		operator Real4() const;

		// Member Functions ============================================================================

		V3 MultiplyVector(const V3& vector) const;

		V3 GetAxis() const;

		void SetAxis(const V3& vector);

		void Normalize();

		// Member Variables ============================================================================

		Q Conjugate() const;

		Q Inverse() const;

		Real SqrMagnitude() const;

		Real Magnitude() const;

		Q Normalized() const;

		V3 EulerAngles() const;

		Real4x4 GetRowRotationMatrix() const;

		Real4x4 GetColumnRotationMatrix() const;

		_declspec(property(get = Conjugate)) Q conjugate;
		_declspec(property(get = Inverse)) Q inversed;
		_declspec(property(get = SqrMagnitude)) Real sqrMagnitude;
		_declspec(property(get = Magnitude)) Real magnitude;
		_declspec(property(get = Normalized)) Q normalized;
		_declspec(property(get = EulerAngles)) V3 eulerAngles;
		_declspec(property(get = GetRowRotationMatrix)) Real4x4 rowRotationMatrix;
		_declspec(property(get = GetColumnRotationMatrix)) Real4x4 columnRotationMatrix;

		// Static Functions ============================================================================

		static Real Dot(const Q& lhs, const Q& rhs);

		static Real Radian(const Q& lhs, const Q& rhs);

		static Real Angle(const Q& lhs, const Q& rhs);

		static Q Euler(Real x, Real y, Real z);

		static Q Euler(const V3& euler);

		static Q AxisAngle(const V3& axis, Real angle);

		static Q FromToRotation(const V3& from, const V3& to);

		static Q SLerp(const Q& a, const Q& b, Real t);

		static Q SLerpUnclamped(Q a, Q b, Real t);

		static Q Lerp(const Q& a, const Q& b, Real t);

		static Q LerpUnclamped(const Q& a, const Q& b, Real t);

		static Q RotateTowards(const Q& from, const Q& to, Real maxDegreeDelta);

		static Q LookRotation(const V3& forward, V3& up);

		static Q RightHandedToLeftHanded(const Q& rh);

		// Static Variables ============================================================================

		inline static Q identity() { return Q(0, 0, 0, 1); };
	};

	XMMATHPLUS_API Q operator * (Real scala, const Q& quaternion);
}

