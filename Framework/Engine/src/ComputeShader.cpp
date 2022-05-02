#include "EnginePCH.h"
#include "ComputeShader.h"
#include "CompiledShaderDesc.h"
#include "ResourceManagement.h"
#include "System.h"
#include "GraphicSystem.h"

ComputeShader::ComputeShader(
	ResourceManagement* management, bool managed, const tstring& path,
	CompiledShaderDesc* shaderDesc) :
	ResourceObject(management, managed, path),
	m_shaderDesc(shaderDesc)
{
}

ComputeShader::~ComputeShader()
{
	SafeDelete(m_shaderDesc);
}

ResourceRef<ComputeShader> ComputeShader::CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path)
{
	if (!management)
		return ResourceRef<ComputeShader>();

	ResourceRef<ComputeShader> find = management->Find(path);
	if (find)
		return find;

	tstring error;
	CompiledShaderDesc* compiledShaderDesc =
		CompiledShaderDesc::LoadCompiledShaderFromFile(management->GetSystem()->graphic->device, path, error);

	if (!compiledShaderDesc)
	{
		tstring error_message = TEXT("Failed to compile Compute Shader: ") + path;
		if (error.length() > 0)
			error_message += TEXT("(") + error + TEXT(")");
		ERROR_MESSAGE_NT(error_message.c_str());
		return ResourceRef<ComputeShader>();
	}

	ComputeShader* shader = new ComputeShader(management, true, path, compiledShaderDesc);

	return ResourceRef<ComputeShader>(shader);
}

