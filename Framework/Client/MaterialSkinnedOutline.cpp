#include "stdafx.h"
#include "MaterialSkinnedOutline.h"

MaterialSkinnedOutline::MaterialSkinnedOutline()
{
}

tstring MaterialSkinnedOutline::GetShaderPath() const
{
    return TEXT("../Shader/SkinnedOutline.fx");
}

void MaterialSkinnedOutline::OnCreated()
{
}

void MaterialSkinnedOutline::OnSetMaterialValues()
{
}
