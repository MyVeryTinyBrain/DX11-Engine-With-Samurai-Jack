#pragma once

#include "IBuiltInResources.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Texture2D;
class Mesh;
class VI;
class Shader;
class Material;
class ENGINE_API BuiltInResources : public IBuiltInResources
{
public:

	BuiltInResources();

	virtual ~BuiltInResources();

private:

	virtual HRESULT Initialize(ResourceFactory* factory) override;

	HRESULT CreateBuiltInResources();

	void ReleaseBuiltInResources();

public:

	const ResourceRef<Texture2D>& GetWhiteTexture() const;
	_declspec(property(get = GetWhiteTexture)) const ResourceRef<Texture2D>& whiteTexture;

	const ResourceRef<Texture2D>& GetBlackTexture() const;
	_declspec(property(get = GetBlackTexture)) const ResourceRef<Texture2D>& blackTexture;

	const ResourceRef<Texture2D>& GetClearTexture() const;
	_declspec(property(get = GetClearTexture)) const ResourceRef<Texture2D>& clearTexture;

	const ResourceRef<Texture2D>& GetRedTexture() const;
	_declspec(property(get = GetRedTexture)) const ResourceRef<Texture2D>& redTexture;

	const ResourceRef<Texture2D>& GetGreenTexture() const;
	_declspec(property(get = GetGreenTexture)) const ResourceRef<Texture2D>& greenTexture;

	const ResourceRef<Texture2D>& GetBlueTexture() const;
	_declspec(property(get = GetBlueTexture)) const ResourceRef<Texture2D>& blueTexture;

	const ResourceRef<Texture2D>& GetTransparentWhiteTexture() const;
	_declspec(property(get = GetTransparentWhiteTexture)) const ResourceRef<Texture2D>& whiteTransparentTexture;

	const ResourceRef<Texture2D>& GetTransparentBlackTexture() const;
	_declspec(property(get = GetTransparentBlackTexture)) const ResourceRef<Texture2D>& blackTransparentTexture;

	const ResourceRef<Texture2D>& GetNormalTexture() const;
	_declspec(property(get = GetNormalTexture)) const ResourceRef<Texture2D>& normalTexture;

	const ResourceRef<Mesh>& GetQuadMesh() const;
	_declspec(property(get = GetQuadMesh)) const ResourceRef<Mesh>& quadMesh;

	const ResourceRef<Mesh>& GetPlaneMesh() const;
	_declspec(property(get = GetPlaneMesh)) const ResourceRef<Mesh>& planeMesh;

	const ResourceRef<Mesh>& GetBoxMesh() const;
	_declspec(property(get = GetBoxMesh)) const ResourceRef<Mesh>& boxMesh;

	const ResourceRef<Mesh>& GetSkyBoxMesh() const;
	_declspec(property(get = GetSkyBoxMesh)) const ResourceRef<Mesh>& skyboxMesh;

	const ResourceRef<Mesh>& GetSphereMesh() const;
	_declspec(property(get = GetSphereMesh)) const ResourceRef<Mesh>& sphereMesh;

	const ResourceRef<Mesh>& GetCapsuleMesh() const;
	_declspec(property(get = GetCapsuleMesh)) const ResourceRef<Mesh>& capsuleMesh;

	const ResourceRef<Mesh>& GetCylinderMesh() const;
	_declspec(property(get = GetCylinderMesh)) const ResourceRef<Mesh>& cylinderMesh;

	const ResourceRef<Shader>& GetStandardShader() const;
	_declspec(property(get = GetStandardShader)) const ResourceRef<Shader>& standardShader;

	const ResourceRef<Shader>& GetColorShader() const;
	_declspec(property(get = GetColorShader)) const ResourceRef<Shader>& colorShader;

	const ResourceRef<Material>& GetStandardMaterial() const;
	_declspec(property(get = GetStandardMaterial)) const ResourceRef<Material>& standardMaterial;

	const ResourceRef<Material>& GetWireframeMaterial() const;
	_declspec(property(get = GetWireframeMaterial)) const ResourceRef<Material>& wireframeMaterial;

public:

	System* GetSystem() const;

	_declspec(property(get = GetSystem)) System* system;

private:

	HRESULT CreateTexture2D(const Color& color, unsigned int width, unsigned int height, tstring resourceKey, ResourceRef<Texture2D>* out_tex2D);

	HRESULT CreateMeshNocopyVI(VI* vi, ResourceRef<Mesh>* out_mesh);

	HRESULT CreateShader(const tstring& shaderPath, ResourceRef<Shader>* out_shader);

private:

	ResourceFactory* m_factory = nullptr;

private: // Built-In resources

	// Texture2D ==========================================
	
	ResourceRef<Texture2D> m_white;
	ResourceRef<Texture2D> m_black;
	ResourceRef<Texture2D> m_clear;

	ResourceRef<Texture2D> m_red;
	ResourceRef<Texture2D> m_green;
	ResourceRef<Texture2D> m_blue;

	ResourceRef<Texture2D> m_normal;

	ResourceRef<Texture2D> m_whiteTransparent;
	ResourceRef<Texture2D> m_blackTransparent;

	// Mesh ===============================================

	ResourceRef<Mesh> m_quad;

	ResourceRef<Mesh> m_plane;

	ResourceRef<Mesh> m_box;

	ResourceRef<Mesh> m_skybox;

	ResourceRef<Mesh> m_sphere;

	ResourceRef<Mesh> m_capsule;

	ResourceRef<Mesh> m_cylinder;

	// Shader =============================================

	ResourceRef<Shader> m_standardShader;

	ResourceRef<Shader> m_colorShader;

	// Material ===========================================

	ResourceRef<Material> m_standardMaterial;

	ResourceRef<Material> m_wireframeMaterial;
};

ENGINE_END
