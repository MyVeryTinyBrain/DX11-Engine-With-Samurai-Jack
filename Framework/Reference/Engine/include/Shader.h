#pragma once

#include "ResourceObject.h"
#include "CompiledShaderDesc.h"

ENGINE_BEGIN
class Texture;
class ENGINE_API Shader : public ResourceObject
{
protected:

	Shader(
		ResourceManagement* management, bool managed, const tstring& path, 
		CompiledShaderDesc* shaderDesc);

	virtual ~Shader();

public:

	HRESULT SetRawValue(const string& name, const void* data, size_t size);
	HRESULT SetFloat(const string& name, float value);
	HRESULT SetVector(const string& name, const V4& value);
	HRESULT SetColor(const string& name, const Color& value);
	HRESULT SetMatrix(const string& name, const M4& value);
	HRESULT SetTexture(const string& name, ResourceRef<Texture> texture);
	HRESULT SetTextures(const string& name, ResourceRef<Texture>* textures, uint count);

	HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex);
	HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex);

public:

	HRESULT GetTechniqueCount(uint& out_techniqueCount) const;
	HRESULT GetPassCount(uint techniqueIndex, uint& out_passCount) const;
	CompiledShaderDesc* GetShaderDesc();

	_declspec(property(get = GetShaderDesc)) CompiledShaderDesc* shaderDesc;

public:

	const uint GetVariableCount() const;
	const ShaderVariableInfo* FindVariableByName(const string& name);
	const ShaderVariableInfo* FindVariableByIndex(uint index);

	_declspec(property(get = GetVariableCount)) uint variableCount;

public:

	static ResourceRef<Shader> LoadShaderFromFileM(ResourceManagement* management, const tstring& path);
	static ResourceRef<Shader> LoadShaderFromBinaryFolderM(ResourceManagement* management, const tstring& path);

private:

	CompiledShaderDesc* m_shaderDesc = nullptr;
};

ENGINE_END
