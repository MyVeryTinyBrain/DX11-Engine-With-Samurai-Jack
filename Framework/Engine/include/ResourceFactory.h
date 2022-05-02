#pragma once

#include "IResourceFactory.h"
#include "ResourceRef.h"
#include "DxTextureCreateType.h"

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

	ResourceRef<Texture2D> CreateTexture2DM(const TextureCreateDesc& desc, const tstring& resourceKey);
	ResourceRef<Texture2D> CreateTexture2DUM(const TextureCreateDesc& desc);
	ResourceRef<Texture2D> LoadTexture2DM(const TextureOptionDesc& desc, const tstring& path);
	ResourceRef<Texture2D> LoadTexture2DUM(const TextureOptionDesc& desc, const tstring& path);

	ResourceRef<RenderTexture2D> CreateRenderTexture2DM(const tstring& resourceKey, unsigned int width, unsigned int height);
	ResourceRef<RenderTexture2D> CreateRenderTexture2DUM(unsigned int width, unsigned int height);

	ResourceRef<Shader> LoadShaderFromFileM(const tstring& path); 
	ResourceRef<Shader> LoadShaderFromBinaryFolderM(const tstring& path);

	ResourceRef<Mesh> CreateMeshM(const tstring& resourceKey, const VIBuffer* viBuffer);
	ResourceRef<Mesh> CreateMeshUM(const VIBuffer* viBuffer);
	ResourceRef<Mesh> CreateMeshNocopyM(const tstring& resourceKey, VIBuffer** ppVIBuffer);
	ResourceRef<Mesh> CreateMeshNocopyUM(VIBuffer** ppVIBuffer);
	ResourceRef<Mesh> LoadMeshM(const tstring& path, bool withTextures = true);

	ResourceRef<Material> CreateMaterialByShaderM(ResourceRef<Shader> shader, const tstring& resourceKey);
	ResourceRef<Material> CreateMaterialByShaderUM(ResourceRef<Shader> shader);
	ResourceRef<Material> CopyMaterialM(ResourceRef<Material> material, const tstring& resourceKey);
	ResourceRef<Material> CopyMaterialUM(ResourceRef<Material> material);
	ResourceRef<Material> LoadMaterialM(const tstring& jsonPath);
	ResourceRef<Material> LoadMaterialUM(const tstring& jsonPath);

public:

	ResourceManagement* GetManagement() const;
	
	_declspec(property(get = GetManagement)) ResourceManagement* management;

private:

	virtual HRESULT Initialize(ResourceManagement* management) override;

private:

	ResourceManagement* m_management = nullptr;

};

ENGINE_END
