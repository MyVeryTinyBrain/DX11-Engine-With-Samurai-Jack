#pragma once

namespace XmMathPlus
{
	struct XMMATHPLUS_API Real2
	{
		inline Real2() { memset(&xmv, 0, sizeof(xmv)); }

		inline Real2(DirectX::FXMVECTOR xmvector)
		{
			xmv = xmvector;
			xmv = DirectX::XMVectorSetZ(xmv, 0);
			xmv = DirectX::XMVectorSetW(xmv, 0);
		}

		inline Real2(const DirectX::XMFLOAT2& xmfloat2)
		{
			xmv = DirectX::XMLoadFloat2(&xmfloat2);
		}

		inline Real2(Real x, Real y)
		{
			xmv = DirectX::XMVectorSet(x, y, 0, 0);
		}

		inline Real2(const Real2& other) : xmv(other.xmv) {};

		inline operator DirectX::XMVECTOR() const { return xmv; }

		inline operator DirectX::XMFLOAT2() const { DirectX::XMFLOAT2 v; DirectX::XMStoreFloat2(&v, xmv); return v; }

		inline Real2& operator += (const Real2& rhs) { xmv = DirectX::XMVectorAdd(xmv, rhs.xmv); return *this; }

		inline Real2& operator -= (const Real2& rhs) { xmv = DirectX::XMVectorSubtract(xmv, rhs.xmv); return *this; }

		inline Real2& operator *= (Real rhs) { xmv = DirectX::XMVectorScale(xmv, rhs); return *this; }

		inline Real2& operator /= (Real rhs) 
		{
			XMVECTOR vs = DirectX::XMVectorReplicate(rhs);
			xmv = DirectX::XMVectorDivide(xmv, vs);
			return *this;
		}

		inline Real2 operator + (const Real2& rhs) const { return Real2(DirectX::XMVectorAdd(xmv, rhs.xmv)); }

		inline Real2 operator - (const Real2& rhs) const { return Real2(DirectX::XMVectorSubtract(xmv, rhs.xmv)); }

		inline Real2 operator * (Real rhs) const { return Real2(DirectX::XMVectorScale(xmv, rhs)); }

		inline Real2 operator / (Real rhs) const
		{
			XMVECTOR vs = DirectX::XMVectorReplicate(rhs);
			return Real2(DirectX::XMVectorDivide(xmv, vs));
		}

		inline Real2 operator + () const { return *this; }

		inline Real2 operator - () const { return Real2(DirectX::XMVectorNegate(xmv)); }

		inline bool operator == (const Real2& rhs) const { return DirectX::XMVector2Equal(xmv, rhs.xmv); }

		inline bool operator != (const Real2& rhs) const { return !(*this == rhs); }

		inline Real GetX() const { return DirectX::XMVectorGetX(xmv); }

		inline Real GetY() const { return DirectX::XMVectorGetY(xmv); }

		inline void SetX(Real value) { xmv = DirectX::XMVectorSetX(xmv, value); }

		inline void SetY(Real value) { xmv = DirectX::XMVectorSetY(xmv, value); }

		union
		{
			struct
			{
				float x;
				float y;
				float unused[2];
			};
			struct
			{
				DirectX::XMVECTOR xmv;
			};
		};
	};

	struct XMMATHPLUS_API Real3
	{
		inline Real3() { memset(&xmv, 0, sizeof(xmv)); }

		inline Real3(DirectX::FXMVECTOR xmvector)
		{
			xmv = xmvector;
			xmv = DirectX::XMVectorSetW(xmvector, 0);
		}

		inline Real3(const DirectX::XMFLOAT3& xmfloat3)
		{
			xmv = DirectX::XMLoadFloat3(&xmfloat3);
		}

		inline Real3(Real x, Real y, Real z)
		{
			xmv = DirectX::XMVectorSet(x, y, z, 0);
		}

		inline Real3(const Real3& other) : xmv(other.xmv) {};

		inline operator DirectX::XMVECTOR() const { return xmv; }

		inline operator DirectX::XMFLOAT3() const { DirectX::XMFLOAT3 v; DirectX::XMStoreFloat3(&v, xmv); return v; }

		inline Real3& operator += (const Real3& rhs) { xmv = DirectX::XMVectorAdd(xmv, rhs.xmv); return *this; }

		inline Real3& operator -= (const Real3& rhs) { xmv = DirectX::XMVectorSubtract(xmv, rhs.xmv); return *this; }

		inline Real3& operator *= (Real rhs) { xmv = DirectX::XMVectorScale(xmv, rhs); return *this; }

		inline Real3& operator /= (Real rhs)
		{
			XMVECTOR vs = DirectX::XMVectorReplicate(rhs);
			xmv = DirectX::XMVectorDivide(xmv, vs);
			return *this;
		}

		inline Real3 operator + (const Real3& rhs) const { return Real3(DirectX::XMVectorAdd(xmv, rhs.xmv)); }

		inline Real3 operator - (const Real3& rhs) const { return Real3(DirectX::XMVectorSubtract(xmv, rhs.xmv)); }

		inline Real3 operator * (Real rhs) const { return Real3(DirectX::XMVectorScale(xmv, rhs)); }

		inline Real3 operator / (Real rhs) const
		{
			XMVECTOR vs = DirectX::XMVectorReplicate(rhs);
			return Real3(DirectX::XMVectorDivide(xmv, vs));
		}

		inline Real3 operator + () const { return *this; }

		inline Real3 operator - () const { return Real3(DirectX::XMVectorNegate(xmv)); }

		inline bool operator == (const Real3& rhs) const { return DirectX::XMVector3Equal(xmv, rhs.xmv); }

		inline bool operator != (const Real3& rhs) const { return !(*this == rhs); }

		inline Real GetX() const { return DirectX::XMVectorGetX(xmv); }

		inline Real GetY() const { return DirectX::XMVectorGetY(xmv); }

		inline Real GetZ() const { return DirectX::XMVectorGetZ(xmv); }

		inline void SetX(Real value) { xmv = DirectX::XMVectorSetX(xmv, value); }

		inline void SetY(Real value) { xmv = DirectX::XMVectorSetY(xmv, value); }

		inline void SetZ(Real value) { xmv = DirectX::XMVectorSetZ(xmv, value); }

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float unused;
			};
			struct
			{
				DirectX::XMVECTOR xmv;
			};
		};
	};

	struct XMMATHPLUS_API Real4
	{
		inline Real4() { memset(&xmv, 0, sizeof(xmv)); }

		inline Real4(DirectX::FXMVECTOR xmvector) : xmv(xmvector) {}

		inline Real4(const DirectX::XMFLOAT4& xmfloat4)
		{
			xmv = DirectX::XMLoadFloat4(&xmfloat4);
		}

		inline Real4(Real x, Real y, Real z, Real w)
		{
			xmv = DirectX::XMVectorSet(x, y, z, w);
		}

		inline Real4(const Real4& other) : xmv(other.xmv) {};

		inline operator DirectX::XMVECTOR() const { return xmv; }

		inline operator DirectX::XMFLOAT4() const { DirectX::XMFLOAT4 v; DirectX::XMStoreFloat4(&v, xmv); return v; }

		inline Real4& operator += (const Real4& rhs) { xmv = DirectX::XMVectorAdd(xmv, rhs.xmv); return *this; }

		inline Real4& operator -= (const Real4& rhs) { xmv = DirectX::XMVectorSubtract(xmv, rhs.xmv); return *this; }

		inline Real4& operator *= (Real rhs) { xmv = DirectX::XMVectorScale(xmv, rhs); return *this; }

		inline Real4& operator /= (Real rhs)
		{
			XMVECTOR vs = DirectX::XMVectorReplicate(rhs);
			xmv = DirectX::XMVectorDivide(xmv, vs);
			return *this;
		}

		inline Real4 operator + (const Real4& rhs) const { return Real4(DirectX::XMVectorAdd(xmv, rhs.xmv)); }

		inline Real4 operator - (const Real4& rhs) const { return Real4(DirectX::XMVectorSubtract(xmv, rhs.xmv)); }

		inline Real4 operator * (Real rhs) const { return Real4(DirectX::XMVectorScale(xmv, rhs)); }

		inline Real4 operator / (Real rhs) const
		{
			XMVECTOR vs = DirectX::XMVectorReplicate(rhs);
			return Real4(DirectX::XMVectorDivide(xmv, vs));
		}

		inline Real4 operator + () const { return *this; }

		inline Real4 operator - () const { return Real4(DirectX::XMVectorNegate(xmv)); }

		inline bool operator == (const Real4& rhs) const { return DirectX::XMVector4Equal(xmv, rhs.xmv); }

		inline bool operator != (const Real4& rhs) const { return !(*this == rhs); }

		inline Real GetX() const { return DirectX::XMVectorGetX(xmv); }

		inline Real GetY() const { return DirectX::XMVectorGetY(xmv); }

		inline Real GetZ() const { return DirectX::XMVectorGetZ(xmv); }

		inline Real GetW() const { return DirectX::XMVectorGetW(xmv); }

		inline void SetX(Real value) { xmv = DirectX::XMVectorSetX(xmv, value); }

		inline void SetY(Real value) { xmv = DirectX::XMVectorSetY(xmv, value); }

		inline void SetZ(Real value) { xmv = DirectX::XMVectorSetZ(xmv, value); }

		inline void SetW(Real value) { xmv = DirectX::XMVectorSetW(xmv, value); }

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			struct
			{
				DirectX::XMVECTOR xmv;
			};
		};
	};

	struct XMMATHPLUS_API Real4x4
	{
		inline Real4x4() :
			m00(0), m01(0), m02(0), m03(0),
			m10(0), m11(0), m12(0), m13(0),
			m20(0), m21(0), m22(0), m23(0),
			m30(0), m31(0), m32(0), m33(0)
		{
		}

		inline Real4x4(
			Real _00, Real _01, Real _02, Real _03,
			Real _10, Real _11, Real _12, Real _13,
			Real _20, Real _21, Real _22, Real _23,
			Real _30, Real _31, Real _32, Real _33) :
			xmm(_00, _01, _02, _03,
				_10, _11, _12, _13,
				_20, _21, _22, _23,
				_30, _31, _32, _33)
		{
		}

		inline Real4x4(const Real4x4& other) :
			xmm(other.xmm)
		{
		}

		inline Real4x4(DirectX::FXMMATRIX xmmatrix) : xmm(xmmatrix) {}

		inline Real4x4(const DirectX::XMFLOAT4X4& xmfloat4x4) { xmm = DirectX::XMLoadFloat4x4(&xmfloat4x4); }

		inline Real4x4(
			DirectX::FXMVECTOR row0, 
			DirectX::FXMVECTOR row1,
			DirectX::FXMVECTOR row2,
			DirectX::XMVECTOR row3) :
			xmm(row0, row1, row2, row3)
		{

		}

		inline operator DirectX::XMMATRIX() const { return xmm; }

		inline operator DirectX::XMFLOAT4X4() const { DirectX::XMFLOAT4X4 m; DirectX::XMStoreFloat4x4(&m, xmm); return m; }

		inline Real4x4 operator * (Real rhs) const { return Real4x4(xmm * rhs); }

		inline bool operator == (const Real4x4& rhs) const
		{
			return
				DirectX::XMVector4Equal(xmm.r[0], rhs.xmm.r[0]) &&
				DirectX::XMVector4Equal(xmm.r[1], rhs.xmm.r[1]) &&
				DirectX::XMVector4Equal(xmm.r[2], rhs.xmm.r[2]) &&
				DirectX::XMVector4Equal(xmm.r[3], rhs.xmm.r[3]);
		}

		inline bool operator != (const Real4x4& rhs) const
		{
			return !(*this == rhs);
		}

		union
		{
			struct
			{
				Real			m[4][4];
			};
			struct
			{
				Real			m00, m01, m02, m03;
				Real			m10, m11, m12, m13;
				Real			m20, m21, m22, m23;
				Real			m30, m31, m32, m33;
			};
			struct
			{
				Real			a[16];
			};
			struct
			{
				DirectX::XMVECTOR row[4];
			};
			struct
			{
				DirectX::XMMATRIX xmm;
			};
		};
	};
}