#include "XmMathPlusPCH.h"
#include "Frustum.h"

void XmMathPlus::Frustum::GetCorners(V3* out_arrCorners) const
{
	XMFLOAT3 xmCorners[8] = {};
	m_frustum.GetCorners(xmCorners);

	out_arrCorners[0] = xmCorners[0];
	out_arrCorners[1] = xmCorners[1];
	out_arrCorners[2] = xmCorners[2];
	out_arrCorners[3] = xmCorners[3];
	out_arrCorners[4] = xmCorners[4];
	out_arrCorners[5] = xmCorners[5];
	out_arrCorners[6] = xmCorners[6];
	out_arrCorners[7] = xmCorners[7];
}
