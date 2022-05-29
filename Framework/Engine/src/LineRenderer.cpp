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
	SetupMesh();
	SetMaterial(system->resource->builtIn->whiteMaterial);
}

void LineRenderer::LateUpdate()
{
	Renderer::LateUpdate();
}

void LineRenderer::Render()
{
	if (!isValid)
		return;

	M4 localToWorldMatrix = transform->localToWorldMatrix;

	if (m_materials.empty())
		return;

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
		input.customPrimitiveCount.primitiveCount = GetNumInterpolatedRect() * 2;

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

void LineRenderer::OnCamera(ICamera* camera, RenderRequest* inout_pinput)
{
	if (IsValid())
		ApplyVertices(camera);
}

void LineRenderer::AddPoint(const V3& point)
{
	m_points.push_back(point);
}

bool LineRenderer::RemovePoint(uint index)
{
	if (index >= m_points.size())
		return false;

	auto it = m_points.begin() + index;
	m_points.erase(it);

	return true;
}

void LineRenderer::ClearPoints()
{
	size_t prevSize = m_points.size();
	m_points.clear();
}

void LineRenderer::SetupMesh()
{
	Vertex* vertices = new Vertex[3]{};
	Index* indicies = new Index[3]{};
	VI* vi = new VI;
	vi->SetVerticesNocopy(&vertices, 3);
	vi->SetIndicesNocopy(&indicies, 3);
	VIBuffer* vibuffer = nullptr;
	VIBuffer::CreateVIBufferNocopy(
		system->graphic->device, system->graphic->deviceContext, &vi,
		D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&vibuffer);
	m_mesh = system->resource->factory->CreateMeshNocopyUM(&vibuffer);
}

void LineRenderer::SetNumMeshRect(uint numRect)
{
	if (GetNumMeshRect() == numRect)
		return;

	uint numVertices = numRect * 2 + 2;
	uint numTriangles = numRect * 2;
	Vertex* vertices = new Vertex[numVertices]{};
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};
	VIBuffer* vibuffer = m_mesh->GetVIBuffer();
	VI* vi = vibuffer->GetVI();

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

	vi->SetVerticesNocopy(&vertices, numVertices);
	vi->SetIndicesAsTrianglesNocopy(&triangles, numTriangles);
	vi->RecalculcateNormals();
	vi->RecalculateTangentsBinormals();
	vi->RecalculateBounds();

	vibuffer->RegenerateVertexBuffer();
	vibuffer->RegenerateIndexBuffer();
}

uint LineRenderer::GetNumMeshRect() const
{
	ResourceRef<Mesh> mesh = m_mesh;
	if (!mesh)
		return 0;

	VIBuffer* vibuffer = m_mesh->GetVIBuffer();
	if (!vibuffer)
		return 0;

	VI* vi = vibuffer->GetVI();
	if (!vi)
		return 0;

	uint vertexCount = vi->GetVertexCount();
	uint numRect = (vertexCount / 2) - 1;

	return numRect;
}

uint LineRenderer::GetNumInterpolatedRect() const
{
	uint numPoints = (uint)m_points.size();
	uint interpolatedNumRect = (m_interpolateStep + 1) * (numPoints - 1);
	return interpolatedNumRect;
}

void LineRenderer::ApplyVertices(ICamera* camera)
{
	uint interpolatedNumRect = GetNumInterpolatedRect();
	if (numPoints == 0) interpolatedNumRect = 0;

	if (interpolatedNumRect > GetNumMeshRect())
	{
		SetNumMeshRect(interpolatedNumRect * 2);
	}

	VI* vi = m_mesh->viBuffer->GetVI();
	Vertex* vertices = vi->GetVertices();
	V3 minV = V3(FLT_MAX, FLT_MAX, FLT_MAX);
	V3 maxV = V3(FLT_MIN, FLT_MIN, FLT_MIN);

	for (uint i = 0; i < uint(numPoints - 1); ++i)
	{
		uint currentIndex = i;
		uint nextIndex = (i + 1 >= numPoints) ? numPoints - 1 : i + 1;
		uint prevIndex = (i > 0) ? i - 1 : 0;
		uint next2Index = (i + 2 >= numPoints) ? numPoints - 1 : i + 2;

		const V3& prevPoint = m_points[prevIndex];
		const V3& point = m_points[currentIndex];
		const V3& nextPoint = m_points[nextIndex];
		const V3& next2Point = m_points[next2Index];

		for (uint j = 0; j < m_interpolateStep + 1; ++j)
		{
			uint totalIndex = i * (m_interpolateStep + 1) + j;
			float interpolatePercent = float(j) / float(m_interpolateStep + 1);
			float nextInterpolatePercent = float(j + 1) / float(m_interpolateStep + 1);

			VertexPairInput input;
			input.start = V3::CatMulRom(prevPoint, point, nextPoint, next2Point, interpolatePercent);
			input.end = V3::CatMulRom(prevPoint, point, nextPoint, next2Point, nextInterpolatePercent);
			input.alignment = m_alignment;
			input.camera = camera;
			input.percent = float(totalIndex) / float(interpolatedNumRect);
			input.width = m_width;
			input.inverse = false;

			VertexPairOutput output = CalcVertexPair(input, minV, maxV);

			uint vertexIndexL = totalIndex * 2;
			uint vertexIndexR = vertexIndexL + 1;
			vertices[vertexIndexL].position = output.position[0];
			vertices[vertexIndexR].position = output.position[1];
			vertices[vertexIndexL].uvw = output.uvw[0];
			vertices[vertexIndexR].uvw = output.uvw[1];
			vertices[vertexIndexL].normal = output.normal;
			vertices[vertexIndexR].normal = output.normal;
			vertices[vertexIndexL].biNormal = output.binormal;
			vertices[vertexIndexR].biNormal = output.binormal;
			vertices[vertexIndexL].tangent = output.tangent;
			vertices[vertexIndexR].tangent = output.tangent;
		}
	}

	{
		VertexPairInput input;
		input.end = m_points[numPoints - 1];
		input.start = (vertices[interpolatedNumRect * 2 - 2].position + vertices[interpolatedNumRect * 2 - 1].position) * 0.5f;
		input.alignment = m_alignment;
		input.camera = camera;
		input.percent = 1.0f;
		input.width = m_width;
		input.inverse = true;

		VertexPairOutput output = CalcVertexPair(input, minV, maxV);

		uint vertexIndexL = interpolatedNumRect * 2;
		uint vertexIndexR = vertexIndexL + 1;
		vertices[vertexIndexL].position = output.position[0];
		vertices[vertexIndexR].position = output.position[1];
		vertices[vertexIndexL].uvw = V3(0, 1, 1);
		vertices[vertexIndexR].uvw = V3(1, 1, 1);
		vertices[vertexIndexL].normal = output.normal;
		vertices[vertexIndexR].normal = output.normal;
		vertices[vertexIndexL].biNormal = output.binormal;
		vertices[vertexIndexR].biNormal = output.binormal;
		vertices[vertexIndexL].tangent = output.tangent;
		vertices[vertexIndexR].tangent = output.tangent;
	}

	Bounds bounds;
	bounds.extents = (maxV - minV) * 0.5f;
	bounds.center = (minV + maxV) * 0.5f;
	vi->SetBounds(bounds);

	m_mesh->viBuffer->UpdateVertexBuffer();
	m_mesh->viBuffer->UpdateIndexBuffer();
}

LineRenderer::VertexPairOutput LineRenderer::CalcVertexPair(const LineRenderer::VertexPairInput& desc, V3& inout_min, V3& inout_max) const
{
	LineRenderer::VertexPairOutput output;

	V3 a2b = desc.end - desc.start;
	if (desc.inverse) a2b *= -1.0f;
	V3 dir = a2b.normalized;

	V3 right, up;
	switch (desc.alignment)
	{
		case (LineRenderer::Alignment::View):
		{
			right = V3::Cross(desc.camera->GetDirection(), dir).normalized;
			up = V3::Cross(right, dir).normalized;
		}
		break;
		case (LineRenderer::Alignment::Local):
		{
			up = transform->up;
			right = V3::Cross(up, dir).normalized;
		}
		break;
	}

	float halfWidth = desc.width * 0.5f;

	if (!desc.inverse)
	{
		output.position[0] = desc.start + right * halfWidth;
		output.position[1] = desc.start - right * halfWidth;
	}
	else
	{
		output.position[0] = desc.end + right * halfWidth;
		output.position[1] = desc.end - right * halfWidth;
	}

	output.normal = up;
	output.binormal = right;
	output.tangent = dir;

	output.uvw[0].x = 0.0f;
	output.uvw[1].x = 1.0f;
	output.uvw[0].y = desc.percent;
	output.uvw[1].y = desc.percent;
	output.uvw[0].z = desc.percent;
	output.uvw[1].z = desc.percent;

	V3 min = V3::Min(output.position[0], output.position[1]);
	V3 max = V3::Max(output.position[0], output.position[1]);
	inout_min = V3::Min(min, inout_min);
	inout_max = V3::Max(max, inout_max);

	return output;
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

