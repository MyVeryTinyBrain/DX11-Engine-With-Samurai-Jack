#include "EnginePCH.h"
#include "LineRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include "System.h"
#include "VI.h"
#include "VIBuffer.h"
#include "GraphicSystem.h"
#include "ResourceManagement.h"
#include "CameraManager.h"
#include "Material.h"
#include "RenderQueue.h"

void LineRenderer::Awake()
{
	Renderer::Awake();

	SetupMesh(512);

	SetMaterial(system->resource->builtInResources->whiteMaterial);
}

void LineRenderer::Render()
{
	if (!isValid)
		return;

	M4 localToWorldMatrix = transform->localToWorldMatrix;

	if (m_materials.empty())
		return;

	ApplyVertices();

	if (!m_mesh || !m_mesh->isValid)
		return;

	ResourceRef<Material>& currentMaterial = m_materials[0];

	if (!currentMaterial || !currentMaterial->isValid)
		return;

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
		if (m_localSpace)
		{
			input.essential.worldMatrix = transform->localToWorldMatrix;
		}
		else
		{
			input.essential.worldMatrix = M4::identity();
		}
		input.essential.renderGroup = renderGroup;
		input.essential.renderGroupOrder = renderGroupOrder;
		input.essential.layerIndex = m_layerIndex;
		input.essential.material = currentMaterial;
		input.essential.techniqueIndex = currentMaterial->techniqueIndex;
		input.essential.passIndex = j;
		input.essential.mesh = m_mesh;
		input.essential.subMeshIndex = 0;
		input.essential.cull = cullingFlag;
		input.essential.instance = instancingFlag;

		input.customPrimitiveCount.usePrimitiveCount = true;
		input.customPrimitiveCount.primitiveCount = uint(m_points.size() - 1) * 2;

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

inline void LineRenderer::AddPoint(const V3& point)
{
	if (m_numRect <= m_points.size())
	{
		FitNumRect();
	}

	m_points.push_back(point);
}

bool LineRenderer::RemovePoint(uint index)
{
	if (index >= m_points.size())
		return false;

	auto it = m_points.begin() + index;
	m_points.erase(it);

	if (m_points.size() < m_numRect / 2)
		FitNumRect();

	return true;
}

void LineRenderer::ClearPoints()
{
	size_t prevSize = m_points.size();
	m_points.clear();

	if (prevSize != m_points.size())
		FitNumRect();
}

bool LineRenderer::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds LineRenderer::GetBounds() const
{
	if (!m_mesh)
		return Bounds(transform->position, V3::zero());

	if (m_localSpace)
	{
		Bounds bounds = m_mesh->GetBounds();
		bounds.Transform(transform->localToWorldMatrix);
		return bounds;
	}
	else
	{
		return m_mesh->GetBounds();
	}
}

bool LineRenderer::IsValid() const
{
	if (!Renderer::IsValid())
		return false;

	return m_points.size() >= 2;
}

void LineRenderer::FitNumRect()
{
	SetupMesh(uint(m_points.size()) * 2);
}

void LineRenderer::SetupMesh(uint numRect)
{
	if (numRect == m_numRect)
		return;

	VI* vi = !m_mesh ? new VI : m_mesh->viBuffer->GetVI();

	uint numVertices = numRect * 2 + 2;
	Vertex* vertices = new Vertex[numVertices]{};
	vi->SetVerticesNocopy(&vertices, numVertices);

	uint numTriangles = numRect * 2;
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};
	for (uint i = 0; i < numRect; ++i)
	{
		uint indexBegin = i * 2;
		uint triangle[2] = { indexBegin, indexBegin + 1 };
		uint vertex[4] = { indexBegin, indexBegin + 1, indexBegin + 2, indexBegin + 3 };

		triangles[triangle[0]]._0 = vertex[0];
		triangles[triangle[0]]._1 = vertex[2];
		triangles[triangle[0]]._2 = vertex[3];

		triangles[triangle[1]]._0 = vertex[0];
		triangles[triangle[1]]._1 = vertex[3];
		triangles[triangle[1]]._2 = vertex[1];
	}
	vi->SetIndicesAsTrianglesNocopy(&triangles, numTriangles);

	vi->RecalculcateNormals();
	vi->RecalculateTangentsBinormals();
	vi->RecalculateBounds();

	if (!m_mesh)
	{
		VIBuffer* viBuffer = nullptr;
		VIBuffer::CreateVIBufferNocopy(
			system->graphic->device, system->graphic->deviceContext,
			&vi,
			D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
			D3D11_USAGE_IMMUTABLE, 0, 0,
			&viBuffer);

		m_mesh = system->resource->factory->CreateMeshNocopyUM(&viBuffer);
	}
	else
	{
		m_mesh->viBuffer->RegenerateVertexBuffer();
		m_mesh->viBuffer->RegenerateIndexBuffer();
	}

	m_numRect = numRect;
}

void LineRenderer::SetupVertexPair(uint pointIndex, const V3& camDir, float width, V3& inout_min, V3& inout_max)
{
	VI* vi = m_mesh->viBuffer->GetVI();
	Vertex* vertices = vi->GetVertices();

	// Calc indices of vertices

	uint beginIndex = pointIndex * 2;
	uint vertex[2] = { beginIndex, beginIndex + 1 };
	uint next = pointIndex + 1;
	if (pointIndex == m_points.size() - 1)
		next = pointIndex - 1;

	const V3& curPoint = m_points[pointIndex];
	const V3& nextPoint = m_points[next];

	V3 delta = nextPoint - curPoint;
	V3 forward = delta.normalized;
	if (pointIndex == m_points.size() - 1)
		forward *= -1.0f;

	V3 right, up;
	{
		right = V3::Cross(camDir, forward).normalized;
		up = V3::Cross(right, forward).normalized;
	}

	// Set positions of pair

	float halfWidth = width * 0.5f;
	vertices[vertex[0]].position = curPoint + right * halfWidth;
	vertices[vertex[1]].position = curPoint - right * halfWidth;

	// Set normals of pair

	vertices[vertex[0]].normal = up;
	vertices[vertex[1]].normal = up;
	vertices[vertex[0]].biNormal = right;
	vertices[vertex[1]].biNormal = right;
	vertices[vertex[0]].tangent = forward;
	vertices[vertex[1]].tangent = forward;

	// Set uvw of pair

	float percent = float(pointIndex) / m_points.size();

	vertices[vertex[0]].uvw.x = 0.0f;
	vertices[vertex[1]].uvw.x = 1.0f;

	vertices[vertex[0]].uvw.z = percent;
	vertices[vertex[1]].uvw.z = percent;

	vertices[vertex[0]].uvw.y = percent;
	vertices[vertex[1]].uvw.y = percent;

	// Calc extents

	V3 minV = V3::Min(vertices[vertex[0]].position, vertices[vertex[1]].position);
	V3 maxV = V3::Max(vertices[vertex[0]].position, vertices[vertex[1]].position);

	inout_min = V3::Min(inout_min, minV);
	inout_max = V3::Max(inout_max, maxV);
}

void LineRenderer::ApplyVertices()
{
	Camera* mainCamera = (Camera*)system->graphic->cameraManager->mainCamera;
	V3 camDir = mainCamera->transform->forward;

	V3 minV = V3(FLT_MAX, FLT_MAX, FLT_MAX);
	V3 maxV = V3(FLT_MIN, FLT_MIN, FLT_MIN);

	for (int i = 0; i < int(m_points.size()); ++i)
	{
		SetupVertexPair(i, camDir, m_width, minV, maxV);
	}

	Bounds bounds;
	bounds.extents = (maxV - minV) * 0.5f;
	bounds.center = (minV + maxV) * 0.5f;

	VI* vi = m_mesh->viBuffer->GetVI();
	vi->SetBounds(bounds);

	//vi->RecalculcateNormals();
	//vi->RecalculateTangentsBinormals();
	//vi->RecalculateBounds();

	m_mesh->viBuffer->UpdateVertexBuffer();
	m_mesh->viBuffer->UpdateIndexBuffer();
}
