#include "EnginePCH.h"
#include "MaterialGizmoRotationLine.h"

MaterialGizmoRotationLine::MaterialGizmoRotationLine()
{
}

MaterialGizmoRotationLine::~MaterialGizmoRotationLine()
{
}

tstring MaterialGizmoRotationLine::GetShaderPath() const
{
	return TEXT("../Shader/GizmoRotationLine.fx");
}

void MaterialGizmoRotationLine::OnCreated()
{
}

void MaterialGizmoRotationLine::OnSetMaterialValues()
{
}
