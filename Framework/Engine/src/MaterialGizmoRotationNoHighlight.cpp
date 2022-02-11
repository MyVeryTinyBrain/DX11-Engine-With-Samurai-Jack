#include "EnginePCH.h"
#include "MaterialGizmoRotationNoHighlight.h"

MaterialGizmoRotationNoHighlight::MaterialGizmoRotationNoHighlight()
{
}

MaterialGizmoRotationNoHighlight::~MaterialGizmoRotationNoHighlight()
{
}

tstring MaterialGizmoRotationNoHighlight::GetShaderPath() const
{
	return TEXT("../Shader/GizmoRotationNoHighlight.fx");
}

void MaterialGizmoRotationNoHighlight::OnCreated()
{
}

void MaterialGizmoRotationNoHighlight::OnSetMaterialValues()
{
}
