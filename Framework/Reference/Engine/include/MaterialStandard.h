#pragma once

#include "Material.h"

ENGINE_BEGIN

class ENGINE_API MaterialStandard : public Material
{
public:

	MaterialStandard();

private:

	virtual ~MaterialStandard();
	virtual tstring GetShaderPath() const override;
	virtual void OnCreated() override;
	virtual void OnSetMaterialValues() override;

public:

	const ResourceRef<Texture>& GetNormalMapTexture();
	const ResourceRef<Texture>& GetLightMapTexture();
	const ResourceRef<Texture>& GetOcclusionTexture();
	const ResourceRef<Texture>& GetShadowMapTexture();
	const ResourceRef<Texture>& GetSpecularMapTexture();
	float GetSpecularPower();

	void SetNormalMapTexture(const ResourceRef<Texture>& texture);
	void SetLightMapTexture(const ResourceRef<Texture>& texture);
	void SetOcclusionTexture(const ResourceRef<Texture>& texture);
	void SetShadowMapTexture(const ResourceRef<Texture>& texture);
	void SetSpecularMapTexture(const ResourceRef<Texture>& texture);
	void SetSpecularPower(float value);

	_declspec(property(get = GetNormalMapTexture, put = SetNormalMapTexture)) const ResourceRef<Texture>& normalMap;
	_declspec(property(get = GetLightMapTexture, put = SetLightMapTexture)) const ResourceRef<Texture>& lightMap;
	_declspec(property(get = GetOcclusionTexture, put = SetOcclusionTexture)) const ResourceRef<Texture>& occlusion;
	_declspec(property(get = GetShadowMapTexture, put = SetShadowMapTexture)) const ResourceRef<Texture>& shadowMap;
	_declspec(property(get = GetSpecularMapTexture, put = SetSpecularMapTexture)) const ResourceRef<Texture>& specularMap;
	_declspec(property(get = GetSpecularPower, put = SetSpecularPower)) float specularPower;

private:

	// texture2D _NormalMapTexture
	ResourceRef<Texture> m_normalMapTexture;

	// texture2D _LightMapTexture;
	ResourceRef<Texture> m_lightMapTexture;

	// texture2D _OcclusionTexture;
	ResourceRef<Texture> m_occlusionTexture;

	// texture2D _ShadowMapTexture;
	ResourceRef<Texture> m_shadowMapTexture;

	// texture2D _SpecularMapTexture;
	ResourceRef<Texture> m_specularMapTexture;

	// float _SpecularPower;
	float m_specularPower = 0.5f;
};

ENGINE_END
