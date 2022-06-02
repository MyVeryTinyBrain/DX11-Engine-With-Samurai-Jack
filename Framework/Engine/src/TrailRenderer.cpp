#include "EnginePCH.h"
#include "TrailRenderer.h"
#include "Mesh.h"
#include "System.h"
#include "ResourceFactory.h"
#include "VIBuffer.h"
#include "VI.h"
#include "GraphicSystem.h"
#include "ICamera.h"
#include "Transform.h"
#include "ResourceManagement.h"
#include "BuiltInResources.h"
#include "Material.h"
#include "RenderQueue.h"
#include "Texture.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Times.h"

void TrailRenderer::Awake()
{
	Renderer::Awake();
	SetupMesh();
	SetMaterial(system->resource->builtIn->whiteColorMaterial);
	SetNumMeshRect(1024);
}

void TrailRenderer::LateUpdate()
{
	Renderer::LateUpdate();

	if (m_beginShrinkCounter > 0)
	{
		m_beginShrinkCounter -= system->time->deltaTime;
	}
	else
	{
		Shrink(m_shrinkDistance * system->time->deltaTime);
	}

	if (m_autoTrail)
	{
		const V3& position = transform->position;
		if (m_datas.empty())
		{
			AddPosition(transform->position, transform->rotation);
		}
		else
		{
			const V3& last = m_datas.back().position;
			float dist = V3::Distance(position, last);

			if (dist > m_minVertexDistance)
			{
				AddPosition(transform->position, transform->rotation);
			}
		}
	}
}

void TrailRenderer::Render()
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
		input.essential.worldMatrix = M4::identity();
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
		memcpy(input.essential.instanceData, GetInstanceDataArray(), sizeof(Real4) * 4);

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

void TrailRenderer::OnCamera(ICamera* camera, RenderRequest* inout_pinput)
{
	if (IsValid())
		ApplyVertices(camera);
}

void TrailRenderer::AddPosition(const V3& position, const Q& rotation)
{
	if (m_datas.empty())
	{
		m_beginShrinkCounter = m_beginShrinkDelay;
	}

	Data data;
	data.position = position;
	data.rotation = rotation;

	if (m_datas.empty())
	{
		data.distanceAcc = 0.0f;
	}
	else
	{
		const Data& back = m_datas.back();
		data.distanceAcc = back.distanceAcc + V3::Distance(back.position, position);
	}

	m_datas.push_back(data);
}

void TrailRenderer::PopPosition()
{
	if (!m_datas.empty())
		m_datas.pop_front();
}

bool TrailRenderer::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds TrailRenderer::GetBounds() const
{
	if (!m_mesh)
		return Bounds(transform->position, V3::zero());

	return m_mesh->GetBounds();
}

bool TrailRenderer::IsValid() const
{
	if (!Renderer::IsValid())
		return false;

	return m_datas.size() >= 2;
}

void TrailRenderer::SetupMesh()
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

void TrailRenderer::SetNumMeshRect(uint numRect)
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

uint TrailRenderer::GetNumMeshRect() const
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

uint TrailRenderer::GetNumInterpolatedRect() const
{
	uint numPoints = (uint)m_datas.size();
	uint interpolatedNumRect = (m_interpolateStep + 1) * (numPoints - 1);
	return interpolatedNumRect;
}

void TrailRenderer::Shrink(float shrinkDistance)
{
	if (m_datas.empty())
		return;

	if (m_datas.size() == 1)
	{
		m_datas.clear();
		return;
	}

	Data& first = m_datas[0];
	Data& second = m_datas[1];

	V3 delta = second.position - first.position;
	float deltaDist = delta.magnitude;
	V3 dir = delta / deltaDist;
	float newDist = deltaDist - shrinkDistance;

	if (newDist < 0)
	{
		float shrinkDistance2 = shrinkDistance - deltaDist;
		PopPosition();
		Shrink(shrinkDistance2);
		return;
	}
	else
	{
		first.distanceAcc += shrinkDistance;
		first.position = second.position - dir * newDist;
	}
}

void TrailRenderer::ApplyVertices(ICamera* camera)
{
	uint numDatas = (uint)m_datas.size();
	uint interpolatedNumRect = GetNumInterpolatedRect();
	if (numDatas == 0) interpolatedNumRect = 0;
	
	if (interpolatedNumRect > GetNumMeshRect())
	{
		SetNumMeshRect(interpolatedNumRect * 2);
	}

	VI* vi = m_mesh->viBuffer->GetVI();
	Vertex* vertices = vi->GetVertices();
	V3 minV = V3(FLT_MAX, FLT_MAX, FLT_MAX);
	V3 maxV = V3(FLT_MIN, FLT_MIN, FLT_MIN);

	for (uint i = 0; i < uint(numDatas - 1); ++i)
	{
		uint currentIndex = i;
		uint nextIndex = (i + 1 >= numDatas) ? numDatas - 1 : i + 1;
		uint prevIndex = (i > 0) ? i - 1 : 0;
		uint next2Index = (i + 2 >= numDatas) ? numDatas - 1 : i + 2;

		const Data& prevData = m_datas[prevIndex];
		const Data& data = m_datas[currentIndex];
		const Data& nextData = m_datas[nextIndex];
		const Data& next2Data = m_datas[next2Index];

		for (uint j = 0; j < m_interpolateStep + 1; ++j)
		{
			uint totalIndex = i * (m_interpolateStep + 1) + j;
			float interpolatePercent = float(j) / float(m_interpolateStep + 1);
			float nextInterpolatePercent = float(j + 1) / float(m_interpolateStep + 1);

			VertexPairInput input;
			input.start = V3::CatMulRom(prevData.position, data.position, nextData.position, next2Data.position, interpolatePercent);
			input.end = V3::CatMulRom(prevData.position, data.position, nextData.position, next2Data.position, nextInterpolatePercent);
			input.alignment = m_alignment;
			input.camera = camera;
			input.percent = float(totalIndex) / float(interpolatedNumRect);
			input.width = m_width;
			input.inverse = false;
			input.rotation = Q::SLerp(data.rotation, nextData.rotation, interpolatePercent);
			input.distanceAcc = Lerp(data.distanceAcc, nextData.distanceAcc, interpolatePercent);

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
		input.end = m_datas[numDatas - 1].position;
		input.start = (vertices[interpolatedNumRect * 2 - 2].position + vertices[interpolatedNumRect * 2 - 1].position) * 0.5f;
		input.alignment = m_alignment;
		input.camera = camera;
		input.percent = 1.0f;
		input.width = m_width;
		input.inverse = true;
		input.rotation = m_datas[numDatas - 1].rotation;
		input.distanceAcc = m_datas[numDatas - 1].distanceAcc;

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

TrailRenderer::VertexPairOutput TrailRenderer::CalcVertexPair(const TrailRenderer::VertexPairInput& desc, V3& inout_min, V3& inout_max) const
{
	TrailRenderer::VertexPairOutput output;

	V3 a2b = desc.end - desc.start;
	if (desc.inverse) a2b *= -1.0f;
	V3 dir = a2b.normalized;

	V3 right, up;
	switch (desc.alignment)
	{
		case (TrailRenderer::Alignment::View):
		{
			right = V3::Cross(desc.camera->GetDirection(), dir).normalized;
			up = V3::Cross(right, dir).normalized;
		}
		break;
		case (TrailRenderer::Alignment::Local):
		{
			dir = desc.rotation.MultiplyVector(V3::forward());
			right = desc.rotation.MultiplyVector(V3::right());
			up = desc.rotation.MultiplyVector(V3::up());
		}
		break;
	}

	float halfWidth = desc.width * 0.5f;
	float scale = 1.0f;

	if (m_applyWidthByLength)
	{
		const Data& first = m_datas.front();
		const Data& end = m_datas.back();

		float firstDistance = first.distanceAcc;
		scale = Clamp01((desc.distanceAcc - first.distanceAcc) / (end.distanceAcc - first.distanceAcc));
	}

	if (!desc.inverse)
	{
		output.position[0] = desc.start + right * halfWidth * scale;
		output.position[1] = desc.start - right * halfWidth * scale;
	}
	else
	{
		output.position[0] = desc.end + right * halfWidth * scale;
		output.position[1] = desc.end - right * halfWidth * scale;
	}

	output.normal = up;
	output.binormal = right;
	output.tangent = dir;

	if (!m_fitUToWidth)
	{
		output.uvw[0].x = 0.0f;
		output.uvw[1].x = 1.0f;
	}
	else
	{
		output.uvw[0].x = 1.0f - (scale * 0.5f + 0.5f);
		output.uvw[1].x = scale * 0.5f + 0.5f;
	}

	if (m_fitVToLength)
	{
		output.uvw[0].y = desc.percent;
		output.uvw[1].y = desc.percent;
	}
	else
	{
		float v = desc.distanceAcc/ m_vScale;
		output.uvw[0].y = v;
		output.uvw[1].y = v;
	}

	output.uvw[0].z = desc.percent;
	output.uvw[1].z = desc.percent;

	V3 min = V3::Min(output.position[0], output.position[1]);
	V3 max = V3::Max(output.position[0], output.position[1]);
	inout_min = V3::Min(min, inout_min);
	inout_max = V3::Max(max, inout_max);

	return output;
}
