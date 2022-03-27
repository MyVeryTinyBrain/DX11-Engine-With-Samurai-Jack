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

ResourceRef<Texture2D> ResourceFactory::CreateManagedTexture2DFromFile(const tstring& path, bool warning)
{
	return Texture2D::CreateManagedTexture2DFromFile(m_management, path, warning);
}

ResourceRef<Texture2D> ResourceFactory::CreateManagedTexture2DFromFile(const tstring& path, const tstring& groupName, bool warning)
{
	return Texture2D::CreateManagedTexture2DFromFile(m_management, groupName, path, warning);
}

ResourceRef<Texture2D> ResourceFactory::CreateUnmanagedTexture2DFromFile(const tstring& path, bool warning)
{
	return Texture2D::CreateUnmanagedTexture2DFromFile(m_management, path, warning);
}

ResourceRef<Texture2D> ResourceFactory::CreateManagedTexture2D(const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height)
{
	return Texture2D::CreateManagedTexture2D(m_management, resourceKey, color, width, height);
}

ResourceRef<Texture2D> ResourceFactory::CreateManagedTexture2D(const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height)
{
	return Texture2D::CreateManagedTexture2D(m_management, resourceKey, groupName, color, width, height);
}

ResourceRef<Texture2D> ResourceFactory::CreateUnmanagedTexture2D(const Color& color, unsigned int width, unsigned int height)
{
	return Texture2D::CreateUnmanagedTexture2D(m_management, color, width, height);
}

ResourceRef<Texture2D> ResourceFactory::CreateManagedDynamicTexture2D(const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height)
{
	return Texture2D::CreateManagedDynamicTexture2D(m_management, resourceKey, color, width, height);
}

ResourceRef<Texture2D> ResourceFactory::CreateManagedDynamicTexture2D(const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height)
{
	return Texture2D::CreateManagedDynamicTexture2D(m_management, resourceKey, groupName, color, width, height);
}

ResourceRef<Texture2D> ResourceFactory::CreateUnmanagedDynamicTexture2D(const Color& color, unsigned int width, unsigned int height)
{
	return Texture2D::CreateUnmanagedDynamicTexture2D(m_management, color, width, height);
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
			CreateManagedTexture2DFromFile(materialDescs[i].diffuse, false);
	}

	return mesh;
}

ResourceRef<Material> ResourceFactory::CreateManagedMaterialByShader(const tstring& shaderPath, const tstring& resourceKey)
{
	return Material::CreateManagedMaterialByShader(m_management, shaderPath, resourceKey);
}

ResourceRef<Material> ResourceFactory::CreateManagedMaterialByShader(const tstring& shaderPath, const tstring& resourceKey, const tstring& groupName)
{
	return Material::CreateManagedMaterialByShader(m_management, shaderPath, resourceKey, groupName);
}

ResourceRef<Material> ResourceFactory::CreateUnmanagedMaterialByShader(const tstring& shaderPath)
{
	return Material::CreateUnmanagedMaterialByShader(m_management, shaderPath);
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
