#pragma once

#include "ResourceObject.h"
#include "IMaterial.h"
#include "ShaderType.h"

ENGINE_BEGIN
class Shader;
class Texture;
class ShaderVariable;
class ISpecificShaderVariable;
class ENGINE_API Material : public ResourceObject, public IMaterial
{
public:

	Material(ResourceManagement* management, bool managed, const tstring& path, ResourceRef<Shader> shader);
	Material(ResourceManagement* management, bool managed, const tstring& path, const Material& other);

	virtual ~Material();

	virtual void ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, ICamera* camera) override;

public:

	HRESULT SetRawValue(const string& name, const void* data, size_t size);
	HRESULT SetFloat(const string& name, float value);
	HRESULT SetVector(const string& name, const V4& value);
	HRESULT SetColor(const string& name, const Color& value);
	HRESULT SetMatrix(const string& name, const M4& value);
	HRESULT SetTexture(const string& name, ResourceRef<Texture> texture);
	HRESULT SetTextures(const string& name, ResourceRef<Texture>* textures, uint count);

	HRESULT GetRawValue(const string& name, void* out_data, size_t* inout_size);
	HRESULT GetFloat(const string& name, float* out_value);
	HRESULT GetVector(const string& name, V4* out_value);
	HRESULT GetColor(const string& name, Color* out_value);
	HRESULT GetMatrix(const string& name, M4* out_value);
	HRESULT GetTexture(const string& name, ResourceRef<Texture>* out_texture);
	HRESULT GetTextures(const string& name, ResourceRef<Texture>** out_textures, uint* out_count);

public:

	virtual HRESULT GetTechniqueCount(uint& out_techniqueCount) const final override;
	virtual HRESULT GetPassCount(uint techniqueIndex, uint& out_passCount) const final override;

	inline void SetTechniqueIndex(uint index) { m_techniqueIndex = index; }
	inline uint GetTechniqueIndex() const { return m_techniqueIndex; }

	uint GetPassCountOfAppliedTechnique() const;
	HRESULT GetRenderGroupOfAppliedTechnique(uint passIndex, RenderGroup& out_renderGroup) const;
	HRESULT GetRenderGroupOrderOfAppliedTechnique(uint passIndex, int& out_renderGroupOrder) const;
	HRESULT GetCullingFlagOfAppliedTechnique(uint passIndex, bool& out_cullingFlag) const;
	HRESULT GetInstancingFlagOfAppliedTechnique(uint passIndex, bool& out_instancingFlag) const;
	HRESULT GetDrawShadowFlagOfAppliedTechnique(uint passIndex, bool& out_drawShadowFlag) const;
	HRESULT GetShadowPassFlagOfAppliedTechnique(uint passIndex, bool& out_shadowPassFlag) const;

	bool IsValid() const;

	_declspec(property(get = GetTechniqueIndex, put = SetTechniqueIndex)) uint techniqueIndex;
	_declspec(property(get = IsValid)) bool isValid;

public:

	ResourceRef<Shader> GetShader() const;
	inline uint GetVariableCount() const { return (uint)m_shaderVariables.size(); }
	inline uint GetSpecificVariableCount() const { return (uint)m_specificShaderVariables.size(); }
	inline ShaderVariable* GetVariable(uint index) { return m_shaderVariables[index]; }
	inline ISpecificShaderVariable* GetSpecificVariable(uint index) { return m_specificShaderVariables[index]; }

	_declspec(property(get = GetShader)) ResourceRef<Shader> shader;
	_declspec(property(get = GetVariableCount)) uint variableCount;
	_declspec(property(get = GetSpecificVariableCount)) uint specificVariableCount;

public:

	static ResourceRef<Material> CreateMaterialByShaderM(ResourceManagement* management, ResourceRef<Shader> shader, const tstring& resourceKey);
	static ResourceRef<Material> CreateMaterialByShaderUM(ResourceManagement* management, ResourceRef<Shader> shader);

	static ResourceRef<Material> CopyMaterialM(ResourceManagement* management, ResourceRef<Material> material, const tstring& resourceKey);
	static ResourceRef<Material> CopyMaterialUM(ResourceManagement* management, ResourceRef<Material> material);

	static ResourceRef<Material> LoadMaterialM(ResourceManagement* management, const tstring& jsonPath);
	static ResourceRef<Material> LoadMaterialUM(ResourceManagement* management, const tstring& jsonPath);

	string ToJson() const;

private:

	static ResourceRef<Material> LoadMaterialFromJsonCommon(ResourceManagement* management, const tstring& jsonPath, const tstring* nullable_resourceKey);

private: 

	virtual HRESULT GetEffectDesc(Com<ID3DX11Effect>& out_effect) const final override;
	virtual HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) final override;
	virtual HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) final override;

private:

	void ApplyVariables();
	void ApplySpecificVariables(Com<ID3D11DeviceContext> deviceContext, ICamera* camera);

private:

	void SetupShaderVariables();
	void ApplyAnnotation(ShaderVariable* variable);

private:

	ResourceRef<Shader> m_shader;

	uint m_techniqueIndex = 0;

	vector<ShaderVariable*> m_shaderVariables;
	vector<ISpecificShaderVariable*> m_specificShaderVariables;
};

ENGINE_END
