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

	SetupMesh(512);

	SetMaterial(system->resource->builtIn->whiteMaterial);
}

void TrailRenderer::LateUpdate()
{
	if (m_beginShrinkCounter > 0)
	{
		m_beginShrinkCounter -= system->time->deltaTime;
	}
	else
	{
		Shrink(m_shrinkDistance * system->time->deltaTime);
	}

	if(m_autoTrail)
	{
		const V3& position = transform->position;
		if (m_datas.empty())
		{
			AddPosition(transform->position, transform->rotation);
		}
		else
		{
			const V3& last = m_datas.back().Position;
			float dist = V3::Distance(position, last);

			if (dist > m_minVertexDistance)
			{
				AddPosition(transform->position, transform->rotation);

				//if (m_pairs.size() <= 2)
				//{
				//	AddPosition(transform->position, transform->rotation);
				//}
				//else
				//{
				//	auto end = m_pairs.end() - 1;
				//	auto prevend = end - 1;
				//	Pair c;
				//	c.Position = transform->position;
				//	c.Rotation = transform->rotation;
				//	const Pair& b = *end;
				//	const Pair& a = *prevend;
				//	for (int i = 0; i < 20; ++i)
				//	{
				//		float percent = float(i + 1) / float(20);
				//		V3 position = V3::CatMulRom(a.Position, b.Position, c.Position, c.Position, percent);
				//		Q rotation = Q::SLerp(b.Rotation, c.Rotation, percent);
				//		AddPosition(position, rotation);
				//	}
				//}
			}
		}
	}
}

void TrailRenderer::Render()
{
	if (!isValid)
		return;

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

		input.customPrimitiveCount.usePrimitiveCount = true;
		input.customPrimitiveCount.primitiveCount = uint(m_datas.size() - 1) * 2;

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

void TrailRenderer::AddPosition(const V3& position, const Q& rotation)
{
	if (m_datas.size() >= m_numRect)
	{
		PopPosition();
	}

	if (m_datas.empty())
	{
		m_beginShrinkCounter = m_beginShrinkDelay;
	}

	Data data;
	data.Position = position;
	data.Rotation = rotation;

	if (m_datas.empty())
	{
		data.DistanceAccumulation = 0.0f;
	}
	else
	{
		const Data& back = m_datas.back();
		data.DistanceAccumulation = back.DistanceAccumulation + V3::Distance(back.Position, position);
	}

	m_datas.push_back(data);
}

void TrailRenderer::PopPosition()
{
	if (!m_datas.empty())
		m_datas.pop_front();
}

void TrailRenderer::SetRectCount(uint value)
{
	if (m_numRect == value)
		return;

	SetupMesh(value);
}

bool TrailRenderer::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds TrailRenderer::GetBounds() const
{
	if (!m_mesh)
		return Bounds(transform->position, V3::zero());

	// 이미 월드 공간에 있으므로, 월드 변환을 하지 않은채로 반환합니다.
	return m_mesh->GetBounds();
}

bool TrailRenderer::IsValid() const
{
	if (!Renderer::IsValid())
		return false;

	return m_datas.size() >= 2;
}

void TrailRenderer::SetupMesh(uint numRect)
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

	V3 delta = second.Position - first.Position;
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
		first.DistanceAccumulation += shrinkDistance;
		first.Position = second.Position - dir * newDist;
	}
}

void TrailRenderer::SetupVertexPair(
	uint dataIndex, 
	const V3& camDir, 
	const V3& camPos,
	float width, 
	V3& inout_min, V3& inout_max)
{
	VI* vi = m_mesh->viBuffer->GetVI();
	Vertex* vertices = vi->GetVertices();

	// Calc indices of vertices

	uint beginIndex = dataIndex * 2;
	uint vertex[2] = { beginIndex, beginIndex + 1 };
	uint next = dataIndex + 1;
	if (dataIndex == m_datas.size() - 1)
		next = dataIndex - 1;

	Data& curPair = m_datas[dataIndex];
	Data& nextPair = m_datas[next];

	V3 delta = nextPair.Position - curPair.Position;
	float deltaDist = delta.magnitude;
	V3 forward = delta / deltaDist;
	if (dataIndex == m_datas.size() - 1)
		forward *= -1.0f;

	V3 right, up;

	//V3 c2p = (curPair.Position - camPos).normalized;

	switch (m_alignment)
	{
		case TrailRenderer::Alignment::View:
			right = V3::Cross(camDir, forward).normalized;
			//right = V3::Cross(c2p, forward).normalized;
			up = V3::Cross(right, forward).normalized;
			break;
		case TrailRenderer::Alignment::Local:
			forward = curPair.Rotation.MultiplyVector(V3::forward());
			right = curPair.Rotation.MultiplyVector(V3::right());
			up = curPair.Rotation.MultiplyVector(V3::up());
			break;
	}

	// Set positions of pair

	float halfWidth = width * 0.5f;
	float scale = 1.0f;

	if (m_applyWidthByLength)
	{
		Data& first = m_datas.front();
		Data& end = m_datas.back();

		float firstDistance = first.DistanceAccumulation;
		scale = Clamp01((curPair.DistanceAccumulation - first.DistanceAccumulation) / (end.DistanceAccumulation - first.DistanceAccumulation));
	}

	vertices[vertex[0]].position = curPair.Position + right * halfWidth * scale;
	vertices[vertex[1]].position = curPair.Position - right * halfWidth * scale;

	// Set normals of pair

	vertices[vertex[0]].normal = up;
	vertices[vertex[1]].normal = up;
	vertices[vertex[0]].biNormal = right;
	vertices[vertex[1]].biNormal = right;
	vertices[vertex[0]].tangent = forward;
	vertices[vertex[1]].tangent = forward;

	// Set uvw of pair

	float percent = float(dataIndex) / m_datas.size();

	if (!m_fitUToWidth)
	{
		vertices[vertex[0]].uvw.x = 0.0f;
		vertices[vertex[1]].uvw.x = 1.0f;
	}
	else
	{
		vertices[vertex[0]].uvw.x = 1.0f - (scale * 0.5f + 0.5f);
		vertices[vertex[1]].uvw.x = scale * 0.5f + 0.5f;
	}

	vertices[vertex[0]].uvw.z = percent;
	vertices[vertex[1]].uvw.z = percent;

	if (m_fitVToLength)
	{
		vertices[vertex[0]].uvw.y = percent;
		vertices[vertex[1]].uvw.y = percent;
	}
	else
	{
		float v = curPair.DistanceAccumulation / m_vScale;

		vertices[vertex[0]].uvw.y = v;
		vertices[vertex[1]].uvw.y = v;
	}

	// Calc extents

	V3 minV = V3::Min(vertices[vertex[0]].position, vertices[vertex[1]].position);
	V3 maxV = V3::Max(vertices[vertex[0]].position, vertices[vertex[1]].position);

	inout_min = V3::Min(inout_min, minV);
	inout_max = V3::Max(inout_max, maxV);
}

void TrailRenderer::ApplyVertices()
{
	Camera* mainCamera = (Camera*)system->graphic->cameraManager->mainCamera;
	V3 camDir = mainCamera->transform->forward;
	V3 camPos = mainCamera->transform->position;

	V3 minV = V3(FLT_MAX, FLT_MAX, FLT_MAX);
	V3 maxV = V3(FLT_MIN, FLT_MIN, FLT_MIN);

	for (int i = 0; i < int(m_datas.size()); ++i)
	{
		SetupVertexPair(i, camDir, camPos, m_width, minV, maxV);
	}

	//bool drawCanInterpolate = m_datas.size() >= 4;
	//for (int i = 0; i < int(m_datas.size()); ++i)
	//{
	//	if (i > 0) // 1번째 데이터부터 CatMulRom 보간 적용이 가능합니다.
	//	{
	//		for (int j = 0; j < m_numInterpolate; ++j)
	//		{

	//		}
	//	}
	//	//SetupVertexPair(i, camDir, camPos, m_width, minV, maxV);
	//	// 인덱스 i대신 두 개의 데이터를 입력받아야 함
	//}

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
