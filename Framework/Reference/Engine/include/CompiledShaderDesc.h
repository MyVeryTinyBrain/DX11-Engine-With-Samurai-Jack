#pragma once

#include "ShaderTypes.h"

ENGINE_BEGIN
class PassDesc;
class TechniqueDesc;
class ShaderVariableInfo;
class ENGINE_API CompiledShaderDesc
{
protected:

	CompiledShaderDesc(ID3DX11Effect* effect, const vector<TechniqueDesc*>& techniqueDescs, const vector<ShaderVariableInfo*>& infos);

public:

	~CompiledShaderDesc();

public:

	Com<ID3DX11Effect> GetEffect() const;

	uint GetTechniqueDescCount() const;
	TechniqueDesc* GetTechniqueDesc(uint index) const;
	HRESULT GetTechniqueIndexByName(const string& name, uint& out_index) const;
	HRESULT GetPassIndexByName(const string& name, uint techniqueIndex, uint& out_index) const;

public:

	HRESULT SetRawValue(const string& name, const void* data, size_t size);
	HRESULT SetFloat(const string& name, float value);
	HRESULT SetBool(const string& name, bool value);
	HRESULT SetVector(const string& name, const V4& value);
	HRESULT SetColor(const string& name, const Color& value);
	HRESULT SetMatrix(const string& name, const M4& value);
	HRESULT SetTexture(const string& name, Com<ID3D11ShaderResourceView> texture);
	HRESULT SetTextures(const string& name, ID3D11ShaderResourceView** textures, uint count);

public:

	HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex);
	HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex);

public:

	inline const uint GetVariableCount() const { return (uint)m_variableInfos.size(); }
	const ShaderVariableInfo* FindVariableByName(const string& name);
	const ShaderVariableInfo* FindVariableByIndex(uint index);

public:

	static CompiledShaderDesc* LoadCompiledShaderFromFile(Com<ID3D11Device> device, const tstring& path, tstring& out_error);
	static CompiledShaderDesc* LoadCompiledShaderFromBinaryFolder(Com<ID3D11Device> device, const tstring& path, tstring& out_error);

	static tstring ParseToBinaryShaderPath(const tstring& path);

private:

	static HRESULT CompileShader(const tstring& path, ID3DBlob** out_memory, ID3DBlob** out_errorMsg);
	static CompiledShaderDesc* CreateCompiledShader(Com<ID3D11Device> device, ID3DBlob* memory, tstring& out_error);
	static ShaderVariableType TypeOf(const D3DX11_EFFECT_TYPE_DESC& typeDesc);

private:

	ID3DX11Effect*				m_effect = nullptr;

	vector<TechniqueDesc*>		m_techniqueDescs;

	vector<ShaderVariableInfo*>	m_variableInfos;
};

ENGINE_END
