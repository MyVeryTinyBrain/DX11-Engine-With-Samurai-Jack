#include "EnginePCH.h"
#include "ScreenRenderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Transform.h"
#include "RenderRequestInput.h"
#include "System.h"
#include "GraphicSystem.h"
#include "RenderQueue.h"
#include "ResourceManagement.h"
#include "BuiltInResources.h"
#include "ResourceFactory.h"
#include "Texture2D.h"

void ScreenRenderer::Awake()
{
	SetMesh(system->resource->builtIn->screenMesh);

	ResourceRef<Shader> shader = system->resource->builtIn->screenShader;
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	SetMaterial(material);
}

void ScreenRenderer::Render()
{
	if (!isValid)
		return;

	for (uint i = 0; i < m_mesh->GetSubMeshCount() && i < m_materials.size(); ++i)
	{
		if (!m_mesh || !m_mesh->isValid)
			continue;

		ResourceRef<Material>& currentMaterial = m_materials[i];

		if (!currentMaterial || !currentMaterial->isValid)
			continue;

		uint passCount = currentMaterial->GetPassCountOfAppliedTechnique();

		for (uint j = 0; j < passCount; ++j)
		{
			RenderGroup renderGroup;
			int renderGroupOrder;
			bool cullingFlag;
			bool instancingFlag;
			bool drawShadowFlag;
			bool shadowPassFlag;

			if (FAILED(currentMaterial->GetRenderGroupOfAppliedTechnique(j, renderGroup))) continue;
			if (FAILED(currentMaterial->GetRenderGroupOrderOfAppliedTechnique(j, renderGroupOrder))) continue;
			if (FAILED(currentMaterial->GetCullingFlagOfAppliedTechnique(j, cullingFlag))) continue;
			if (FAILED(currentMaterial->GetInstancingFlagOfAppliedTechnique(j, instancingFlag))) continue;
			if (FAILED(currentMaterial->GetDrawShadowFlagOfAppliedTechnique(j, drawShadowFlag))) continue;
			if (FAILED(currentMaterial->GetShadowPassFlagOfAppliedTechnique(j, shadowPassFlag))) continue;

			RenderRequest input = {};
			input.essential.worldMatrix = transform->localToWorldMatrix;
			input.essential.renderGroup = renderGroup;
			input.essential.renderGroupOrder = m_renderGroupOrder;
			input.essential.layerIndex = m_layerIndex;
			input.essential.material = currentMaterial;
			input.essential.techniqueIndex = currentMaterial->techniqueIndex;
			input.essential.passIndex = j;
			input.essential.mesh = m_mesh;
			input.essential.subMeshIndex = i;
			input.essential.cull = cullingFlag;
			input.essential.instance = instancingFlag;
			memcpy(input.essential.instanceData, GetInstanceDataArray(), sizeof(Real4) * 4);

			RenderRequestShadow shadow = {};
			shadow.draw = drawShadowFlag;
			shadow.shadowPass = shadowPassFlag;
			input.shadow = shadow;

			input.op.boneOp = nullptr;
			input.op.cullOp = nullptr;
			input.op.boundsOp = this;

			system->graphic->renderQueue->Add(input);
		}
	}
}

Bounds ScreenRenderer::GetBounds() const
{
	return Bounds(V3::zero(), V3::zero());
}

bool ScreenRenderer::IsValid() const
{
	if (!Renderer::IsValid())
		return false;

	return true;
}

void ScreenRenderer::SetMesh(const ResourceRef<Mesh>& mesh)
{
	if (m_mesh == mesh)
		return;

	ResourceRef<Mesh> before = m_mesh;
	m_mesh = mesh;
	OnMeshChanged(before, mesh);
}
