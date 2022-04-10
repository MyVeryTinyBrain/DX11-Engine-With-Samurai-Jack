#pragma once

#include "IResourceFactory.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Texture2D;
class RenderTexture2D;
class Shader;
class Mesh;
class VIBuffer;
class Material;
ENGINE_END

ENGINE_BEGIN
class ENGINE_API ResourceFactory : public IResourceFactory
{
public:

	ResourceFactory();

	virtual ~ResourceFactory();

public:

	ResourceRef<Texture2D> CreateManagedTexture2DFromFile(const tstring& path, bool warning = true);
	ResourceRef<Texture2D> CreateManagedTexture2DFromFile(const tstring& path, const tstring& groupName, bool warning = true);
	ResourceRef<Texture2D> CreateUnmanagedTexture2DFromFile(const tstring& path, bool warning = true);
	ResourceRef<Texture2D> CreateManagedTexture2D(const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height);
	ResourceRef<Texture2D> CreateManagedTexture2D(const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height);
	ResourceRef<Texture2D> CreateUnmanagedTexture2D(const Color& color, unsigned int width, unsigned int height);
	ResourceRef<Texture2D> CreateManagedDynamicTexture2D(const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height);
	ResourceRef<Texture2D> CreateManagedDynamicTexture2D(const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height);
	ResourceRef<Texture2D> CreateUnmanagedDynamicTexture2D(const Color& color, unsigned int width, unsigned int height);

	ResourceRef<RenderTexture2D> CreateManagedRenderTexture2D(const tstring& resourceKey, unsigned int width, unsigned int height);
	ResourceRef<RenderTexture2D> CreateManagedRenderTexture2D(const tstring& resourceKey, const tstring& groupName, unsigned int width, unsigned int height);
	ResourceRef<RenderTexture2D> CreateUnmanagedRenderTexture2D(unsigned int width, unsigned int height);

	ResourceRef<Shader> CreateManagedShaderFromFile(const tstring& path); 
	ResourceRef<Shader> CreateManagedShaderFromFile(const tstring& path, const tstring& groupName);
	ResourceRef<Shader> CreateManagedShaderFromBinaryFolder(const tstring& path);
	ResourceRef<Shader> CreateManagedShaderFromBinaryFolder(const tstring& path, const tstring& groupName);

	ResourceRef<Mesh> CreateManagedMesh(const tstring& resourceKey, const VIBuffer* viBuffer);
	ResourceRef<Mesh> CreateManagedMesh(const tstring& resourceKey, const tstring& groupName, const VIBuffer* viBuffer);
	ResourceRef<Mesh> CreateUnamanagedMesh(const VIBuffer* viBuffer);
	ResourceRef<Mesh> CreateManagedMeshNocopy(const tstring& resourceKey, VIBuffer** ppVIBuffer);
	ResourceRef<Mesh> CreateManagedMeshNocopy(const tstring& resourceKey, const tstring& groupName, VIBuffer** ppVIBuffer);
	ResourceRef<Mesh> CreateUnamanagedMeshNocopy(VIBuffer** ppVIBuffer);
	ResourceRef<Mesh> CreateManagedMeshFromFile(const tstring& path, bool withTextures = true);

	ResourceRef<Material> CreateManagedMaterialByShader(ResourceRef<Shader> shader, const tstring& resourceKey);
	ResourceRef<Material> CreateManagedMaterialByShader(ResourceRef<Shader> shader, const tstring& resourceKey, const tstring& groupName);
	ResourceRef<Material> CreateUnmanagedMaterialByShader(ResourceRef<Shader> shader);
	ResourceRef<Material> CopyManagedMaterial(ResourceRef<Material> material, const tstring& resourceKey);
	ResourceRef<Material> CopyManagedMaterial(ResourceRef<Material> material, const tstring& resourceKey, const tstring& groupName);
	ResourceRef<Material> CopyUnmanagedMaterial(ResourceRef<Material> material);
	ResourceRef<Material> CreateManagedMaterialFromJson(const tstring& jsonPath);
	ResourceRef<Material> CreateManagedMaterialFromJson(const tstring& jsonPath, const tstring& groupName);
	ResourceRef<Material> CreateUnmanagedMaterialFromJson(const tstring& jsonPath);

public:

	ResourceManagement* GetManagement() const;
	
	_declspec(property(get = GetManagement)) ResourceManagement* management;

private:

	virtual HRESULT Initialize(ResourceManagement* management) override;

private:

	ResourceManagement* m_management = nullptr;

};

ENGINE_END
