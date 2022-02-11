#include "EnginePCH.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "TechniqueDesc.h"
#include "PassDesc.h"
#include "BuiltInResources.h"
#include "System.h"
#include "Times.h"
#include "Texture2D.h"
#include "ResourceFactory.h"

Material::Material() :
	ResourceObject()
{
}

Material::~Material()
{
}

HRESULT Material::SetRawValue(const string& name, const void* data, size_t size)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetRawValue(name, data, size);
}

HRESULT Material::SetFloat(const string& name, float value)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetFloat(name, value);
}

HRESULT Material::SetVector(const string& name, const V4& value)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetVector(name, value);
}

HRESULT Material::SetColor(const string& name, const Color& value)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetColor(name, value);
}

HRESULT Material::SetMatrix(const string& name, const M4& value)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetMatrix(name, value);
}

HRESULT Material::SetTexture(const string& name, ResourceRef<Texture> texture)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetTexture(name, texture);
}

HRESULT Material::GetTechniqueCount(size_t& out_techniqueCount) const
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->GetTechniqueCount(out_techniqueCount);
}

HRESULT Material::GetPassCount(size_t techniqueIndex, size_t& out_passCount) const
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->GetPassCount(techniqueIndex, out_passCount);
}

size_t Material::GetPassCountOfAppliedTechnique() const
{
	if (!m_shader)
		return 0;

	size_t passCount = 0;
	if (FAILED(m_shader->GetPassCount(m_techniqueIndex, passCount)))
		return 0;

	return passCount;
}

HRESULT Material::GetRenderGroupOfAppliedTechnique(uint passIndex, RenderGroup& out_renderGroup) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_renderGroup = passDesc->GetRenderGroup();

	return S_OK;
}

HRESULT Material::GetRenderGroupOrderOfAppliedTechnique(uint passIndex, int& out_renderGroupOrder) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_renderGroupOrder = passDesc->GetRenderGroupOrder();

	return S_OK;
}

HRESULT Material::GetInstancingFlagOfAppliedTechnique(uint passIndex, bool& out_instancingFlag) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_instancingFlag = passDesc->IsInstancing();

	return S_OK;
}

HRESULT Material::GetDrawShadowFlagOfAppliedTechnique(uint passIndex, bool& out_drawShadowFlag) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_drawShadowFlag = passDesc->IsDrawShadow();

	return S_OK;
}

HRESULT Material::GetShadowCutoffEnableFlagOfAppliedTechnique(uint passIndex, bool& out_shadowCutoffEnableFlag) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_shadowCutoffEnableFlag = passDesc->IsShadowCutoffEnable();

	return S_OK;
}

HRESULT Material::GetShadowCutoffAlphaOfAppliedTechnique(uint passIndex, float& out_shadowCutoffAlpha) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_shadowCutoffAlpha = passDesc->GetShadowCutoffAlphaValue();

	return S_OK;
}

bool Material::IsValid() const
{
	if (!m_shader)
		return false;

	size_t techniqueCount = 0;
	
	if (FAILED(GetTechniqueCount(techniqueCount)))
		return false;

	if (m_techniqueIndex >= techniqueCount)
		return false;

	size_t passCount = 0;

	if (FAILED(GetPassCount(m_techniqueIndex, passCount)))
		return false;

	if (passCount == 0)
		return false;

	return true;
}

ResourceRef<Shader> Material::GetShader() const
{
	return m_shader;
}

ResourceRef<Texture> Material::GetDiffuseTexture() const
{
	return m_diffuseTexture;
}

void Material::SetDiffuseTexture(const ResourceRef<Texture>& texture)
{
	m_diffuseTexture = texture;

	if (!m_diffuseTexture)
		m_diffuseTexture = system->resourceManagement->builtInResources->whiteTexture;
}

void Material::SetMaterialValues()
{
	SetTexture("_DiffuseTexture", m_diffuseTexture);
	SetFloat("_Time", system->time->accumulatedSinceStartup);
	SetFloat("_UnscaledTime", system->time->unscaledAccumulatedSinceStartup);
	OnSetMaterialValues();
}

HRESULT Material::GetEffectDesc(Com<ID3DX11Effect>& out_effect) const
{
	if (!m_shader)
		return E_FAIL;

	auto shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	out_effect = shaderDesc->GetEffect();

	return S_OK;
}

HRESULT Material::SetInputLayout(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetInputLayout(techniqueIndex, passIndex);
}

HRESULT Material::ApplyPass(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->ApplyPass(techniqueIndex, passIndex);
}

bool Material::Initialize(ResourceManagement* management, bool managed, const tstring& resourceKey, const tstring& groupName)
{
	if (!management)
		return false;

	if (managed)
	{
		SetToManaged(management, resourceKey, groupName);
	}
	else
	{
		SetManagement(management);
	}

	return true;
}

bool Material::SetupShader()
{
	//ResourceRef<Shader> shader = management->Find(GetShaderPath());
	ResourceRef<Shader> shader = management->factory->CreateManagedShaderFromFile(GetShaderPath());
	if (!shader)
		return false;

	m_shader = shader;
	return true;
}

bool Material::SetupDefaultDiffuseTexture()
{
	m_diffuseTexture = management->builtInResources->whiteTexture;
	return nullptr != m_diffuseTexture;
}
