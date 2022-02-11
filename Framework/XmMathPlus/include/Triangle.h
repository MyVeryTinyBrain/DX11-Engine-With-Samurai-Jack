#pragma once

#include "Ray.h"
#include "V3.h"
#include "Q.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API Triangle
	{
		inline Triangle() {}

		inline explicit Triangle(const V3& P0, const V3& P1, const V3& P2) : p0(P0), p1(P1), p2(P2) {}

		inline explicit Triangle(const V3* Points) { memcpy(points, Points, sizeof(V3) * 3); }

		V3 GetNormal() const;

		V3 GetCenter() const;

		Real4 ToPlane() const;

		Real GetAngleOfP0() const;

		Real GetAngleOfP1() const;

		Real GetAngleOfP2() const;

		_declspec(property(get = GetNormal)) V3 normal;
		_declspec(property(get = GetCenter)) V3 center;
		_declspec(property(get = ToPlane)) Real4 plane;
		_declspec(property(get = GetAngleOfP0)) Real angleOfP0;
		_declspec(property(get = GetAngleOfP1)) Real angleOfP1;
		_declspec(property(get = GetAngleOfP2)) Real angleOfP2;

		bool ContainsInSweep(const V3& point) const;

		bool Raycast(const Ray& ray, Real& out_distance) const;
		bool RaycastIn90Degrees(const Ray& ray, Real& out_distance) const;

		inline bool Intersect(const Real4& plane) const { return DirectX::TriangleTests::Intersects(p0, p1, p2, plane); }
		inline bool Intersect(const Triangle& triangle) const { return DirectX::TriangleTests::Intersects(p0, p1, p2, triangle.p0, triangle.p1, triangle.p2); }

		void Transform(const V3& scale, const Q& rotation, const V3& position);
		void Transform(const M4& transformation);

		union
		{
			struct
			{
				V3 p0, p1, p2;
			};
			V3 points[3] = {};
		};
	};
}

