#pragma once

#include "ResourceObject.h"
#include "CompiledShaderDesc.h"

ENGINE_BEGIN

class ENGINE_API ComputeShader : public ResourceObject
{
protected:

	ComputeShader(
		ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName,
		CompiledShaderDesc* shaderDesc);

	virtual ~ComputeShader();

public:

	static ResourceRef<ComputeShader> CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path);

	static ResourceRef<ComputeShader> CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path, const tstring& groupName);

private:

	CompiledShaderDesc* m_shaderDesc = nullptr;
};

ENGINE_END
