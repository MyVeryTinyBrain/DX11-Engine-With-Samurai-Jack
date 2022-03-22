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
	float GetSpecularTransparency();
	float GetSpecularPower();
	const ResourceRef<Texture>& GetEmissiveTexture();
	const ResourceRef<Texture>& GetReflectionTexture();
	float GetReflectionTransparency();
	float GetReflectionBlur();
	float GetReflectMask();

	void SetNormalMapTexture(const ResourceRef<Texture>& texture);
	void SetLightMapTexture(const ResourceRef<Texture>& texture);
	void SetOcclusionTexture(const ResourceRef<Texture>& texture);
	void SetShadowMapTexture(const ResourceRef<Texture>& texture);
	void SetSpecularMapTexture(const ResourceRef<Texture>& texture);
	void SetSpecularTransparency(float value);
	void SetSpecularPower(float value);
	void SetEmissiveTexture(const ResourceRef<Texture>& texture);
	void SetReflectionTexture(const ResourceRef<Texture>& texture);
	void SetReflectionTransparency(float value);
	void SetReflectionBlur(float value);
	void SetReflectMask(float value);

	_declspec(property(get = GetNormalMapTexture, put = SetNormalMapTexture)) const ResourceRef<Texture>& normalMap;
	_declspec(property(get = GetLightMapTexture, put = SetLightMapTexture)) const ResourceRef<Texture>& lightMap;
	_declspec(property(get = GetOcclusionTexture, put = SetOcclusionTexture)) const ResourceRef<Texture>& occlusion;
	_declspec(property(get = GetShadowMapTexture, put = SetShadowMapTexture)) const ResourceRef<Texture>& shadowMap;
	_declspec(property(get = GetSpecularMapTexture, put = SetSpecularMapTexture)) const ResourceRef<Texture>& specularMap;
	_declspec(property(get = GetSpecularTransparency, put = SetSpecularTransparency)) float specularTransparency;
	_declspec(property(get = GetSpecularPower, put = SetSpecularPower)) float specularPower;
	_declspec(property(get = GetEmissiveTexture, put = SetEmissiveTexture)) const ResourceRef<Texture>& emissive;
	_declspec(property(get = GetReflectionTexture, put = SetReflectionTexture)) const ResourceRef<Texture>& reflection;
	_declspec(property(get = GetReflectionTransparency, put = SetReflectionTransparency)) float reflectionTransparency;
	_declspec(property(get = GetReflectionBlur, put = SetReflectionBlur)) float reflectionBlur;
	_declspec(property(get = GetReflectMask, put = SetReflectMask)) float reflectMask;

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

	// float _SpecularTransparency
	float m_specularTransparency = 1.0f;

	// float _SpecularPower;
	float m_specularPower = 5.0f;

	// texture2D _EmissiveTexture;
	ResourceRef<Texture> m_emissiveTexture;

	// texture2D _ReflectionTexture;
	ResourceRef<Texture> m_reflectionTexture;

	// float _ReflectionTransparency;
	float m_reflectionTransparency = 0.5f;

	// float _ReflectionBlur;
	float m_reflectionBlur = 1.0f;

	// float _ReflectMask;
	float m_reflectMask = 1.0f;
};

ENGINE_END
