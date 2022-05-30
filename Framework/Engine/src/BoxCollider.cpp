#include "EnginePCH.h"
#include "BoxCollider.h"
#include "Transform.h"

// Debug Render
#include "System.h"
#include "ResourceManagement.h"
#include "BuiltInResources.h"
#include "ResourceRef.h"
#include "Mesh.h"
#include "VIBuffer.h"
#include "VI.h"
#include "Material.h"
#include "GraphicSystem.h"
#include "RenderQueue.h"
#include "ICamera.h"

PxGeometryHolder BoxCollider::CreatePxGeometry(bool& out_invalid) const
{
    V3 extentsWithScale = V3::Scale(m_extents, V3::Abs(transform->lossyScale));

    PxBoxGeometry box;

    box.halfExtents = ToPxVec3(extentsWithScale);

	if (V3::MinAbsElement(extentsWithScale) < Epsilon)
		out_invalid = true;

    return box;
}

void BoxCollider::Awake()
{
	Collider::Awake();

	m_dbgMesh = system->resource->builtIn->boxMesh;
	m_dbgMaterial = system->resource->builtIn->greenWireframeMaterial;
}

void BoxCollider::DebugRender()
{
	if (!isValid)
		return;

	V3 extentsWithScale = V3::Scale(m_extents, V3::Abs(transform->lossyScale)) * 2.0f;
	M4 localToWorldMatrix = M4::SRT(extentsWithScale, transform->rotation, transform->position);
	
	if (!m_dbgMesh || !m_dbgMesh->isValid)
		return;

	if (!m_dbgMaterial || !m_dbgMaterial->isValid)
		return;

	uint passCount = m_dbgMaterial->GetPassCountOfAppliedTechnique();
	if (passCount == 0)
		return;

	for (uint j = 0; j < passCount; ++j)
	{
		RenderGroup renderGroup;
		int renderGroupOrder;
		bool instancingFlag;

		if (FAILED(m_dbgMaterial->GetRenderGroupOfAppliedTechnique(j, renderGroup)))
			return;
		if (FAILED(m_dbgMaterial->GetRenderGroupOrderOfAppliedTechnique(j, renderGroupOrder)))
			return;
		if (FAILED(m_dbgMaterial->GetInstancingFlagOfAppliedTechnique(j, instancingFlag)))
			return;

		RenderRequest input;
		input.essential.worldMatrix = localToWorldMatrix;
		input.essential.renderGroup = renderGroup;
		input.essential.renderGroupOrder = renderGroupOrder;
		input.essential.layerIndex = 0;
		input.essential.material = m_dbgMaterial;
		input.essential.techniqueIndex = m_dbgMaterial->techniqueIndex;
		input.essential.passIndex = j;
		input.essential.mesh = m_dbgMesh;
		input.essential.subMeshIndex = 0;
		input.essential.instance = instancingFlag;

		input.op.cullOp = this;

		system->graphic->renderQueue->Add(input);
	}
}

bool BoxCollider::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds BoxCollider::GetBounds() const
{
	PxVec3 pxBoxExtents = GetPxGeometry().box().halfExtents;
	V3 boxExtents = FromPxVec3(pxBoxExtents);

	Bounds bounds(transform->position, boxExtents);
	bounds.Transform(M4::Rotate(transform->rotation));

	return bounds;
}

void BoxCollider::SetExtents(const V3& extents)
{
    V3 absExtents = V3::Abs(extents);

    if (absExtents == m_extents)
        return;

    m_extents = absExtents;

    ResetShape();
}
