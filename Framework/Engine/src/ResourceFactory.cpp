#include "EnginePCH.h"
#include "ResourceFactory.h"
#include "Texture2D.h"
#include "RenderTexture2D.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "AudioClip.h"

ResourceFactory::ResourceFactory()
{
}

ResourceFactory::~ResourceFactory()
{
}

ResourceRef<Texture2D> ResourceFactory::CreateTexture2DM(const TextureCreateDesc& desc, const tstring& resourceKey)
{
	return Texture2D::CreateTexture2DM(m_management, desc, resourceKey);
}

ResourceRef<Texture2D> ResourceFactory::CreateTexture2DUM(const TextureCreateDesc& desc)
{
	return Texture2D::CreateTexture2DUM(m_management, desc);
}

ResourceRef<Texture2D> ResourceFactory::LoadTexture2DM(const TextureOptionDesc& desc, const tstring& path)
{
	return Texture2D::LoadTexture2DM(m_management, desc, path);
}

ResourceRef<Texture2D> ResourceFactory::LoadTexture2DUM(const TextureOptionDesc& desc, const tstring& path)
{
	return Texture2D::LoadTexture2DUM(m_management, desc, path);
}

ResourceRef<RenderTexture2D> ResourceFactory::CreateRenderTexture2DM(const tstring& resourceKey, unsigned int width, unsigned int height)
{
	return RenderTexture2D::CreateRenderTexture2DM(m_management, resourceKey, width, height);
}

ResourceRef<RenderTexture2D> ResourceFactory::CreateRenderTexture2DUM(unsigned int width, unsigned int height)
{
	return RenderTexture2D::CreateRenderTexture2DUM(m_management, width, height);
}

ResourceRef<Shader> ResourceFactory::LoadShaderFromFileM(const tstring& path)
{
	return Shader::LoadShaderFromFileM(m_management, path);
}

ResourceRef<Shader> ResourceFactory::LoadShaderFromBinaryFolderM(const tstring& path)
{
	return Shader::LoadShaderFromBinaryFolderM(m_management, path);
}

ResourceRef<Mesh> ResourceFactory::CreateMeshM(const tstring& resourceKey, const VIBuffer* viBuffer)
{
	return Mesh::CreateMeshM(m_management, resourceKey, viBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateMeshUM(const VIBuffer* viBuffer)
{
	return Mesh::CreateMeshUM(m_management, viBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateMeshNocopyM(const tstring& resourceKey, VIBuffer** ppVIBuffer)
{
	return Mesh::CreateMeshNocopyM(m_management, resourceKey, ppVIBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateMeshNocopyUM(VIBuffer** ppVIBuffer)
{
	return Mesh::CreateMeshNocopyUM(m_management, ppVIBuffer);
}

ResourceRef<Mesh> ResourceFactory::LoadMeshM(const tstring& path, bool withTextures)
{
	ResourceRef<Mesh> mesh = Mesh::LoadMeshM(m_management, path);

	if (mesh && withTextures)
	{
		vector<ModelMaterialDesc> materialDescs = mesh->materialDescs;
		for (uint i = 0; i < materialDescs.size(); ++i)
		{
			TextureOptionDesc desc = {};
			if (materialDescs[i].HasDiffuse()) LoadTexture2DM(desc, materialDescs[i].diffuse);
			if (materialDescs[i].HasNormals()) LoadTexture2DM(desc, materialDescs[i].normals);
			if (materialDescs[i].HasEmission()) LoadTexture2DM(desc, materialDescs[i].emission);
		}
	}

	return mesh;
}

ResourceRef<Material> ResourceFactory::CreateMaterialByShaderM(ResourceRef<Shader> shader, const tstring& resourceKey)
{
	return Material::CreateMaterialByShaderM(m_management, shader, resourceKey);
}

ResourceRef<Material> ResourceFactory::CreateMaterialByShaderUM(ResourceRef<Shader> shader)
{
	return Material::CreateMaterialByShaderUM(m_management, shader);
}

ResourceRef<Material> ResourceFactory::CopyMaterialM(ResourceRef<Material> material, const tstring& resourceKey)
{
	return Material::CopyMaterialM(m_management, material, resourceKey);
}

ResourceRef<Material> ResourceFactory::CopyMaterialUM(ResourceRef<Material> material)
{
	return Material::CopyMaterialUM(m_management, material);
}

ResourceRef<Material> ResourceFactory::LoadMaterialM(const tstring& jsonPath)
{
	return Material::LoadMaterialM(m_management, jsonPath);
}

ResourceRef<Material> ResourceFactory::LoadMaterialUM(const tstring& jsonPath)
{
	return Material::LoadMaterialUM(m_management, jsonPath);
}

ResourceRef<AudioClip> ResourceFactory::LoadAudioClipM(const tstring& path)
{
	return AudioClip::LoadAudioClipM(m_management, path);
}

ResourceRef<AudioClip> ResourceFactory::LoadAudioClipUM(const tstring& path)
{
	return AudioClip::LoadAudioClipUM(m_management, path);
}

ResourceManagement* ResourceFactory::GetManagement() const
{
	return m_management;
}

HRESULT ResourceFactory::Initialize(ResourceManagement* management)
{
	if (!management)
		return E_FAIL;

	m_management = management;

	return S_OK;
}
