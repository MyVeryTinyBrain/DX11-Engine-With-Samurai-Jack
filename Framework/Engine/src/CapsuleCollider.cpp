#include "EnginePCH.h"
#include "CapsuleCollider.h"
#include "Transform.h"

#define CAPSULE_INIT_SLICE		16
#define CAPSULE_INIT_STEP		16
#define CAPSULE_SLICE			(CAPSULE_INIT_SLICE % 2 == 0 ? CAPSULE_INIT_SLICE + 1 : CAPSULE_INIT_SLICE)
#define CAPSULE_STEP			(CAPSULE_INIT_STEP * 2)

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
#include "PrimitiveVI.h"
#include "ResourceFactory.h"
#include "ICamera.h"

PxGeometryHolder CapsuleCollider::CreatePxGeometry(bool& out_invalid) const
{
	V3 absScale = V3::Abs(transform->lossyScale);
	float biggestElementOfXZ = absScale.x > absScale.z ? absScale.x : absScale.z;

	PxCapsuleGeometry capsule;

	capsule.radius = m_radius * biggestElementOfXZ;
	capsule.halfHeight = m_halfHeight * absScale.y;

	if (capsule.radius < Epsilon || capsule.halfHeight < Epsilon)
		out_invalid = true;

	return capsule;
}

void CapsuleCollider::ResetShape()
{
	Collider::ResetShape();

	if (debugRender)
	{
		ResetDebugShape();
	}
}

void CapsuleCollider::Awake()
{
	// PhysX의 캡슐은 기본적으로 누워 있습니다.
	// z축에서 90도 회전하면 서 있는 캡슐이 됩니다.
	// 여기서는 서 있는 캡슐을 기본으로 사용하겠습니다.
	m_defaultRotation = Q::Euler(0, 0, 90);

	Collider::Awake();

	if (debugRender)
	{
		CreateDebugShape();
	}
}

void CapsuleCollider::DebugRender()
{
	if (!isValid)
		return;

	V3 absScale = V3::Abs(transform->lossyScale);
	float biggestElementOfXZ = absScale.x > absScale.z ? absScale.x : absScale.z;
	V3 scale = V3(biggestElementOfXZ, absScale.y, biggestElementOfXZ);
	M4 localToWorldMatrix = M4::SRT(scale, transform->rotation, transform->position);

	if (!m_dbgMesh || !m_dbgMesh->isValid)
		return;

	if (!m_dbgMaterial || !m_dbgMaterial->isValid)
		return;

	uint passCount = m_dbgMaterial->GetPassCountOfAppliedTechnique();
	if (passCount == 0)
		return;

	RenderGroup renderGroup;
	int renderGroupOrder;
	bool instancingFlag;

	if (FAILED(m_dbgMaterial->GetRenderGroupOfAppliedTechnique(m_dbgMaterial->techniqueIndex, renderGroup)))
		return;
	if (FAILED(m_dbgMaterial->GetRenderGroupOrderOfAppliedTechnique(m_dbgMaterial->techniqueIndex, renderGroupOrder)))
		return;
	if (FAILED(m_dbgMaterial->GetInstancingFlagOfAppliedTechnique(m_dbgMaterial->techniqueIndex, instancingFlag)))
		return;

	RenderRequest input;
	input.essential.worldMatrix = localToWorldMatrix;
	input.essential.renderGroup = renderGroup;
	input.essential.renderGroupOrder = renderGroupOrder;
	input.essential.layerIndex = 0;
	input.essential.material = m_dbgMaterial;
	input.essential.techniqueIndex = m_dbgMaterial->techniqueIndex;
	input.essential.passIndex = 0;
	input.essential.mesh = m_dbgMesh;
	input.essential.subMeshIndex = 0;
	input.essential.instance = instancingFlag;

	system->graphicSystem->renderQueue->Add(input);
}

bool CapsuleCollider::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

void CapsuleCollider::OnDebugRenderModeChanged(bool value)
{
	if (value == true)
	{
		if (!m_dbgMesh)
			CreateDebugShape();

		ResetDebugShape();
	}
}

Bounds CapsuleCollider::GetBounds() const
{
	PxCapsuleGeometry capsule = GetPxGeometry().capsule();
	V3 extents = V3(capsule.radius, capsule.halfHeight, capsule.radius);

	Bounds bounds(transform->position, extents);
	bounds.Transform(M4::Rotate(transform->rotation));

	return bounds;
}

void CapsuleCollider::SetRadius(float value)
{
	value = Abs(value);

	if (m_radius == value)
		return;

	m_radius = value;

	ResetShape();
}

void CapsuleCollider::SetHalfHeight(float value)
{
	value = Abs(value);

	if (m_halfHeight == value)
		return;

	m_halfHeight = value;

	ResetShape();
}

void CapsuleCollider::CreateDebugShape()
{
	V3 absScale = V3::Abs(transform->lossyScale);
	float biggestElementOfXZ = absScale.x > absScale.z ? absScale.x : absScale.z;

	VI* vi = PrimitiveVI::CreateCapsule(m_radius * biggestElementOfXZ, m_halfHeight * absScale.y, CAPSULE_INIT_SLICE, CAPSULE_INIT_STEP);
	VIBuffer* viBuffer = nullptr;
	VIBuffer::CreateVIBufferNocopy(
		system->graphicSystem->device,
		system->graphicSystem->deviceContext,
		&vi,
		D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBuffer);
	m_dbgMesh = system->resourceManagement->factory->CreateUnamanagedMeshNocopy(&viBuffer);
	m_dbgMaterial = system->resourceManagement->builtInResources->wireframeMaterial;
}

void CapsuleCollider::ResetDebugShape()
{
	if (m_dbgMesh)
	{
		VIBuffer* viBuffer = m_dbgMesh->viBuffer;
		if (!viBuffer)
			return;

		VI* vi = viBuffer->GetVI();
		if (!vi)
			return;

		Vertex* vertices = vi->GetVertices();

		{
			V3 absScale = V3::Abs(transform->lossyScale);
			float biggestElementOfXZ = absScale.x > absScale.z ? absScale.x : absScale.z;
			float capsuleHalfHeight = m_halfHeight * absScale.y;
			float capsuleRadius = m_radius * biggestElementOfXZ;

			SphericalYaw spherical;
			spherical.Radius = capsuleRadius * biggestElementOfXZ;

			uint vIndex = 0;
			for (uint i = 1; i <= CAPSULE_STEP; ++i)
			{
				uint k = i;
				if (i > CAPSULE_STEP / 2)
					k -= 1;

				float heightPercent = float(k) / float(CAPSULE_STEP);
				spherical.PhiAngle = 180.0f * heightPercent;

				for (uint j = 0; j < CAPSULE_SLICE; ++j)
				{
					float spinPercent = (float(j) / float(CAPSULE_SLICE - 1));
					spherical.ThetaAngle = 360.0f * spinPercent;

					V3 cartesian = spherical.cartesian;

					vertices[vIndex].position = cartesian;

					if (i <= CAPSULE_STEP / 2)
						vertices[vIndex].position += V3::up() * capsuleHalfHeight;
					else
						vertices[vIndex].position += V3::down() * capsuleHalfHeight;

					float v = 1 - (vertices[vIndex].position.y + capsuleHalfHeight + capsuleRadius) / (2.0f * (capsuleHalfHeight + capsuleRadius));

					vertices[vIndex].uvw = V2(spinPercent, v);
					vertices[vIndex].normal = cartesian.normalized;
					++vIndex;
				}
			}

			uint topVertexIndex = vIndex;
			uint bottomVertexIndex = vIndex + 1;

			vertices[topVertexIndex].position = V3::up() * (capsuleRadius + capsuleHalfHeight);
			vertices[topVertexIndex].uvw = V2(0, 0);
			vertices[topVertexIndex].normal = V3(0, 1, 0);

			vertices[bottomVertexIndex].position = V3::down() * (capsuleRadius + capsuleHalfHeight);
			vertices[bottomVertexIndex].uvw = V2(0, 1);
			vertices[bottomVertexIndex].normal = V3(0, -1, 0);
		}

		viBuffer->UpdateVertexBuffer();
	}
}
