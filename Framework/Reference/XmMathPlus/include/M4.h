#pragma once

#include "Real.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API M4 : public Real4x4
	{
		// Constructors ================================================================================

		inline M4() : Real4x4() {}

		inline M4(
			Real _11, Real _12, Real _13, Real _14,
			Real _21, Real _22, Real _23, Real _24,
			Real _31, Real _32, Real _33, Real _34,
			Real _41, Real _42, Real _43, Real _44) :
			Real4x4(
				_11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44)
		{
		}

		inline M4(const Real4x4& other) : Real4x4(other) {}

		inline M4(DirectX::FXMMATRIX other) : Real4x4(other) {}

		inline M4(const M4& other) : Real4x4(other) {}

		// Operators ===================================================================================

		M4& operator *= (const M4& rhs);

		M4& operator += (const M4& rhs);

		M4& operator -= (const M4& rhs);

		M4& operator *= (Real rhs);

		M4& operator /= (Real rhs);

		M4 operator + () const;

		M4 operator - () const;

		M4 operator * (const M4& rhs) const;

		M4 operator + (const M4& rhs) const;

		M4 operator - (const M4& rhs) const;

		M4 operator * (Real rhs) const;

		M4 operator / (Real rhs) const;

		bool operator == (const M4& rhs) const;

		bool operator != (const M4& rhs) const;

		Real4 operator * (const Real4& rhs) const;

		operator Real4x4() const;

		operator DirectX::XMMATRIX() const;

		// Member Functions ============================================================================

		Real3 GetTranslation() const;

		Real4 GetRotation() const;

		Real4 GetEulerAngles() const;

		Real3 GetScale() const;

		Real4 GetRow(int i) const;

		Real4 GetColumn(int i) const;

		void SetRow(int i, const Real4& value);

		void SetColumn(int i, const Real4& value);

		Real3 MultiplyPoint(const Real3& lhs) const;

		Real3 MultiplyVector(const Real3& lhs) const;

		// Member Variables	============================================================================

		Real GetDeterminant() const;

		M4 GetInversed() const;

		void Inverse();

		bool IsIdentity() const;

		M4 GetTransposed() const;

		void Transpose();

		_declspec(property(get = GetDeterminant)) Real determinant;
		_declspec(property(get = GetInversed)) M4 inversed;
		_declspec(property(get = IsIdentity)) bool isIdentity;
		_declspec(property(get = GetTransposed)) M4 transposed;

		// Static Functions ============================================================================

		static M4 MultiplyMatrix(const M4& lhs, const M4& rhs);

		static Real4 MultiplyLHSReal4(const Real4& lhs, const M4& rhs);

		static Real4 MultiplyRHSReal4(const M4& lhs, const Real4& rhs);

		// Static Variables	============================================================================

		inline static M4 zero() { return M4(); }
	
		static M4 identity();

		static M4 Translate(const Real3& translation);

		static M4 Rotate(const Real4& quaternion);

		static M4 Rotate(const Real3& eulerAngles);

		static M4 Scale(const Real3& scale);

		static M4 SRT(const Real3& scale, const Real4& quaternion, const Real3& translation);

		static M4 SRT(const Real3& scale, const Real3& eulerAngles, const Real3& translation);
	};

	XMMATHPLUS_API M4 operator * (Real scala, const M4& matrix);

	XMMATHPLUS_API Real4 operator * (const Real4& lhs, const M4& rhs);
}

