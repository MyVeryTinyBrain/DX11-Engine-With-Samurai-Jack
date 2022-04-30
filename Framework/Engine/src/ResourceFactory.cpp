#include "EnginePCH.h"
#include "ResourceFactory.h"
#include "Texture2D.h"
#include "RenderTexture2D.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"

ResourceFactory::ResourceFactory()
{
}

ResourceFactory::~ResourceFactory()
{
}

ResourceRef<Texture2D> ResourceFactory::CreateTexture2DM(const TextureCreateDesc& desc, const tstring& resourceKey, const tstring& groupName)
{
	return Texture2D::CreateTexture2DM(m_management, desc, resourceKey, groupName);
}

ResourceRef<Texture2D> ResourceFactory::CreateTexture2DUM(const TextureCreateDesc& desc)
{
	return Texture2D::CreateTexture2DUM(m_management, desc);
}

ResourceRef<Texture2D> ResourceFactory::LoadTexture2DM(const TextureOptionDesc& desc, const tstring& path, const tstring& groupName)
{
	return Texture2D::LoadTexture2DM(m_management, desc, path, groupName);
}

ResourceRef<Texture2D> ResourceFactory::LoadTexture2DUM(const TextureOptionDesc& desc, const tstring& path)
{
	return Texture2D::LoadTexture2DUM(m_management, desc, path);
}

ResourceRef<RenderTexture2D> ResourceFactory::CreateManagedRenderTexture2D(const tstring& resourceKey, unsigned int width, unsigned int height)
{
	return RenderTexture2D::CreateManagedRenderTexture2D(m_management, resourceKey, width, height);
}

ResourceRef<RenderTexture2D> ResourceFactory::CreateManagedRenderTexture2D(const tstring& resourceKey, const tstring& groupName, unsigned int width, unsigned int height)
{
	return RenderTexture2D::CreateManagedRenderTexture2D(m_management, resourceKey, groupName, width, height);
}

ResourceRef<RenderTexture2D> ResourceFactory::CreateUnmanagedRenderTexture2D(unsigned int width, unsigned int height)
{
	return RenderTexture2D::CreateUnmanagedRenderTexture2D(m_management, width, height);
}

ResourceRef<Shader> ResourceFactory::CreateManagedShaderFromFile(const tstring& path)
{
	return Shader::CreateManagedShaderFromFile(m_management, path);
}

ResourceRef<Shader> ResourceFactory::CreateManagedShaderFromFile(const tstring& path, const tstring& groupName)
{
	return Shader::CreateManagedShaderFromFile(m_management, path, groupName);
}

ResourceRef<Shader> ResourceFactory::CreateManagedShaderFromBinaryFolder(const tstring& path)
{
	return Shader::CreateManagedShaderFromBinaryFolder(m_management, path);
}

ResourceRef<Shader> ResourceFactory::CreateManagedShaderFromBinaryFolder(const tstring& path, const tstring& groupName)
{
	return Shader::CreateManagedShaderFromBinaryFolder(m_management, path, groupName);
}

ResourceRef<Mesh> ResourceFactory::CreateManagedMesh(const tstring& resourceKey, const VIBuffer* viBuffer)
{
	return Mesh::CreateManagedMesh(m_management, resourceKey, viBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateManagedMesh(const tstring& resourceKey, const tstring& groupName, const VIBuffer* viBuffer)
{
	return Mesh::CreateManagedMesh(m_management, resourceKey, groupName, viBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateUnamanagedMesh(const VIBuffer* viBuffer)
{
	return Mesh::CreateUnamanagedMesh(m_management, viBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateManagedMeshNocopy(const tstring& resourceKey, VIBuffer** ppVIBuffer)
{
	return Mesh::CreateManagedMeshNocopy(m_management, resourceKey, ppVIBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateManagedMeshNocopy(const tstring& resourceKey, const tstring& groupName, VIBuffer** ppVIBuffer)
{
	return Mesh::CreateManagedMeshNocopy(m_management, resourceKey, groupName, ppVIBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateUnamanagedMeshNocopy(VIBuffer** ppVIBuffer)
{
	return Mesh::CreateUnamanagedMeshNocopy(m_management, ppVIBuffer);
}

ResourceRef<Mesh> ResourceFactory::CreateManagedMeshFromFile(const tstring& path, bool withTextures)
{
	ResourceRef<Mesh> mesh = Mesh::CreateManagedMeshFromFile(m_management, path);

	if (mesh && withTextures)
	{
		vector<ModelMaterialDesc> materialDescs = mesh->materialDescs;
		for (uint i = 0; i < materialDescs.size(); ++i)
		{
			TextureOptionDesc desc = {};
			LoadTexture2DM(desc, materialDescs[i].diffuse);
		}
	}

	return mesh;
}

ResourceRef<Material> ResourceFactory::CreateManagedMaterialByShader(ResourceRef<Shader> shader, const tstring& resourceKey)
{
	return Material::CreateManagedMaterialByShader(m_management, shader, resourceKey);
}

ResourceRef<Material> ResourceFactory::CreateManagedMaterialByShader(ResourceRef<Shader> shader, const tstring& resourceKey, const tstring& groupName)
{
	return Material::CreateManagedMaterialByShader(m_management, shader, resourceKey, groupName);
}

ResourceRef<Material> ResourceFactory::CreateUnmanagedMaterialByShader(ResourceRef<Shader> shader)
{
	return Material::CreateUnmanagedMaterialByShader(m_management, shader);
}

ResourceRef<Material> ResourceFactory::CopyManagedMaterial(ResourceRef<Material> material, const tstring& resourceKey)
{
	return Material::CopyManagedMaterial(m_management, material, resourceKey);
}

ResourceRef<Material> ResourceFactory::CopyManagedMaterial(ResourceRef<Material> material, const tstring& resourceKey, const tstring& groupName)
{
	return Material::CopyManagedMaterial(m_management, material, resourceKey, groupName);
}

ResourceRef<Material> ResourceFactory::CopyUnmanagedMaterial(ResourceRef<Material> material)
{
	return Material::CopyUnmanagedMaterial(m_management, material);
}

ResourceRef<Material> ResourceFactory::CreateManagedMaterialFromJson(const tstring& jsonPath)
{
	return Material::CreateManagedMaterialFromJson(m_management, jsonPath);
}

ResourceRef<Material> ResourceFactory::CreateManagedMaterialFromJson(const tstring& jsonPath, const tstring& groupName)
{
	return Material::CreateManagedMaterialFromJson(m_management, jsonPath, groupName);
}

ResourceRef<Material> ResourceFactory::CreateUnmanagedMaterialFromJson(const tstring& jsonPath)
{
	return Material::CreateUnmanagedMaterialFromJson(m_management, jsonPath);
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
