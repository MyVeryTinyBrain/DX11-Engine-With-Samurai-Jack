#pragma once

#include "IBuiltInResources.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Texture2D;
class Mesh;
class VI;
class Shader;
class Material;

class MaterialStandard;
class MaterialColorLine;
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

	const ResourceRef<Mesh>& GetQuadMesh() const;
	_declspec(property(get = GetQuadMesh)) const ResourceRef<Mesh>& quadMesh;

	const ResourceRef<Mesh>& GetPlaneMesh() const;
	_declspec(property(get = GetPlaneMesh)) const ResourceRef<Mesh>& planeMesh;

	const ResourceRef<Mesh>& GetBoxMesh() const;
	_declspec(property(get = GetBoxMesh)) const ResourceRef<Mesh>& boxMesh;

	const ResourceRef<Mesh>& GetSphereMesh() const;
	_declspec(property(get = GetSphereMesh)) const ResourceRef<Mesh>& sphereMesh;

	const ResourceRef<Mesh>& GetCapsuleMesh() const;
	_declspec(property(get = GetCapsuleMesh)) const ResourceRef<Mesh>& capsuleMesh;

	const ResourceRef<Mesh>& GetCylinderMesh() const;
	_declspec(property(get = GetCylinderMesh)) const ResourceRef<Mesh>& cylinderMesh;

	const ResourceRef<Shader>& GetStandardShader() const;
	_declspec(property(get = GetStandardShader)) const ResourceRef<Shader>& standardShader;

	const ResourceRef<Shader>& GetColorLineShader() const;
	_declspec(property(get = GetColorLineShader)) const ResourceRef<Shader>& colorLineShader;

	const ResourceRef<Shader>& GetGizmoTranslationShader() const;
	_declspec(property(get = GetGizmoTranslationShader)) const ResourceRef<Shader>& gizmoTranslationShader;

	const ResourceRef<Shader>& GetGizmoRotationNoHighlightShader() const;
	_declspec(property(get = GetGizmoRotationNoHighlightShader)) const ResourceRef<Shader>& gizmoRotationNoHighlightShader;

	const ResourceRef<Shader>& GetGizmoRotationHighlightShader() const;
	_declspec(property(get = GetGizmoRotationHighlightShader)) const ResourceRef<Shader>& gizmoRotationHighlightShader;

	const ResourceRef<Shader>& GetGizmoRotationLineShader() const;
	_declspec(property(get = GetGizmoRotationLineShader)) const ResourceRef<Shader>& gizmoRotationLineShader;

	const ResourceRef<MaterialStandard>& GetStandardMaterial() const;
	_declspec(property(get = GetStandardMaterial)) const ResourceRef<MaterialStandard>& standardMaterial;

	const ResourceRef<MaterialColorLine>& GetGreenColorLineMaterial() const;
	_declspec(property(get = GetGreenColorLineMaterial)) const ResourceRef<MaterialColorLine>& greenColorLineMaterial;

public:

	System* GetSystem() const;

	_declspec(property(get = GetSystem)) System* system;

private:

	HRESULT CreateTexture2D(const Color& color, unsigned int width, unsigned int height, ResourceRef<Texture2D>* out_tex2D);

	HRESULT CreateMeshNocopyVI(VI* vi, ResourceRef<Mesh>* out_mesh);

	HRESULT CreateShader(const tstring& shaderPath, ResourceRef<Shader>* out_shader);

private:

	ResourceFactory* m_factory = nullptr;

private: // Built-In resources

	// Texture2D ==========================================
	
	ResourceRef<Texture2D> m_white;
	ResourceRef<Texture2D> m_black;

	ResourceRef<Texture2D> m_red;
	ResourceRef<Texture2D> m_green;
	ResourceRef<Texture2D> m_blue;

	ResourceRef<Texture2D> m_whiteTransparent;
	ResourceRef<Texture2D> m_blackTransparent;

	// Mesh ===============================================

	ResourceRef<Mesh> m_quad;

	ResourceRef<Mesh> m_plane;

	ResourceRef<Mesh> m_box;

	ResourceRef<Mesh> m_sphere;

	ResourceRef<Mesh> m_capsule;

	ResourceRef<Mesh> m_cylinder;

	// Shader =============================================

	ResourceRef<Shader> m_standardShader;

	ResourceRef<Shader> m_colorLineShader;

	ResourceRef<Shader> m_gizmoTranslationShader;

	ResourceRef<Shader> m_gizmoRotationNoHighlightShader;

	ResourceRef<Shader> m_gizmoRotationHighlightShader;

	ResourceRef<Shader> m_gizmoRotationLineShader;

	// Material ===========================================

	ResourceRef<MaterialStandard> m_standardMaterial;

	ResourceRef<MaterialColorLine> m_greenColorLineMaterial;
};

ENGINE_END
