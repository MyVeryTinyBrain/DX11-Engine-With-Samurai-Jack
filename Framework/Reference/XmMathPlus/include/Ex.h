#pragma once

namespace XmMathPlus
{
	namespace Ex
	{
		constexpr Real PI = ((Real)3.141592654f);

		constexpr Real TAU = PI * 2.0f;

		constexpr Real Epsilon = 1.192092896e-07F;

		constexpr Real Deg2Rad = (PI / 180.0f);

		constexpr Real Rad2Deg = (180.0f / PI);

		constexpr Real Infinity = ((Real)(1e+300 * 1e+300));

		constexpr Real NaN = ((Real)(Infinity * 0.0F));

		XMMATHPLUS_API Real Lerp(Real from, Real to, Real t);

		XMMATHPLUS_API Real LerpUnclmaped(Real from, Real to, Real t);

		XMMATHPLUS_API Real Clamp(Real value, Real min, Real max);

		XMMATHPLUS_API int Clamp(int value, int min, int max);

		XMMATHPLUS_API uint Clamp(uint value, uint min, uint max);

		XMMATHPLUS_API Real Clamp01(Real value, Real min, Real max);

		XMMATHPLUS_API int Clamp01(int value, int min, int max);

		XMMATHPLUS_API uint Clamp01(uint value, uint min, uint max);

		XMMATHPLUS_API Real Closet(Real value, Real min, Real max);

		XMMATHPLUS_API int Closet(int value, int min, int max);

		XMMATHPLUS_API uint Closet(uint value, uint min, uint max);

		XMMATHPLUS_API Real DeltaAngle(Real from, Real to);

		XMMATHPLUS_API Real Repeat(Real value, Real max);

		XMMATHPLUS_API int Repeat(int value, int max);

		XMMATHPLUS_API uint Repeat(uint value, uint max);

		XMMATHPLUS_API inline Real Max(Real a, Real b) { return a >= b ? a : b; }

		XMMATHPLUS_API inline int Max(int a, int b) { return a >= b ? a : b; }

		XMMATHPLUS_API inline uint Max(uint a, uint b) { return a >= b ? a : b; }

		XMMATHPLUS_API inline Real Min(Real a, Real b) { return a <= b ? a : b; }

		XMMATHPLUS_API inline int Min(int a, int b) { return a <= b ? a : b; }

		XMMATHPLUS_API inline uint Min(uint a, uint b) { return a <= b ? a : b; }

		XMMATHPLUS_API inline Real Abs(Real value) { return value < 0.0f ? -value : value; }

		XMMATHPLUS_API inline int Abs(int value) { return value < 0 ? -value : value; }

		XMMATHPLUS_API Real Sign(Real value);

		XMMATHPLUS_API int Sign(int value);

		XMMATHPLUS_API uint Sign(uint value);

		XMMATHPLUS_API inline Real Sin(Real value) { return ::sinf(value); }

		XMMATHPLUS_API inline Real Cos(Real value) { return ::cosf(value); }

		XMMATHPLUS_API inline Real Tan(Real value) { return ::tanf(value); }

		XMMATHPLUS_API inline Real Asin(Real value) { return ::asinf(Ex::Clamp(value, -1.0f, +1.0f)); }

		XMMATHPLUS_API inline Real Acos(Real value) { return ::acosf(Ex::Clamp(value, -1.0f, +1.0f)); }

		XMMATHPLUS_API inline Real Atan(Real value) { return ::atanf(value); }

		XMMATHPLUS_API inline Real Atan2(Real y, Real x) { return ::atan2f(y, x); }

		XMMATHPLUS_API inline Real Mod(Real x, Real y) { return ::fmod(x, y); }

		XMMATHPLUS_API inline int Mod(int x, int y) { return x % y; }

		XMMATHPLUS_API inline uint Mod(uint x, uint y) { return x % y; }

		XMMATHPLUS_API inline bool IsNaN(Real value) { return ::isnan(value); }

	};
}

