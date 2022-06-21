#include "EnginePCH.h"
#include "MaterialUtil.h"
#include "Shader.h"
#include "Renderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "System.h"
#include "ResourceManagement.h"
#include "ResourceFactory.h"

void MaterialUtil::SetupPBRMaterials(Renderer* renderer, ResourceRef<Shader> shader, uint techniqueIndex)
{
    renderer->ClearMaterials();

    System* system = renderer->system;
    ResourceRef<Mesh> mesh = renderer->GetProtectedMesh();

    if (!mesh || !shader)
    {
        return;
    }

    vector<ModelMaterialDesc> materialDescs = mesh->materialDescs;
    vector<uint> materialIndices = mesh->materialIndices;

    uint numMaterials = mesh->subMeshCount;
    renderer->SetMaterialCount(numMaterials);

    for (uint i = 0; i < numMaterials; ++i)
    {
        ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
        material->techniqueIndex = techniqueIndex;

        ResourceRef<Texture> albedoTexture;
        ResourceRef<Texture> normalMapTexture;
        ResourceRef<Texture> emissiveTexture;
        ResourceRef<Texture> shinessTexture;

        if (i < (uint)materialIndices.size())
        {
            const uint& materialIndex = materialIndices[i];
            const ModelMaterialDesc& materialDesc = materialDescs[materialIndex];

            if (materialDesc.HasDiffuse())
            {
                albedoTexture = system->resource->Find(materialDesc.diffuse);
            }
            if (materialDesc.HasShininess())
            {
                shinessTexture = system->resource->Find(materialDesc.shininess);
            }
            if (materialDesc.HasNormals())
            {
                normalMapTexture = system->resource->Find(materialDesc.normals);
            }
            if (materialDesc.HasEmission())
            {
                emissiveTexture = system->resource->Find(materialDesc.emission);
            }
        }

        if (albedoTexture)
        {
            material->SetTexture("_AlbedoTexture", albedoTexture);
        }
        if (shinessTexture)
        {
            material->SetTexture("_RoughnessTexture", shinessTexture);
        }
        if (shinessTexture)
        {
            material->SetTexture("_MetallicTexture", shinessTexture);
        }
        if (normalMapTexture)
        {
            material->SetTexture("_NormalMapTexture", normalMapTexture);
        }
        if (emissiveTexture)
        {
            material->SetTexture("_EmissiveTexture", emissiveTexture);
        }

        renderer->SetMaterialByIndex(i, material);
    }
}

void MaterialUtil::SetupInstancePBRMaterials(Renderer* renderer, ResourceRef<Shader> shader, uint techniqueIndex)
{
    renderer->ClearMaterials();

    System* system = renderer->system;
    ResourceRef<Mesh> mesh = renderer->GetProtectedMesh();

    if (!mesh || !shader)
    {
        return;
    }

    vector<ModelMaterialDesc> materialDescs = mesh->materialDescs;
    vector<uint> materialIndices = mesh->materialIndices;

    uint numMaterials = mesh->subMeshCount;
    renderer->SetMaterialCount(numMaterials);

    for (uint i = 0; i < numMaterials; ++i)
    {
        tstring shaderName = tstring_format(mesh->path + TEXT(".Material") + TEXT("%d/%d"), i, techniqueIndex);
        ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderM(shader, shaderName);
        material->techniqueIndex = techniqueIndex;

        ResourceRef<Texture> albedoTexture;
        ResourceRef<Texture> normalMapTexture;
        ResourceRef<Texture> emissiveTexture;
        ResourceRef<Texture> shinessTexture;

        if (i < (uint)materialIndices.size())
        {
            const uint& materialIndex = materialIndices[i];
            const ModelMaterialDesc& materialDesc = materialDescs[materialIndex];

            if (materialDesc.HasDiffuse())
            {
                albedoTexture = system->resource->Find(materialDesc.diffuse);
            }
            if (materialDesc.HasShininess())
            {
                shinessTexture = system->resource->Find(materialDesc.shininess);
            }
            if (materialDesc.HasNormals())
            {
                normalMapTexture = system->resource->Find(materialDesc.normals);
            }
            if (materialDesc.HasEmission())
            {
                emissiveTexture = system->resource->Find(materialDesc.emission);
            }
        }

        if (albedoTexture)
        {
            material->SetTexture("_AlbedoTexture", albedoTexture);
        }
        if (shinessTexture)
        {
            material->SetTexture("_RoughnessTexture", shinessTexture);
        }
        if (shinessTexture)
        {
            material->SetTexture("_MetallicTexture", shinessTexture);
        }
        if (normalMapTexture)
        {
            material->SetTexture("_NormalMapTexture", normalMapTexture);
        }
        if (emissiveTexture)
        {
            material->SetTexture("_EmissiveTexture", emissiveTexture);
        }

        renderer->SetMaterialByIndex(i, material);
    }
}
