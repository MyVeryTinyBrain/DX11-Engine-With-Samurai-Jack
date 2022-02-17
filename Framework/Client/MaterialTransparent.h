#pragma once

class MaterialTransparent : public Material
{
	virtual tstring GetShaderPath() const override;
	virtual void OnCreated() override;
	virtual void OnSetMaterialValues() override;

public:

	const ResourceRef<Texture>& GetNormalMapTexture();

	void SetNormalMapTexture(const ResourceRef<Texture>& texture);

	_declspec(property(get = GetNormalMapTexture, put = SetNormalMapTexture)) const ResourceRef<Texture>& normalMap;

private:

	// texture2D _NormalMapTexture
	ResourceRef<Texture> m_normalMapTexture;
};

