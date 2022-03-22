#pragma once

#include "ResourceObject.h"
#include "IMaterial.h"
#include "ShaderTypes.h"

ENGINE_BEGIN
class Shader;
class Texture;
class ENGINE_API Material abstract : public ResourceObject, public IMaterial
{
public:

	Material();

	virtual ~Material();

	virtual tstring GetShaderPath() const = 0;

	virtual void OnCreated() = 0;

	virtual void OnSetMaterialValues() = 0;

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
	HRESULT GetShadowCutoffEnableFlagOfAppliedTechnique(uint passIndex, bool& out_shadowCutoffEnableFlag) const;
	HRESULT GetShadowCutoffAlphaOfAppliedTechnique(uint passIndex, float& out_shadowCutoffAlpha) const;

	bool IsValid() const;

	_declspec(property(get = GetTechniqueIndex, put = SetTechniqueIndex)) uint techniqueIndex;
	_declspec(property(get = IsValid)) bool isValid;

public:

	ResourceRef<Shader> GetShader() const;

	ResourceRef<Texture> GetDiffuseTexture() const;

	void SetDiffuseTexture(const ResourceRef<Texture>& texture);

	_declspec(property(get = GetDiffuseTexture, put = SetDiffuseTexture)) ResourceRef<Texture> diffuseTexture;

public:

	template <class MaterialType>
	static ResourceRef<MaterialType> CreateManagedMaterial(ResourceManagement* management, const tstring& resourceKey);

	template <class MaterialType>
	static ResourceRef<MaterialType> CreateManagedMaterial(ResourceManagement* management, const tstring& resourceKey, const tstring& groupName);

	template <class MaterialType>
	static ResourceRef<MaterialType> CreateUnmanagedMaterial(ResourceManagement* management);

private: 

	virtual void SetMaterialValues() final override;

	virtual HRESULT GetEffectDesc(Com<ID3DX11Effect>& out_effect) const final override;

	virtual HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) final override;

	virtual HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) final override;

private:

	bool Initialize(ResourceManagement* management, bool managed, const tstring& resourceKey, const tstring& groupName);

	bool SetupShader();

	bool SetupDefaultDiffuseTexture();

private:

	ResourceRef<Shader> m_shader;

	uint m_techniqueIndex = 0;

	// texture2D _DiffuseTexture
	ResourceRef<Texture> m_diffuseTexture;


};

ENGINE_END

#include "Material.hpp"
