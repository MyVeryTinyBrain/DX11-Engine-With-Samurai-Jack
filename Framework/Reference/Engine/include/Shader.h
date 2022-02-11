#pragma once

#include "ResourceObject.h"
#include "CompiledShaderDesc.h"

ENGINE_BEGIN
class Texture;
class ENGINE_API Shader : public ResourceObject
{
protected:

	Shader(
		ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName, 
		CompiledShaderDesc* shaderDesc);

	virtual ~Shader();

public:

	HRESULT SetRawValue(const string& name, const void* data, size_t size);

	HRESULT SetFloat(const string& name, float value);

	HRESULT SetVector(const string& name, const V4& value);

	HRESULT SetColor(const string& name, const Color& value);

	HRESULT SetMatrix(const string& name, const M4& value);

	HRESULT SetTexture(const string& name, ResourceRef<Texture> texture);

	HRESULT SetInputLayout(size_t techniqueIndex, size_t passIndex);

	HRESULT ApplyPass(size_t techniqueIndex, size_t passIndex);

public:

	HRESULT GetTechniqueCount(size_t& out_techniqueCount) const;

	HRESULT GetPassCount(size_t techniqueIndex, size_t& out_passCount) const;

	CompiledShaderDesc* GetShaderDesc();

	_declspec(property(get = GetShaderDesc)) CompiledShaderDesc* shaderDesc;

public:

	static ResourceRef<Shader> CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path);

	static ResourceRef<Shader> CreateManagedShaderFromFile(ResourceManagement* management, const tstring& path, const tstring& groupName);

private:

	CompiledShaderDesc* m_shaderDesc = nullptr;
};

ENGINE_END
