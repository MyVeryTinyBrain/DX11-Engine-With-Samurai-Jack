#pragma once

ENGINE_BEGIN
class PassDesc;
class TechniqueDesc;
class ENGINE_API CompiledShaderDesc
{
protected:

	CompiledShaderDesc(ID3DX11Effect* effect, const vector<TechniqueDesc*>& techniqueDescs);

public:

	~CompiledShaderDesc();

public:

	Com<ID3DX11Effect> GetEffect() const;

	size_t GetTechniqueDescCount() const;

	TechniqueDesc* GetTechniqueDesc(size_t index) const;

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

	HRESULT SetTextureArray(const string& name, ID3D11ShaderResourceView** textureArray, uint count);

	HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex);

	HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex);

public:

	static CompiledShaderDesc* CreateCompiledShaderFromFile(Com<ID3D11Device> device, const tstring& path);

private:

	ID3DX11Effect* m_effect = nullptr;

	vector<TechniqueDesc*> m_techniqueDescs;
};

ENGINE_END
