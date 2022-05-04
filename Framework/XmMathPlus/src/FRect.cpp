#include "XmMathPlusPCH.h"
#include "FRect.h"

void XmMathPlus::FRect::GetCorners(V2* out_arrCorners) const
{
	out_arrCorners[0] = V2(Left, Top);
	out_arrCorners[1] = V2(Right, Top);
	out_arrCorners[2] = V2(Right, Bottom);
	out_arrCorners[3] = V2(Left, Bottom);
}
