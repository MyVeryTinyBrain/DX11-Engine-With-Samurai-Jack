#include "EnginePCH.h"
#include "MaterialStandard.h"
#include "System.h"
#include "BuiltInResources.h"
#include "Texture.h"
#include "Texture2D.h"

MaterialStandard::MaterialStandard()
{
}

MaterialStandard::~MaterialStandard()
{
}

tstring MaterialStandard::GetShaderPath() const
{
	return TEXT("../Shader/Standard.fx");
}

void MaterialStandard::OnCreated()
{
	m_normalMapTexture = system->resourceManagement->builtInResources->blueTexture;
	m_lightMapTexture = system->resourceManagement->builtInResources->whiteTexture;
	m_occlusionTexture = system->resourceManagement->builtInResources->whiteTexture;
	m_shadowMapTexture = system->resourceManagement->builtInResources->whiteTexture;
	m_specularMapTexture = system->resourceManagement->builtInResources->whiteTexture;
	m_emissiveTexture = system->resourceManagement->builtInResources->clearTexture;
	m_reflectionTexture = system->resourceManagement->builtInResources->blackTexture;
}

void MaterialStandard::OnSetMaterialValues()
{
	SetTexture("_NormalMapTexture", m_normalMapTexture);
	SetTexture("_LightMapTexture", m_lightMapTexture);
	SetTexture("_OcclusionTexture", m_occlusionTexture);
	SetTexture("_ShadowMapTexture", m_occlusionTexture);
	SetTexture("_SpecularMapTexture", m_occlusionTexture);
	SetFloat("_SpecularPower", m_specularPower);
	SetFloat("_SpecularTransparency", m_specularTransparency);
	SetTexture("_EmissiveTexture", m_emissiveTexture);
	SetTexture("_ReflectionTexture", m_reflectionTexture);
	SetFloat("_ReflectionTransparency", m_reflectionTransparency);
	SetFloat("_ReflectionBlur", m_reflectionBlur);
	SetFloat("_ReflectMask", m_reflectMask);
}

const ResourceRef<Texture>& MaterialStandard::GetNormalMapTexture()
{
	return m_normalMapTexture;
}

const ResourceRef<Texture>& MaterialStandard::GetLightMapTexture()
{
	return m_lightMapTexture;
}

const ResourceRef<Texture>& MaterialStandard::GetOcclusionTexture()
{
	return m_occlusionTexture;
}

const ResourceRef<Texture>& MaterialStandard::GetShadowMapTexture()
{
	return m_shadowMapTexture;
}

const ResourceRef<Texture>& MaterialStandard::GetSpecularMapTexture()
{
	return m_specularMapTexture;
}

float MaterialStandard::GetSpecularPower()
{
	return m_specularPower;
}

float MaterialStandard::GetSpecularTransparency()
{
	return m_specularTransparency;
}

const ResourceRef<Texture>& MaterialStandard::GetEmissiveTexture()
{
	return m_emissiveTexture;
}

const ResourceRef<Texture>& MaterialStandard::GetReflectionTexture()
{
	return m_reflectionTexture;
}

float MaterialStandard::GetReflectionTransparency()
{
	return m_reflectionTransparency;
}

float MaterialStandard::GetReflectionBlur()
{
	return m_reflectionBlur;
}

float MaterialStandard::GetReflectMask()
{
	return m_reflectMask;
}

void MaterialStandard::SetNormalMapTexture(const ResourceRef<Texture>& texture)
{
	m_normalMapTexture = texture;

	if (!m_normalMapTexture)
	{
		m_normalMapTexture = system->resourceManagement->builtInResources->blueTexture;
	}
}

void MaterialStandard::SetLightMapTexture(const ResourceRef<Texture>& texture)
{
	m_lightMapTexture = texture;
}

void MaterialStandard::SetOcclusionTexture(const ResourceRef<Texture>& texture)
{
	m_occlusionTexture = texture;
}

void MaterialStandard::SetShadowMapTexture(const ResourceRef<Texture>& texture)
{
	m_shadowMapTexture = texture;
}

void MaterialStandard::SetSpecularMapTexture(const ResourceRef<Texture>& texture)
{
	m_specularMapTexture = texture;
}

void MaterialStandard::SetSpecularTransparency(float value)
{
	m_specularTransparency = value;
}

void MaterialStandard::SetSpecularPower(float value)
{
	m_specularPower = value;
}

void MaterialStandard::SetEmissiveTexture(const ResourceRef<Texture>& texture)
{
	m_emissiveTexture = texture;
}

void MaterialStandard::SetReflectionTexture(const ResourceRef<Texture>& texture)
{
	m_reflectionTexture = texture;
}

void MaterialStandard::SetReflectionTransparency(float value)
{
	m_reflectionTransparency = value;
}

void MaterialStandard::SetReflectionBlur(float value)
{
	m_reflectionBlur = value;
}

void MaterialStandard::SetReflectMask(float value)
{
	m_reflectMask = value;
}
