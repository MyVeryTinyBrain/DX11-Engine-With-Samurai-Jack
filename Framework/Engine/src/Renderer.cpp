#include "EnginePCH.h"
#include "Renderer.h"
#include "Mesh.h"
#include "System.h"
#include "ResourceManagement.h"
#include "BuiltInResources.h"
#include "Material.h"
#include "ResourceRef.h"
#include "Texture.h"
#include "ResourceFactory.h"
#include "MaterialUtil.h"

ResourceRef<Material> Renderer::GetMaterial() const
{
    if (m_materials.empty())
        return ResourceRef<Material>();

    return m_materials.front();
}

void Renderer::SetMaterial(ResourceRef<Material> material)
{
    if (m_materials.empty())
        m_materials.push_back(material);
    else
        m_materials[0] = material;
}

void Renderer::SetMaterialByIndex(uint index, ResourceRef<Material> material)
{
    if (index >= uint(m_materials.size()))
        return;

    m_materials[index] = material;
}

const vector<ResourceRef<Material>>& Renderer::GetMaterials() const
{
    return m_materials;
}

void Renderer::SetMaterials(const vector<ResourceRef<Material>>& materials)
{
    m_materials = materials;
}

ResourceRef<Material> Renderer::GetMaterialByIndex(uint index) const
{
    if (index >= uint(m_materials.size()))
        return ResourceRef<Material>();

    return m_materials[index];
}

void Renderer::SetMaterialCount(uint count)
{
    vector<ResourceRef<Material>> materials;
    materials.resize(count);

    for (uint i = 0; i < uint(m_materials.size()); ++i)
    {
        if (i >= count)
            break;

        materials[i] = std::move(m_materials[i]);
    }

    m_materials.swap(materials);
}

uint Renderer::GetMaterialCount() const
{
    return uint(m_materials.size());
}

void Renderer::ClearMaterials()
{
    m_materials.clear();
}

void Renderer::SetupStandardMaterials(bool instance)
{
    if (!instance)
    {
        MaterialUtil::SetupPBRMaterials(this, system->resource->builtIn->standardShader, 0);
    }
    else
    {
        MaterialUtil::SetupInstancePBRMaterials(this, system->resource->builtIn->standardInstancingShader, 0);
    }
}

void Renderer::SetupStandardNoShadowMaterials(bool instance)
{
    if (!instance)
    {
        MaterialUtil::SetupPBRMaterials(this, system->resource->builtIn->standardShader, 1);
    }
    else
    {
        MaterialUtil::SetupInstancePBRMaterials(this, system->resource->builtIn->standardInstancingShader, 1);
    }
}

void Renderer::SetupDefaultMaterials()
{
    ClearMaterials();

    if (!m_mesh)
    {
        return;
    }

    uint numMaterials = m_mesh->subMeshCount;
    SetMaterialCount(numMaterials);

    for (uint i = 0; i < numMaterials; ++i)
    {
        SetMaterialByIndex(i, system->resource->builtIn->standardMaterial);
    }
}

void Renderer::SetInstanceData(uint index, const Real4& data)
{
    assert(index <= 3); // ����Ҽ� �ִ� �ν��Ͻ� �������� �ε����� �ʰ��߽��ϴ�.
    m_instanceDatas[index] = data;
}

const Real4& Renderer::GetInstanceData(uint index)
{
    assert(index <= 3); // ����Ҽ� �ִ� �ν��Ͻ� �������� �ε����� �ʰ��߽��ϴ�.
    return m_instanceDatas[index];
}

bool Renderer::IsValid() const
{
    if (m_materials.empty())
        return false;

    if (!m_mesh)
        return false;

    return true;
}

ResourceRef<Mesh> Renderer::GetProtectedMesh() const
{
    return m_mesh;
}
