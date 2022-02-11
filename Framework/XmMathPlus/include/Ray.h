#pragma once

#include "V3.h"
#include "Q.h"
#include "M4.h"

namespace XmMathPlus
{
	struct V3;
	struct Q;
	struct M4;

	struct XMMATHPLUS_API Ray
	{
		inline Ray() : origin(), direction() {}

		inline Ray(const V3& origin, const V3& direction) : origin(origin), direction(direction) {}

		inline V3 GetPoint(Real distance) const { return origin + direction * distance; }

		void Transform(const V3& scale, const Q& rotation, const V3& position);

		void Transform(const M4& transformation);

		V3 origin;
		V3 direction;
	};
}
