#include "EnginePCH.h"
#include "TransformData.h"

void TransformData::UpdateRotation()
{
	rotation = Q::Euler(eulerAngles);
}

void TransformData::UpdateEulerAngles()
{
	eulerAngles = rotation.eulerAngles;
}
