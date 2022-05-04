#pragma once

#include "V2.h"
#include "V4.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API FRect
	{
		inline FRect() : Vector4() {}
		inline FRect(Real left, Real top, Real right, Real bottom) : Left(left), Right(right), Top(top), Bottom(bottom) {}
		inline FRect(const V2& leftTop, const V2& rightBottom) : Left(leftTop.x), Right(rightBottom.x), Top(leftTop.y), Bottom(rightBottom.y) {}
		inline FRect(const FRect& other) : Vector4(other.Vector4) {}
		inline FRect(FRect&& other) noexcept : Vector4(other.Vector4) { other.Vector4 = V4(); }

		inline FRect& operator = (const FRect& other) { this->Vector4 = other.Vector4; return *this; }
		inline bool operator == (const FRect& rhs) { return this->Vector4 == rhs.Vector4; }
		inline bool operator != (const FRect& rhs) { return this->Vector4 != rhs.Vector4; }

		void GetCorners(V2* out_arrCorners) const;

		union
		{
			V4 Vector4;
			struct
			{
				Real Left, Top, Right, Bottom;
			};
		};
	};
}
