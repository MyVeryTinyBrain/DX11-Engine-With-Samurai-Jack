#include "EnginePCH.h"
#include "MaterialGizmoTranslation.h"

MaterialGizmoTranslation::MaterialGizmoTranslation()
{
}

MaterialGizmoTranslation::~MaterialGizmoTranslation()
{
}

tstring MaterialGizmoTranslation::GetShaderPath() const
{
	return TEXT("../Shader/GizmoTranslation.fx");
}

void MaterialGizmoTranslation::OnCreated()
{
}

void MaterialGizmoTranslation::OnSetMaterialValues()
{
}
