#include "XmMathPlusPCH.h"
#include "Ex.h"
#include "V3.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

Real Ex::Lerp(Real from, Real to, Real t)
{
	if (t < 0.0f)
	{
		t = 0.0f;
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
	}

	return LerpUnclmaped(from, to, t);
}

Real Ex::LerpUnclmaped(Real from, Real to, Real t)
{
	return from + (to - from) * t;
}

Real Ex::Clamp(Real value, Real min, Real max)
{
	if (value < min)
	{
		value = min;
	}
	else if (value > max)
	{
		value = max;
	}

	return value;
}

XMMATHPLUS_API int XmMathPlus::Ex::Clamp(int value, int min, int max)
{
	if (value < min)
	{
		value = min;
	}
	else if (value > max)
	{
		value = max;
	}

	return value;
}

XMMATHPLUS_API uint XmMathPlus::Ex::Clamp(uint value, uint min, uint max)
{
	if (value < min)
	{
		value = min;
	}
	else if (value > max)
	{
		value = max;
	}

	return value;
}

Real Ex::Clamp01(Real value, Real min, Real max)
{
	value = Clamp(value, min, max);

	Real delta = max - min;
	Real valueFromMin = value - min;
	Real percent = valueFromMin / delta;

	return percent;
}

XMMATHPLUS_API int XmMathPlus::Ex::Clamp01(int value, int min, int max)
{
	value = Clamp(value, min, max);

	int delta = max - min;
	int valueFromMin = value - min;
	int percent = valueFromMin / delta;

	return percent;
}

XMMATHPLUS_API uint XmMathPlus::Ex::Clamp01(uint value, uint min, uint max)
{
	value = Clamp(value, min, max);

	uint delta = max - min;
	uint valueFromMin = value - min;
	uint percent = valueFromMin / delta;

	return percent;
}

Real Ex::Closet(Real value, Real min, Real max)
{
	Real toMin = abs(value - min);
	Real toMax = abs(value - max);

	if (toMin < toMax) return min;
	else return max;
}

XMMATHPLUS_API int XmMathPlus::Ex::Closet(int value, int min, int max)
{
	int toMin = abs(value - min);
	int toMax = abs(value - max);

	if (toMin < toMax) return min;
	else return max;
}

XMMATHPLUS_API uint XmMathPlus::Ex::Closet(uint value, uint min, uint max)
{
	if (value < min)
		return min;

	if (value > max)
		return max;

	uint toMin = value - min;
	uint toMax = value - max;

	if (toMin < toMax) return min;
	else return max;
}

Real Ex::DeltaAngle(Real from, Real to)
{
	Real delta = to - from;

	while (delta > 180.0f)
		delta -= 360.0f;

	while (delta < -180.0f)
		delta += 360.0f;

	return delta;
}

Real Ex::Repeat(Real value, Real max)
{
	return fmod(value, max);
}

XMMATHPLUS_API int XmMathPlus::Ex::Repeat(int value, int max)
{
	return value % max;
}

XMMATHPLUS_API uint XmMathPlus::Ex::Repeat(uint value, uint max)
{
	return value % max;
}

Real XmMathPlus::Ex::Sign(Real value)
{
	if (value < 0) return -1;
	else if (value > 0) return +1;
	else return 0;
}

XMMATHPLUS_API int XmMathPlus::Ex::Sign(int value)
{
	if (value < 0) return -1;
	else if (value > 0) return +1;
	else return 0;
}

XMMATHPLUS_API uint XmMathPlus::Ex::Sign(uint value)
{
	if (value > 0) return +1;
	else return 0;
}
