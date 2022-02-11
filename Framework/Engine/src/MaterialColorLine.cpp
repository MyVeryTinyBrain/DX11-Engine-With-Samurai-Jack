#include "EnginePCH.h"
#include "MaterialColorLine.h"

tstring MaterialColorLine::GetShaderPath() const
{
    return TEXT("../Shader/ColorLine.fx");
}

void MaterialColorLine::OnCreated()
{
}

void MaterialColorLine::OnSetMaterialValues()
{
    SetColor("color", color);
}
