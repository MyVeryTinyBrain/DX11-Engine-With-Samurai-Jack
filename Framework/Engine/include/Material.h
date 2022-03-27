#pragma once

#include "ResourceObject.h"
#include "IMaterial.h"
#include "ShaderTypes.h"

ENGINE_BEGIN
class Shader;
class Texture;
class ShaderVariable;
class ISpecificShaderVariable;
class ENGINE_API Material : public ResourceObject, public IMaterial
{
public:

	Material(ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName, ResourceRef<Shader> shader);

	virtual ~Material();

	virtual tstring GetShaderPath() const;

	virtual void OnCreated() {};

	virtual void ApplyMaterial(ICamera* camera) override;

public:

	HRESULT SetRawValue(const string& name, const void* data, size_t size);
	HRESULT SetFloat(const string& name, float value);
	HRESULT SetVector(const string& name, const V4& value);
	HRESULT SetColor(const string& name, const Color& value);
	HRESULT SetMatrix(const string& name, const M4& value);
	HRESULT SetTexture(const string& name, ResourceRef<Texture> texture);
	HRESULT SetTextures(const string& name, ResourceRef<Texture>* textures, uint count);

public:

	virtual HRESULT GetTechniqueCount(uint& out_techniqueCount) const final override;
	virtual HRESULT GetPassCount(uint techniqueIndex, uint& out_passCount) const final override;

	inline void SetTechniqueIndex(uint index) { m_techniqueIndex = index; }
	inline uint GetTechniqueIndex() const { return m_techniqueIndex; }

	uint GetPassCountOfAppliedTechnique() const;
	HRESULT GetRenderGroupOfAppliedTechnique(uint passIndex, RenderGroup& out_renderGroup) const;
	HRESULT GetRenderGroupOrderOfAppliedTechnique(uint passIndex, int& out_renderGroupOrder) const;
	HRESULT GetInstancingFlagOfAppliedTechnique(uint passIndex, bool& out_instancingFlag) const;
	HRESULT GetDrawShadowFlagOfAppliedTechnique(uint passIndex, bool& out_drawShadowFlag) const;
	HRESULT GetShadowPassFlagOfAppliedTechnique(uint passIndex, bool& out_shadowPassFlag) const;

	bool IsValid() const;

	_declspec(property(get = GetTechniqueIndex, put = SetTechniqueIndex)) uint techniqueIndex;
	_declspec(property(get = IsValid)) bool isValid;

public:

	ResourceRef<Shader> GetShader() const;

public:

	static ResourceRef<Material> CreateManagedMaterialByShader(ResourceManagement* management, const tstring& shaderPath, const tstring& resourceKey);
	static ResourceRef<Material> CreateManagedMaterialByShader(ResourceManagement* management, const tstring& shaderPath, const tstring& resourceKey, const tstring& groupName);
	static ResourceRef<Material> CreateUnmanagedMaterialByShader(ResourceManagement* management, const tstring& shaderPath);

private: 

	virtual HRESULT GetEffectDesc(Com<ID3DX11Effect>& out_effect) const final override;
	virtual HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) final override;
	virtual HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) final override;

private:

	void ApplyVariables();
	void ApplySpecificVariables(ICamera* camera);

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
