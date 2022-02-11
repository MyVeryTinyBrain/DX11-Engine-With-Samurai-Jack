#pragma once

#include "Real.h"
#include "Ray.h"
#include "V3.h"
#include "Q.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API Plane
	{
		inline Plane() {}
		Plane(const Real3& inNormal, const Real3& inPoint);
		Plane(const Real3& inNormal, Real d);
		Plane(const Real3& a, const Real3& b, const Real3& c);
		inline Plane(Real a, Real b, Real c, Real d) : v(a, b, c, d) {}

		Real3 GetNormal() const;
		void SetNormal(const Real3& normal);

		inline Real GetA() const { return v.x; };
		inline Real GetB() const { return v.y; };
		inline Real GetC() const { return v.z; };
		inline Real GetD() const { return v.w; };

		inline void SetA(Real value) { v.x = value; };
		inline void SetB(Real value) { v.y = value; };
		inline void SetC(Real value) { v.z = value; };
		inline void SetD(Real value) { v.w = value; };

		Plane Flipped() const;

		void Flip();

		Plane Normalized() const;

		void Normalize();

		_declspec(property(get = GetNormal, put = SetNormal)) Real3 normal;
		_declspec(property(get = GetA, put = SetA)) Real a;
		_declspec(property(get = GetB, put = SetB)) Real b;
		_declspec(property(get = GetC, put = SetC)) Real c;
		_declspec(property(get = GetD, put = SetD)) Real d;
		_declspec(property(get = Flipped)) Plane flipped;
		_declspec(property(get = Normalized)) Plane normalized;

		Real GetDistanceToPoint(const Real3& point) const;

		Real GetSide(const Real3& point) const;

		bool Raycast(const Ray& ray, Real& out_distance) const;
		bool RaycastIn90Degree(const Ray& ray, Real& out_distance) const;

		bool Intersect(const V3& a, const V3& b, V3& out_hit) const;

		bool SameSide(const Real3& inPt0, const Real3& inPt1) const;

		Real3 ClosetPoint(const Real3& point) const;

		void Transform(const V3& scale, const Q& rotation, const V3& position);
		void Transform(const M4& transformation);

	public:

		// (v.x, v.y, v.z) = Normal vector of plane
		// v.w = Negative Distance from origin(0, 0, 0) point
		Real4 v;
	};
}
