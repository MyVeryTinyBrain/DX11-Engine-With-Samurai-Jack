#include "EnginePCH.h"
#include "MeshRenderer.h"
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

void MeshRenderer::Render()
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
			input.essential.renderGroupOrder = renderGroupOrder;
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
			input.op.cullOp = this;
			input.op.boundsOp = this;

			system->graphic->renderQueue->Add(input);
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
	// 메쉬의 로컬 바운딩 박스를 반환받습니다.
	Bounds bounds = m_mesh->GetBounds();
	// 월드 바운딩 박스로 변환해 반환합니다.
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
	if (m_mesh == mesh)
		return;

	ResourceRef<Mesh> before = m_mesh;
	m_mesh = mesh;
	OnMeshChanged(before, mesh);

	SetupDefaultMaterials();
}
