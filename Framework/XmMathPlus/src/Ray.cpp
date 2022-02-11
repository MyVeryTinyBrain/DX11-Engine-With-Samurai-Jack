#include "XmMathPlusPCH.h"
#include "Ray.h"
#include "SphericalYaw.h"

using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

void XmMathPlus::Ray::Transform(const V3& scale, const Q& rotation, const V3& position)
{
	M4 transformation = M4::SRT(scale, rotation, position);

	origin = transformation.MultiplyPoint(origin);
	direction = transformation.MultiplyVector(direction);
	direction = direction.normalized;
}

void XmMathPlus::Ray::Transform(const M4& transformation)
{
	origin = transformation.MultiplyPoint(origin);
	direction = transformation.MultiplyVector(direction);
	direction = direction.normalized;
}
