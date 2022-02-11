#include "EnginePCH.h"
#include "MaterialGizmoRotationHighlight.h"

MaterialGizmoRotationHighlight::MaterialGizmoRotationHighlight()
{
}

MaterialGizmoRotationHighlight::~MaterialGizmoRotationHighlight()
{
}

tstring MaterialGizmoRotationHighlight::GetShaderPath() const
{
	return TEXT("../Shader/GizmoRotationHighlight.fx");
}

void MaterialGizmoRotationHighlight::OnCreated()
{
}

void MaterialGizmoRotationHighlight::OnSetMaterialValues()
{
}
