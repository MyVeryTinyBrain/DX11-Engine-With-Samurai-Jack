#include "EnginePCH.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "RenderRequestInput.h"
#include "System.h"
#include "GraphicSystem.h"
#include "RenderQueue.h"
#include "BuiltInResources.h"
#include "MaterialStandard.h"
#include "ResourceFactory.h"
#include "Texture2D.h"

void MeshRenderer::Render()
{
	if (!isValid)
		return;

	M4 localToWorldMatrix = transform->localToWorldMatrix;

	for (uint i = 0; i < m_mesh->GetSubMeshCount() && i < m_materials.size(); ++i)
	{
		if (!m_mesh || !m_mesh->isValid)
			continue;

		ResourceRef<Material>& currentMaterial = m_materials[i];

		if (!currentMaterial || !currentMaterial->isValid)
			continue;

		size_t passCount = currentMaterial->GetPassCountOfAppliedTechnique();

		for (uint j = 0; j < passCount; ++j)
		{
			RenderGroup renderGroup;
			int renderGroupOrder;
			bool instancingFlag;
			bool drawShadowFlag;
			bool shadowCutoffEnableFlag;
			float shadowCutoffAlpha;

			if (FAILED(currentMaterial->GetRenderGroupOfAppliedTechnique(j, renderGroup)))
				continue;
			if (FAILED(currentMaterial->GetRenderGroupOrderOfAppliedTechnique(j, renderGroupOrder)))
				continue;
			if (FAILED(currentMaterial->GetInstancingFlagOfAppliedTechnique(j, instancingFlag)))
				continue;
			if (FAILED(currentMaterial->GetDrawShadowFlagOfAppliedTechnique(j, drawShadowFlag)))
				continue;
			if (FAILED(currentMaterial->GetShadowCutoffEnableFlagOfAppliedTechnique(j, shadowCutoffEnableFlag)))
				continue;
			if (FAILED(currentMaterial->GetShadowCutoffAlphaOfAppliedTechnique(j, shadowCutoffAlpha)))
				continue;

			RenderRequest input;
			input.essential.worldMatrix = localToWorldMatrix;
			input.essential.renderGroup = renderGroup;
			input.essential.renderGroupOrder = renderGroupOrder;
			input.essential.layerIndex = m_layerIndex;
			input.essential.material = currentMaterial;
			input.essential.techniqueIndex = m_techniqueIndex;
			input.essential.passIndex = j;
			input.essential.mesh = m_mesh;
			input.essential.subMeshIndex = i;
			input.essential.instance = instancingFlag;

			RenderRequestShadow shadow = {};
			shadow.draw = drawShadowFlag;
			shadow.cutoffEnable = shadowCutoffEnableFlag;
			shadow.cutoffAlpha = shadowCutoffAlpha;
			shadow.cutoffTexture = currentMaterial->diffuseTexture;
			switch (renderGroup)
			{
				case Engine::RenderGroup::Priority:
				case Engine::RenderGroup::Transparent:
				case Engine::RenderGroup::Overlay:
					shadow.draw = false;
					break;
			}
			input.shadow = shadow;

			input.op.boneOp = nullptr;
			input.op.cullOp = this;
			input.op.boundsOp = this;

			system->graphicSystem->renderQueue->Add(input);
		}
	}
}

bool MeshRenderer::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds MeshRenderer::GetBounds() const
{
	if (!m_mesh)
		return Bounds(transform->position, V3::zero());

	Bounds bounds = m_mesh->GetBounds();
	bounds.Transform(transform->localToWorldMatrix);
	return bounds;
}

bool MeshRenderer::IsValid() const
{
	if (!Renderer::IsValid())
		return false;

	return true;
}

void MeshRenderer::SetMesh(const ResourceRef<Mesh>& mesh)
{
	if (name == TEXT("KatanaRenderer"))
		int i = 0;

	SetupMaterialsToDefault(mesh);

	m_mesh = mesh;
}

void MeshRenderer::SetupMaterialsToDefault(const ResourceRef<Mesh>& mesh)
{
	if (mesh == m_mesh)
		return;

	if (!mesh)
	{
		m_materials.clear();
		return;
	}

	const vector<ModelMaterialDesc>& materials = mesh->materialDescs;
	const vector<uint>& materialIndices = mesh->materialIndices;
	SetMaterialCount(mesh->subMeshCount);

	for (uint i = 0; i < mesh->subMeshCount; ++i)
	{
		if (m_materials[i])
			continue;

		tstring texturePath;
		ResourceRef<Texture2D> texture;

		if (i < materialIndices.size())
		{
			texturePath = materials[materialIndices[i]].diffuse;
			texture = system->resourceManagement->Find(texturePath);
		}

		if (!texture)
		{
			SetMaterialByIndex(i, system->resourceManagement->builtInResources->standardMaterial);
		}
		else
		{
			tstring materialPath = texturePath + tstring(TEXT(".material"));
			ResourceRef<Material> material = system->resourceManagement->factory->CreateManagedMaterial<MaterialStandard>(materialPath);
			material->diffuseTexture = texture;
			SetMaterialByIndex(i, material);
		}
	}
}
