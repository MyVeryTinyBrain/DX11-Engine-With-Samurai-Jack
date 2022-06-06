#include "EnginePCH.h"
#include "CapsuleCollider.h"
#include "Transform.h"

#define CAPSULE_NUM_V		8
#define CAPSULE_NUM_CAP_H	8

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
#include "VIUtility.h"
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

void CapsuleCollider::Awake(void* arg)
{
	m_defaultRotation = Q::Euler(0, 0, 90);

	Collider::Awake(arg);

	if (debugRender)
	{
		CreateDebugShape();
	}
}

void CapsuleCollider::DebugRender()
{
	if (!isValid)
		return;

	M4 localToWorldMatrix = GetDebugRenderWorldMatrix();

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

	VI* vi = VIUtility::CreateCapsule(m_radius * biggestElementOfXZ, m_halfHeight * absScale.y, CAPSULE_NUM_V, CAPSULE_NUM_CAP_H);
	VIBuffer* viBuffer = nullptr;
	VIBuffer::CreateVIBufferNocopy(
		system->graphic->device,
		system->graphic->deviceContext,
		&vi,
		D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBuffer);
	m_dbgMesh = system->resource->factory->CreateMeshNocopyUM(&viBuffer);
}

void CapsuleCollider::ResetDebugShape()
{
	if (m_dbgMesh)
	{
		VIBuffer* viBuffer = m_dbgMesh->viBuffer;
		if (!viBuffer)
			return;

		VI* vi = viBuffer->GetVI();
		if (!viBuffer)
			return;

		Vertex* vertices = vi->GetVertices();
		if (!vertices)
			return;

		uint numV = CAPSULE_NUM_V;
		uint numCapH = CAPSULE_NUM_CAP_H;

		if (numV < 3)
			numV = 3;

		uint numH = numCapH * 2;

		if (numH < 2)
			numH = 2;

		uint numSpin = numV + 1;
		uint numVertices = numSpin * numH + numSpin * 2;
		uint numTriangles = numSpin * (numH + 1) * 2;

		// Setup vertices =====================================================================

		SphericalYaw spherical;
		spherical.Radius = radius;

		uint vertexIndex = 0;
		for (uint i = 1; i <= numH; ++i)
		{
			float heightPercent = float(i) / float(numH);
			spherical.PhiAngle = 180.0f * heightPercent;

			for (uint j = 0; j < numSpin; ++j)
			{
				float spinPercent = (float(j) / float(numV));
				spherical.ThetaAngle = 360.0f * spinPercent;
				V3 cartesian = spherical.cartesian;
				float direction = (i <= numH / 2) ? (+1.0f) : (-1.0f);

				vertices[vertexIndex].position = spherical.cartesian + (V3::up() * halfHeight * direction);

				float normalizedHeight = (vertices[vertexIndex].position.y + halfHeight + radius) / (2.0f * (halfHeight + radius));

				vertices[vertexIndex].uvw = V2(spinPercent, 1.0f - normalizedHeight);
				vertices[vertexIndex].normal = cartesian.normalized;

				++vertexIndex;
			}
		}

		uint topVertexBeginIndex = vertexIndex;
		uint bottomVertexBeginIndex = vertexIndex + numSpin;

		for (uint i = 0; i < numSpin; ++i)
		{
			float spinPercent = (float(i) / float(numV));

			vertices[topVertexBeginIndex + i].position = V3::up() * (radius + halfHeight);
			vertices[topVertexBeginIndex + i].uvw = V2(spinPercent, 0.0f);
			vertices[topVertexBeginIndex + i].normal = V3::up();

			vertices[bottomVertexBeginIndex + i].position = V3::down() * (radius + halfHeight);
			vertices[bottomVertexBeginIndex + i].uvw = V2(spinPercent, 1.0f);
			vertices[bottomVertexBeginIndex + i].normal = V3::down();

			vertexIndex += 2;
		}

		// =====================================================================================

		viBuffer->UpdateVertexBuffer();
	}
}

ResourceRef<Mesh> CapsuleCollider::GetDebugMesh() const
{
	return m_dbgMesh;
}

M4 CapsuleCollider::GetDebugRenderWorldMatrix() const
{
	V3 absScale = V3::Abs(transform->lossyScale);
	float biggestElementOfXZ = absScale.x > absScale.z ? absScale.x : absScale.z;
	V3 scale = V3(biggestElementOfXZ, absScale.y, biggestElementOfXZ);
	return M4::SRT(scale, transform->rotation, transform->position);
}
