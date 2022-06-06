#include "EnginePCH.h"
#include "BillboardRenderer.h"
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

void BillboardRenderer::Awake()
{
	SetMesh(system->resource->builtIn->quadMesh);
}

void BillboardRenderer::Render()
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

			M4 localMatrix = M4::SRT(V3::one(), V3::zero(), transform->localPosition);
			M4 parentMatrix = transform->parent ? transform->parent->localToWorldMatrix : M4::identity();
			input.essential.worldMatrix = localMatrix * parentMatrix;
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
			memcpy(input.essential.instanceData, GetInstanceDataArray(), sizeof(V4) * 4);

			RenderRequestShadow shadow = {};
			shadow.draw = drawShadowFlag;
			shadow.shadowPass = shadowPassFlag;
			input.shadow = shadow;

			input.op.boneOp = nullptr;
			input.op.cullOp = this;
			input.op.boundsOp = this;
			input.op.onCameraOp = this;

			system->graphic->renderQueue->Add(input);
		}
	}
}

void BillboardRenderer::OnCamera(ICamera* camera, RenderRequest* inout_prequest)
{
	Q camRotation = camera->GetRotation();

	V3 camEulerAngles = camRotation.eulerAngles;
	if (IsLock(BillboardRenderer::LockFlag::X))
		camEulerAngles.x = 0;
	if (IsLock(BillboardRenderer::LockFlag::Y))
		camEulerAngles.y = 0;
	if (IsLock(BillboardRenderer::LockFlag::Z))
		camEulerAngles.z = 0;

	V3 localEulerAngles = transform->localEulerAngles;
	if (IsSelfLock(BillboardRenderer::LockFlag::X))
		localEulerAngles.x = 0;
	if (IsSelfLock(BillboardRenderer::LockFlag::Y))
		localEulerAngles.y = 0;
	if (IsSelfLock(BillboardRenderer::LockFlag::Z))
		localEulerAngles.z = 0;

	M4 scaleMatrix = M4::Scale(transform->localScale);
	M4 rotateMatrix = M4::Rotate(camEulerAngles + localEulerAngles);
	M4 billboardWorldMatrix = scaleMatrix * rotateMatrix * inout_prequest->essential.worldMatrix;
	inout_prequest->essential.worldMatrix = billboardWorldMatrix;
}

void BillboardRenderer::SetLockFlags(BillboardRenderer::LockFlags lockFlags)
{
	m_lockFlags = lockFlags;
}

void BillboardRenderer::SetLock(BillboardRenderer::LockFlag lockFlag, bool value)
{
	if (value)
	{
		m_lockFlags |= lockFlag;
	}
	else
	{
		m_lockFlags &= (~lockFlag);
	}
}

BillboardRenderer::LockFlags BillboardRenderer::GetLockFlags() const
{
	return m_lockFlags;
}

bool BillboardRenderer::IsLock(BillboardRenderer::LockFlag lockFlag) const
{
	return (m_lockFlags & lockFlag);
}

void BillboardRenderer::SetSelfLockFlags(BillboardRenderer::LockFlags lockFlags)
{
	m_selfRotateLockFlags = lockFlags;
}

void BillboardRenderer::SetSelfLock(BillboardRenderer::LockFlag lockFlag, bool value)
{
	if (value)
	{
		m_selfRotateLockFlags |= lockFlag;
	}
	else
	{
		m_selfRotateLockFlags &= (~lockFlag);
	}
}

BillboardRenderer::LockFlags BillboardRenderer::GetSelfLockFlags() const
{
	return m_selfRotateLockFlags;
}

bool BillboardRenderer::IsSelfLock(BillboardRenderer::LockFlag lockFlag) const
{
	return (m_selfRotateLockFlags & lockFlag);
}

bool BillboardRenderer::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds BillboardRenderer::GetBounds() const
{
	if (!m_mesh)
		return Bounds(transform->position, V3::zero());

	Bounds bounds = m_mesh->GetBounds();
	bounds.Transform(transform->localToWorldMatrix);
	return bounds;
}

bool BillboardRenderer::IsValid() const
{
	if (!Renderer::IsValid())
		return false;

	return true;
}

void BillboardRenderer::SetMesh(const ResourceRef<Mesh>& mesh)
{
	if (m_mesh == mesh)
		return;

	ResourceRef<Mesh> before = m_mesh;
	m_mesh = mesh;
	OnMeshChanged(before, mesh);

	ClearMaterials();

	if (!m_mesh)
	{
		return;
	}

	uint numMaterials = m_mesh->subMeshCount;
	SetMaterialCount(numMaterials);

	for (uint i = 0; i < numMaterials; ++i)
	{
		SetMaterialByIndex(i, system->resource->builtIn->whiteUnlitMaterial);
	}
}
