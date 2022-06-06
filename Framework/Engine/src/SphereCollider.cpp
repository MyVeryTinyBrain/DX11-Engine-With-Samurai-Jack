#include "EnginePCH.h"
#include "SphereCollider.h"
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

PxGeometryHolder SphereCollider::CreatePxGeometry(bool& out_invalid) const
{
	V3 scale = transform->lossyScale;

	PxSphereGeometry sphere;

	sphere.radius = m_radius * V3::MaxAbsElement(scale);

	if (sphere.radius < Epsilon)
		out_invalid = true;

	return sphere;
}

void SphereCollider::Awake()
{
	Collider::Awake();

	m_dbgMesh = system->resource->builtIn->sphereMesh;
}

void SphereCollider::DebugRender()
{
	if (!isValid)
		return;

	const M4& localToWorldMatrix = GetDebugRenderWorldMatrix();

	if (!m_dbgMesh || !m_dbgMesh->isValid)
		return;

	ResourceRef<Material> material;
	if (!isTrigger) material = system->resource->builtIn->greenWireframeMaterial;
	else  material = system->resource->builtIn->blueWireframeMaterial;

	if (!material || !material->isValid)
		return;

	uint passCount = material->GetPassCountOfAppliedTechnique();
	if (passCount == 0)
		return;

	for (uint j = 0; j < passCount; ++j)
	{
		RenderGroup renderGroup;
		int renderGroupOrder;
		bool instancingFlag;

		if (FAILED(material->GetRenderGroupOfAppliedTechnique(j, renderGroup)))
			return;
		if (FAILED(material->GetRenderGroupOrderOfAppliedTechnique(j, renderGroupOrder)))
			return;
		if (FAILED(material->GetInstancingFlagOfAppliedTechnique(j, instancingFlag)))
			return;

		RenderRequest input;
		input.essential.worldMatrix = localToWorldMatrix;
		input.essential.renderGroup = renderGroup;
		input.essential.renderGroupOrder = renderGroupOrder;
		input.essential.layerIndex = 0;
		input.essential.material = material;
		input.essential.techniqueIndex = material->techniqueIndex;
		input.essential.passIndex = j;
		input.essential.mesh = m_dbgMesh;
		input.essential.subMeshIndex = 0;
		input.essential.instance = instancingFlag;

		system->graphic->renderQueue->Add(input);
	}
}

bool SphereCollider::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds SphereCollider::GetBounds() const
{
	PxSphereGeometry sphere = GetPxGeometry().sphere();
	V3 extents = V3(sphere.radius, sphere.radius, sphere.radius);

	Bounds bounds(transform->position, extents);
	bounds.Transform(M4::Rotate(transform->rotation));

	return bounds;
}

void SphereCollider::SetRadius(float value)
{
	value = Abs(value);

	if (m_radius == value)
		return;

	m_radius = value;

	ResetShape();
}

ResourceRef<Mesh> SphereCollider::GetDebugMesh() const
{
	return m_dbgMesh;
}

M4 SphereCollider::GetDebugRenderWorldMatrix() const
{
	V3 scale = m_radius * V3::MaxAbsElement(transform->lossyScale) * V3::one() * 2.0f;
	return M4::SRT(scale, transform->rotation, transform->position);
}
