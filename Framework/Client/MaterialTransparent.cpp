#include "stdafx.h"
#include "MaterialTransparent.h"

tstring MaterialTransparent::GetShaderPath() const
{
    return TEXT("../Shader/Transparent.fx");
}

void MaterialTransparent::OnCreated()
{
    m_normalMapTexture = system->resourceManagement->builtInResources->blueTexture;
}

void MaterialTransparent::OnSetMaterialValues()
{
	SetTexture("_NormalMapTexture", m_normalMapTexture);
}

const ResourceRef<Texture>& MaterialTransparent::GetNormalMapTexture()
{
    return m_normalMapTexture;
}

void MaterialTransparent::SetNormalMapTexture(const ResourceRef<Texture>& texture)
{
	m_normalMapTexture = texture;

	if (!m_normalMapTexture)
	{
		m_normalMapTexture = system->resourceManagement->builtInResources->blueTexture;
	}
}
