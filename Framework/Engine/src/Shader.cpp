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

HRESULT Shader::SetTextures(const string& name, ResourceRef<Texture>* textures, uint count)
{
	if (!m_shaderDesc || !textures)
		return E_FAIL;

	ID3D11ShaderResourceView* shaderResourceViews[32] = {};
	for (uint i = 0; i < count; ++i)
		shaderResourceViews[i] = textures[i]->shaderResourceView.Get();

	return m_shaderDesc->SetTextures(name, shaderResourceViews, count);
}

HRESULT Shader::SetInputLayout(uint techniqueIndex, uint passIndex)
{
	if (!m_shaderDesc)
		return E_FAIL;

	if (!system || !system->graphic || !system->graphic->deviceContext)
		return E_FAIL;

	return m_shaderDesc->SetInputLayout(system->graphic->deviceContext, techniqueIndex, passIndex);
}

HRESULT Shader::ApplyPass(uint techniqueIndex, uint passIndex)
{
	if (!m_shaderDesc)
		return E_FAIL;

	if (!system || !system->graphic)
		return E_FAIL;

	return m_shaderDesc->ApplyPass(system->graphic->deviceContext, techniqueIndex, passIndex);
}

HRESULT Shader::GetTechniqueCount(uint& out_techniqueCount) const
{
	if (!m_shaderDesc)
	{
		out_techniqueCount = 0;
		return E_FAIL;
	}

	out_techniqueCount = m_shaderDesc->GetTechniqueDescCount();

	return S_OK;
}

HRESULT Shader::GetPassCount(uint techniqueIndex, uint& out_passCount) const
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

const uint Shader::GetVariableCount() const
{
	return m_shaderDesc->GetVariableCount();
}

const ShaderVariableInfo* Shader::FindVariableByName(const string& name)
{
	return m_shaderDesc->FindVariableByName(name);
}

const ShaderVariableInfo* Shader::FindVariableByIndex(uint index)
{
	return m_shaderDesc->FindVariableByIndex(index);
}

ResourceRef<Shader> Shader::CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(path);
	if (find)
		return find;

	tstring error;
	CompiledShaderDesc* compiledShaderDesc =
		CompiledShaderDesc::CreateCompiledShaderFromFile(management->GetSystem()->graphic->device, path, error);

	if (!compiledShaderDesc)
	{
		tstring error_message = TEXT("Failed to compile Shader: ") + path;
		if (error.length() > 0)
			error_message += TEXT("(") + error + TEXT(")");
		ERROR_MESSAGE_NT(error_message.c_str());
		return nullptr;
	}

	return new Shader(management, true, path, TEXT(""), compiledShaderDesc);
}

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

ResourceRef<Shader> Shader::CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path, const tstring& groupName)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(path);
	if (find)
		return find;

	tstring error;
	CompiledShaderDesc* compiledShaderDesc =
		CompiledShaderDesc::CreateCompiledShaderFromFile(management->GetSystem()->graphic->device, path, error);

	if (!compiledShaderDesc)
	{
		tstring error_message = TEXT("Failed to compile Shader: ") + path;
		if (error.length() > 0)
			error_message += TEXT("(") + error + TEXT(")");
		ERROR_MESSAGE_NT(error_message.c_str());
		return nullptr;
	}

	return new Shader(management, true, path, groupName, compiledShaderDesc);
}

ResourceRef<Shader> Shader::CreateManagedShaderFromBinaryFolder(ResourceManagement* management, const tstring& path)
{
	if (!management)
		return nullptr;

	tstring binaryShaderPath = CompiledShaderDesc::ParseToBinaryShaderPath(path);

	ResourceRef<ResourceObject> find = management->Find(binaryShaderPath);
	if (find)
		return find;

	tstring error;
	CompiledShaderDesc* compiledShaderDesc =
		CompiledShaderDesc::CreateCompiledShaderFromBinaryFolder(management->GetSystem()->graphic->device, path, error);

	if (!compiledShaderDesc)
	{
		tstring error_message = TEXT("Failed to compile Shader: ") + binaryShaderPath;
		if (error.length() > 0)
			error_message += TEXT("(") + error + TEXT(")");
		ERROR_MESSAGE_NT(error_message.c_str());
		return nullptr;
	}

	return new Shader(management, true, binaryShaderPath, TEXT(""), compiledShaderDesc);
}

ResourceRef<Shader> Shader::CreateManagedShaderFromBinaryFolder(ResourceManagement* management, const tstring& path, const tstring& groupName)
{
	if (!management)
		return nullptr;

	tstring binaryShaderPath = CompiledShaderDesc::ParseToBinaryShaderPath(path);

	ResourceRef<ResourceObject> find = management->Find(binaryShaderPath);
	if (find)
		return find;

	tstring error;
	CompiledShaderDesc* compiledShaderDesc =
		CompiledShaderDesc::CreateCompiledShaderFromBinaryFolder(management->GetSystem()->graphic->device, path, error);

	if (!compiledShaderDesc)
	{
		tstring error_message = TEXT("Failed to compile Shader: ") + binaryShaderPath;
		if (error.length() > 0)
			error_message += TEXT("(") + error + TEXT(")");
		ERROR_MESSAGE_NT(error_message.c_str());
		return nullptr;
	}

	return new Shader(management, true, binaryShaderPath, groupName, compiledShaderDesc);
}
