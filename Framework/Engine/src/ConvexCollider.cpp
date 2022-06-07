#include "EnginePCH.h"
#include "ConvexCollider.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "VIBuffer.h"
#include "VI.h"
#include "System.h"
#include "PhysicsSystem.h"
#include "GraphicSystem.h"
#include "ResourceManagement.h"
#include "ResourceFactory.h"
#include "Material.h"
#include "RenderQueue.h"

PxGeometryHolder ConvexCollider::CreatePxGeometry(bool& out_invalid) const
{
	if (!m_pxConvexMesh)
	{
		out_invalid = true;
		return PxGeometryHolder();
	}

	//Q worldRotation = transform->rotation;
	V3 lossyScale = V3::Abs(transform->lossyScale);

	PxMeshScale scale;
	scale.rotation = PxIdentity;
	scale.scale = ToPxVec3(lossyScale);

	return PxConvexMeshGeometry(m_pxConvexMesh, scale);
}

void ConvexCollider::Awake()
{
	Collider::Awake();

	FindMeshRenderer();
}

void ConvexCollider::Start()
{
	Collider::Start();

	FindMeshRenderer();
}

void ConvexCollider::Update()
{
	Collider::Update();

	FindMeshRenderer();
}

void ConvexCollider::OnDestroyed()
{
	Collider::OnDestroyed();

	if (m_meshRenderer)
	{
		m_meshRenderer->OnDestroyedCallback -= func<void(SceneObject*)>(this, &ConvexCollider::OnMeshRendererDestroyed);
		m_meshRenderer->OnMeshChanged -= func<void(ResourceRef<Mesh>, ResourceRef<Mesh>)>(this, &ConvexCollider::OnMeshChanged);
		m_meshRenderer = nullptr;
	}
}

void ConvexCollider::DebugRender()
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

void ConvexCollider::UpdatePxConvexMesh()
{
	PxRelease(m_pxConvexMesh);

	if (!m_meshRenderer) return;
	if (m_meshRenderer && !m_meshRenderer->mesh) return;

	ResourceRef<Mesh> mesh = m_meshRenderer->mesh;
	VIBuffer* vibuffer = mesh->GetVIBuffer();
	VI* vi = vibuffer->GetVI();
	if (!vi) return;
	if (!vi->IsValid()) return;

	Vertex* vertices = vi->GetVertices();
	uint numVertices = vi->GetVertexCount();
	PxVec3* inputVertices = new PxVec3[numVertices];
	for (uint i = 0; i < numVertices; ++i)
		inputVertices[i] = ToPxVec3(vertices[i].position);

	uint numSubIndices = vi->GetSubIndicesCount();
	uint numIndices = vi->GetIndexCount();
	Index* inputIndices = new Index[numIndices];
	uint indexCursor = 0;
	for (uint i = 0; i < numSubIndices; ++i)
	{
		SubIndicesDesc* subIndices = vi->GetSubIndicesDesc(i);
		for (uint j = 0; j < subIndices->Count; ++j)
		{
			inputIndices[indexCursor++] = subIndices->SubIncides[j];
		}
	}

	PxConvexMeshDesc desc;
	desc.points.count = numVertices;
	desc.points.stride = sizeof(PxVec3);
	desc.points.data = inputVertices;
	desc.indices.count = vi->GetIndexCount();
	desc.indices.stride = sizeof(Index);
	desc.indices.data = inputIndices;
	desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	IPhysicsSystem* iPhysicsSystem = system->physics;
	PxPhysics* physics = iPhysicsSystem->GetPhysics();
	PxCooking* cooking = iPhysicsSystem->GetCooking();
	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;
	bool cookingTriangleStatus = cooking->cookConvexMesh(desc, writeBuffer, &result);
	assert(cookingTriangleStatus);

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	m_pxConvexMesh = physics->createConvexMesh(readBuffer);

	SafeDeleteArray(inputVertices);
	SafeDeleteArray(inputIndices);

	if (m_pxConvexMesh)
	{
		uint numOutputVertices = m_pxConvexMesh->getNbVertices();
		uint numOutputIndices = m_pxConvexMesh->getNbPolygons() * 3;
		const PxVec3* pxVertices = m_pxConvexMesh->getVertices();
		const PxU8* pxIndices = m_pxConvexMesh->getIndexBuffer();
		Vertex* outputVertices = new Vertex[numOutputVertices];
		Index* outputIndices = new Index[numOutputIndices];

		for (uint i = 0; i < numOutputVertices; ++i)
		{
			outputVertices[i].position = FromPxVec3(pxVertices[i]);
		}
		for (uint i = 0; i < numOutputIndices; ++i)
		{
			outputIndices[i] = pxIndices[i];
		}

		VI* vi = new VI;
		vi->SetVerticesNocopy(&outputVertices, numOutputVertices);
		vi->SetIndicesNocopy(&outputIndices, numOutputIndices);
		vi->RecalculateBounds();

		VIBuffer* viBuffer = nullptr;
		VIBuffer::CreateVIBufferNocopy(
			system->graphic->device,
			system->graphic->deviceContext,
			&vi,
			D3D11_USAGE_IMMUTABLE, 0, 0,
			D3D11_USAGE_IMMUTABLE, 0, 0,
			&viBuffer);
		m_dbgMesh = system->resource->factory->CreateMeshNocopyUM(&viBuffer);
	}
}

void ConvexCollider::FindMeshRenderer()
{
	if (!m_meshRenderer)
	{
		m_meshRenderer = gameObject->GetComponent<MeshRenderer>();
		
		if (m_meshRenderer)
		{
			m_meshRenderer->OnDestroyedCallback += func<void(SceneObject*)>(this, &ConvexCollider::OnMeshRendererDestroyed);
			m_meshRenderer->OnMeshChanged += func<void(ResourceRef<Mesh>, ResourceRef<Mesh>)>(this, &ConvexCollider::OnMeshChanged);

			UpdatePxConvexMesh();
			ResetShape();
		}
	}
}

void ConvexCollider::OnMeshRendererDestroyed(SceneObject* meshRenderer)
{
	m_meshRenderer->OnDestroyedCallback -= func<void(SceneObject*)>(this, &ConvexCollider::OnMeshRendererDestroyed);
	m_meshRenderer->OnMeshChanged -= func<void(ResourceRef<Mesh>, ResourceRef<Mesh>)>(this, &ConvexCollider::OnMeshChanged);
	m_meshRenderer = nullptr;

	UpdatePxConvexMesh();
	ResetShape();
}

void ConvexCollider::OnMeshChanged(ResourceRef<Mesh> before, ResourceRef<Mesh> after)
{
	UpdatePxConvexMesh();
	ResetShape();
}

Bounds ConvexCollider::GetBounds() const
{
	if (!m_meshRenderer)
		return Bounds(V3::zero(), V3::zero());

	return m_meshRenderer->GetBounds();
}

ResourceRef<Mesh> ConvexCollider::GetDebugMesh() const
{
	return m_dbgMesh;
}

M4 ConvexCollider::GetDebugRenderWorldMatrix() const
{
	V3 absScale = V3::Abs(transform->lossyScale);
	return M4::SRT(absScale, transform->rotation, transform->position);
}
