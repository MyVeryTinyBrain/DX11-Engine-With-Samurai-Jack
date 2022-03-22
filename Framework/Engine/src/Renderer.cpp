#include "EnginePCH.h"
#include "Renderer.h"

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
