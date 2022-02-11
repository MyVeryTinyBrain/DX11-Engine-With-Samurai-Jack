#include "EnginePCH.h"
#include "MaterialSkinnedUnlit.h"

MaterialSkinnedUnlit::MaterialSkinnedUnlit()
{
}

MaterialSkinnedUnlit::~MaterialSkinnedUnlit()
{
}

tstring MaterialSkinnedUnlit::GetShaderPath() const
{
	//return TEXT("../Shader/SkinnedUnlit.fx");
	return TEXT("../Shader/Standard.fx");
}

void MaterialSkinnedUnlit::OnCreated()
{
}

void MaterialSkinnedUnlit::OnSetMaterialValues()
{
}
