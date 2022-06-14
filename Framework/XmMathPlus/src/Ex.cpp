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

int XmMathPlus::Ex::Clamp(int value, int min, int max)
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

uint XmMathPlus::Ex::Clamp(uint value, uint min, uint max)
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

Real Ex::Closet(Real value, Real min, Real max)
{
	Real toMin = abs(value - min);
	Real toMax = abs(value - max);

	if (toMin < toMax) return min;
	else return max;
}

int XmMathPlus::Ex::Closet(int value, int min, int max)
{
	int toMin = abs(value - min);
	int toMax = abs(value - max);

	if (toMin < toMax) return min;
	else return max;
}

uint XmMathPlus::Ex::Closet(uint value, uint min, uint max)
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
	float delta = Repeat((to - from), 360.0f);
	if (delta > 180.0f)
		delta -= 360.0f;
	return delta;
}

Real Ex::LerpAngle(Real from, Real to, Real t)
{
	float delta = Repeat((to - from), 360.0f);
	if (delta > 180.0f)
		delta -= 360.0f;
	return from + delta * Clamp01(t);
}

Real Ex::Repeat(Real value, Real max)
{
	return Clamp(value - floor(value / max) * max, 0.0f, max);
}

Real XmMathPlus::Ex::Sign(Real value)
{
	if (value < 0) return -1;
	else if (value > 0) return +1;
	else return 0;
}

int XmMathPlus::Ex::Sign(int value)
{
	if (value < 0) return -1;
	else if (value > 0) return +1;
	else return 0;
}

uint XmMathPlus::Ex::Sign(uint value)
{
	if (value > 0) return +1;
	else return 0;
}
