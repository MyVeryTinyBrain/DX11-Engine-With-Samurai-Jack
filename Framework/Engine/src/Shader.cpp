#include "EnginePCH.h"
#include "Shader.h"
#include "CompiledShaderDesc.h"
#include "ResourceManagement.h"
#include "System.h"
#include "GraphicSystem.h"
#include "Texture.h"
#include "TechniqueDesc.h"

Shader::Shader(
	ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName, 
	CompiledShaderDesc* shaderDesc) :
	ResourceObject(management, managed, path, groupName),
	m_shaderDesc(shaderDesc)
{
}

Shader::~Shader()
{
	SafeDelete(m_shaderDesc);
}

HRESULT Shader::SetRawValue(const string& name, const void* data, size_t size)
{
	if (!m_shaderDesc || !data)
		return E_FAIL;

	return m_shaderDesc->SetRawValue(name, data, size);
}

HRESULT Shader::SetFloat(const string& name, float value)
{
	if (!m_shaderDesc)
		return E_FAIL;

	return m_shaderDesc->SetFloat(name, value);
}

HRESULT Shader::SetVector(const string& name, const V4& value)
{
	if (!m_shaderDesc)
		return E_FAIL;

	return m_shaderDesc->SetVector(name, value);
}

HRESULT Shader::SetColor(const string& name, const Color& value)
{
	if (!m_shaderDesc)
		return E_FAIL;

	return m_shaderDesc->SetColor(name, value);
}

HRESULT Shader::SetMatrix(const string& name, const M4& value)
{
	if (!m_shaderDesc)
		return E_FAIL;

	return m_shaderDesc->SetMatrix(name, value);
}

HRESULT Shader::SetTexture(const string& name, ResourceRef<Texture> texture)
{
	if (!m_shaderDesc || !texture)
		return E_FAIL;

	return m_shaderDesc->SetTexture(name, texture->shaderResourceView);
}

HRESULT Shader::SetInputLayout(size_t techniqueIndex, size_t passIndex)
{
	if (!m_shaderDesc)
		return E_FAIL;

	if (!system || !system->graphicSystem || !system->graphicSystem->deviceContext)
		return E_FAIL;

	return m_shaderDesc->SetInputLayout(system->graphicSystem->deviceContext, techniqueIndex, passIndex);
}

HRESULT Shader::ApplyPass(size_t techniqueIndex, size_t passIndex)
{
	if (!m_shaderDesc)
		return E_FAIL;

	if (!system || !system->graphicSystem)
		return E_FAIL;

	return m_shaderDesc->ApplyPass(system->graphicSystem->deviceContext, techniqueIndex, passIndex);
}

HRESULT Shader::GetTechniqueCount(size_t& out_techniqueCount) const
{
	if (!m_shaderDesc)
	{
		out_techniqueCount = 0;
		return E_FAIL;
	}

	out_techniqueCount = m_shaderDesc->GetTechniqueDescCount();

	return S_OK;
}

HRESULT Shader::GetPassCount(size_t techniqueIndex, size_t& out_passCount) const
{
	if (!m_shaderDesc)
	{
		out_passCount = 0;
		return E_FAIL;
	}

	TechniqueDesc* techniqueDesc = m_shaderDesc->GetTechniqueDesc(techniqueIndex);

	if (!techniqueDesc)
	{
		out_passCount = 0;
		return E_FAIL;
	}

	out_passCount = techniqueDesc->GetPassDescCount();

	return S_OK;
}

CompiledShaderDesc* Shader::GetShaderDesc()
{
	return m_shaderDesc;
}

ResourceRef<Shader> Shader::CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path)
{
	if(!management)
		return ResourceRef<Shader>();

	ResourceRef<Shader> find = management->Find(path);
	if (find)
		return find;

	tstring error;
	CompiledShaderDesc* compiledShaderDesc =
		CompiledShaderDesc::CreateCompiledShaderFromFile(management->GetSystem()->graphicSystem->device, path, error);

	if (!compiledShaderDesc)
	{
		tstring error_message = TEXT("Failed to compile Shader: ") + path;
		if (error.length() > 0)
			error_message += TEXT("(") + error + TEXT(")");
		ERROR_MESSAGE_NT(error_message.c_str());
		return ResourceRef<Shader>();
	}

	Shader* shader = new Shader(management, true, path, TEXT(""), compiledShaderDesc);

	return ResourceRef<Shader>(shader);
}

ResourceRef<Shader> Shader::CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path, const tstring& groupName)
{
	if (!management)
		return ResourceRef<Shader>();

	ResourceRef<Shader> find = management->Find(path);
	if (find)
		return find;

	tstring error;
	CompiledShaderDesc* compiledShaderDesc =
		CompiledShaderDesc::CreateCompiledShaderFromFile(management->GetSystem()->graphicSystem->device, path, error);

	if (!compiledShaderDesc)
	{
		tstring error_message = TEXT("Failed to compile Shader: ") + path;
		if (error.length() > 0)
			error_message += TEXT("(") + error + TEXT(")");
		ERROR_MESSAGE_NT(error_message.c_str());
		return ResourceRef<Shader>();
	}

	Shader* shader = new Shader(management, true, path, groupName, compiledShaderDesc);

	return ResourceRef<Shader>(shader);
}
