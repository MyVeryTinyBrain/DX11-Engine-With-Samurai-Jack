#include "EnginePCH.h"
#include "VI.h"

VI::VI()
{
	SetPrimitiveTopology(PrimitiveTopology::Triangles);
}

VI::VI(const VI& rhs)
{
	m_vertices = new Vertex[rhs.m_vertexCount];
	memcpy(m_vertices, rhs.m_vertices, sizeof(Vertex) * rhs.m_vertexCount);
	m_vertexCount = rhs.m_vertexCount;

	m_indexCount = rhs.m_indexCount;
	m_primitiveCount = rhs.m_primitiveCount;
	m_primitiveVertexCount = rhs.m_primitiveVertexCount;
	m_primitive = rhs.m_primitive;

	m_arrSubIndicesDesc = new SubIndicesDesc * [rhs.m_subIndecesCount]{};
	for (UINT i = 0; i < rhs.m_subIndecesCount; ++i)
	{
		if (rhs.m_arrSubIndicesDesc[i])
			m_arrSubIndicesDesc[i] = new SubIndicesDesc(*rhs.m_arrSubIndicesDesc[i]);
	}
	m_subIndecesCount = rhs.m_subIndecesCount;

	m_bounds = rhs.m_bounds;
}

VI::~VI()
{
	SafeDeleteArray(m_vertices);

	for (UINT i = 0; i < m_subIndecesCount; ++i)
		SafeDelete(m_arrSubIndicesDesc[i]);
	SafeDeleteArray(m_arrSubIndicesDesc);
}

HRESULT VI::SetVertices(const Vertex* vertices, UINT count)
{
	if (!vertices)
		return E_FAIL;

	SafeDeleteArray(m_vertices);
	m_vertexCount = 0;

	m_vertices = new Vertex[count]{};
	m_vertexCount = count;

	memcpy(m_vertices, vertices, sizeof(Vertex) * count);

	SetPrimitiveTopology(m_primitive);
	RecalculateBounds();

	return S_OK;
}

HRESULT VI::SetVerticesNocopy(Vertex** ppVertices, UINT count)
{
	if (!ppVertices || !*ppVertices)
		return E_FAIL;

	SafeDeleteArray(m_vertices);
	m_vertexCount = 0;

	m_vertices = *ppVertices;
	m_vertexCount = count;

	SetPrimitiveTopology(m_primitive);
	RecalculateBounds();

	*ppVertices = nullptr;

	return S_OK;
}

HRESULT VI::SetIndices(const Index* indices, UINT count)
{
	if (!indices)
		return E_FAIL;

	HRESULT hr = S_OK;

	SetSubIndicesCount(1);

	if (FAILED(hr = SetSubIndices(indices, count, 0)))
		return hr;

	return S_OK;
}

HRESULT VI::SetIndicesNocopy(Index** ppIndices, UINT count)
{
	SetSubIndicesCount(1);

	return SetSubIndicesNocopy(ppIndices, count, 0);
}

HRESULT VI::SetIndicesAsTriangles(const TriangleIndex* triangles, UINT count)
{
	if (!triangles)
		return E_FAIL;

	return SetIndices((const Index*)triangles, count * 3);
}

HRESULT VI::SetIndicesAsTrianglesNocopy(TriangleIndex** ppTriangles, UINT count)
{
	return SetIndicesNocopy((Index**)ppTriangles, count * 3);
}

HRESULT VI::SetSubIndices(const Index* indices, UINT count, UINT index)
{
	if (!indices)
		return E_FAIL;

	if (!m_arrSubIndicesDesc)
		return E_FAIL;
	
	if (index >= m_subIndecesCount)
		return E_FAIL;

	if (m_arrSubIndicesDesc[index])
		m_indexCount -= m_arrSubIndicesDesc[index]->Count;

	SafeDelete(m_arrSubIndicesDesc[index]);

	// 인덱스 배열을 복사하여 저정합니다.
	m_arrSubIndicesDesc[index] = new SubIndicesDesc(indices, count);
	m_indexCount += count;

	SetPrimitiveTopology(m_primitive);

	return S_OK;
}

HRESULT VI::SetSubIndicesNocopy(Index** ppIndices, UINT count, UINT index)
{
	auto ReleaseVars = [&]()
	{
		SafeDeleteArray(*ppIndices);
	};

	if (!ppIndices || !*ppIndices)
		return E_FAIL;

	if (!m_arrSubIndicesDesc)
	{
		ReleaseVars();
		return E_FAIL;
	}

	if (index >= m_subIndecesCount)
	{
		ReleaseVars();
		return E_FAIL;
	}

	if (m_arrSubIndicesDesc[index])
		m_indexCount -= m_arrSubIndicesDesc[index]->Count;

	SafeDelete(m_arrSubIndicesDesc[index]);

	// 인덱스 배열을 복사하지 않고 주소를 저장합니다.
	m_arrSubIndicesDesc[index] = new SubIndicesDesc(*ppIndices, count);
	m_indexCount += count;

	SetPrimitiveTopology(m_primitive);

	*ppIndices = nullptr;

	return S_OK;
}

HRESULT VI::SetSubIndicesAsTriangles(const TriangleIndex* triangles, UINT count, UINT index)
{
	return SetSubIndices((const Index*)triangles, count * 3, index);
}

HRESULT VI::SetSubIndicesAsTrianglesNocopy(TriangleIndex** ppTriangles, UINT count, UINT index)
{
	return SetSubIndicesNocopy((Index**)ppTriangles, count * 3, index);
}

void VI::SetSubIndicesCount(UINT count)
{
	SubIndicesDesc** arrSubIndices = new SubIndicesDesc * [count];
	ZeroMemory(arrSubIndices, sizeof(SubIndicesDesc*) * count);

	if (m_arrSubIndicesDesc)
	{
		for (UINT i = 0; i < m_subIndecesCount; ++i)
		{
			if (i < count)
			{
				arrSubIndices[i] = m_arrSubIndicesDesc[i];
			}
			else
			{
				SafeDelete(m_arrSubIndicesDesc[i]);
			}
		}
	}
	SafeDeleteArray(m_arrSubIndicesDesc);

	m_arrSubIndicesDesc = arrSubIndices;
	m_subIndecesCount = count;
}

void VI::SetPrimitiveTopology(PrimitiveTopology value)
{
	switch (value)
	{
		case PrimitiveTopology::Points:
			m_primitiveCount = m_indexCount;
			m_primitiveVertexCount = 1;
			break;
		case PrimitiveTopology::Lines:
			m_primitiveCount = m_indexCount / 2;
			m_primitiveVertexCount = 2;
			break;
		case PrimitiveTopology::LineStrip:
			m_primitiveCount = m_indexCount - 1;
			m_primitiveVertexCount = 2;
			break;
		case PrimitiveTopology::Triangles:
			m_primitiveCount = m_indexCount / 3;
			m_primitiveVertexCount = 3;
			break;
	}

	m_primitive = value;
}

HRESULT VI::GetStartOfSubIndices(UINT index, UINT* out_start) const
{
	if (!out_start)
		return E_FAIL;

	if (index >= m_subIndecesCount)
		return E_FAIL;

	UINT result = 0;

	for (UINT i = 0; i < index; ++i)
	{
		SubIndicesDesc* desc = GetSubIndicesDesc(i);
		if (!desc)
			return E_FAIL;

		result += desc->Count;
	}

	*out_start = result;

	return S_OK;
}

HRESULT VI::RecalculcateNormals()
{
	if (m_primitive != PrimitiveTopology::Triangles)
		return E_FAIL;

	if (!m_vertices || !m_arrSubIndicesDesc)
		return E_FAIL;

	// Clear normals
	for (uint32_t i = 0; i < m_vertexCount; ++i)
	{
		Vertex& vertex = m_vertices[i];
		vertex.normal = V3::zero();
	}

	// Calculate normals
	for (UINT i = 0; i < m_subIndecesCount; ++i)
	{
		TriangleIndex* triangles = (TriangleIndex*)m_arrSubIndicesDesc[i]->SubIncides;
		uint triangleCount = m_arrSubIndicesDesc[i]->Count / 3;

		for (uint j = 0; j < triangleCount; ++j)
		{
			Vertex& v0 = m_vertices[triangles[j]._0];
			Vertex& v1 = m_vertices[triangles[j]._1];
			Vertex& v2 = m_vertices[triangles[j]._2];

			V3 dir_0_1 = v1.position - v0.position;
			V3 dir_1_2 = v2.position - v1.position;
			V3 normal = V3::Cross(dir_0_1, dir_1_2).normalized;

			v0.normal += normal;
			v1.normal += normal;
			v2.normal += normal;
		}

		for (uint j = 0; j < triangleCount; ++j)
		{
			Vertex& v0 = m_vertices[triangles[j]._0];
			Vertex& v1 = m_vertices[triangles[j]._1];
			Vertex& v2 = m_vertices[triangles[j]._2];

			v0.normal.Normalize();
			v1.normal.Normalize();
			v2.normal.Normalize();
		}
	}

	return S_OK;
}

HRESULT VI::RecalculateTangentsBinormals()
{
	if (m_primitive != PrimitiveTopology::Triangles)
		return E_FAIL;

	if (!m_vertices || !m_arrSubIndicesDesc)
		return E_FAIL;

	for (UINT i = 0; i < m_subIndecesCount; ++i)
	{
		TriangleIndex* triangles = (TriangleIndex*)m_arrSubIndicesDesc[i]->SubIncides;
		uint triangleCount = m_arrSubIndicesDesc[i]->Count / 3;

		for (uint j = 0; j < triangleCount; ++j)
		{
			Vertex& v0 = m_vertices[triangles[j]._0];
			Vertex& v1 = m_vertices[triangles[j]._1];
			Vertex& v2 = m_vertices[triangles[j]._2];

			V3 delta0 = v1.position - v0.position;
			V3 delta1 = v2.position - v0.position;

			V2 tu = v1.uv - v0.uv;
			V2 tv = v2.uv - v0.uv;

			float cross = (tu.x * tv.y - tu.y * tv.x);
			if (cross == 0)
				cross = 1.0f;
			float deno = 1.0f / cross;

			V3 tangent;
			tangent.x = (tv.y * delta0.x - tv.x * delta1.x) * deno;
			tangent.y = (tv.y * delta0.y - tv.x * delta1.y) * deno;
			tangent.z = (tv.y * delta0.z - tv.x * delta1.z) * deno;

			V3 binormal;
			binormal.x = (tu.x * delta1.x - tu.y * delta0.x) * deno;
			binormal.y = (tu.x * delta1.y - tu.y * delta0.y) * deno;
			binormal.z = (tu.x * delta1.z - tu.y * delta0.z) * deno;

			tangent.Normalize();
			binormal.Normalize();

			v0.biNormal = v1.biNormal = v2.biNormal = binormal;
			v0.tangent = v1.tangent = v2.tangent = tangent;

		}
	}

	return S_OK;
}

HRESULT VI::RecalculateBounds()
{
	if (!m_vertices)
		return E_FAIL;

	V3 extents = V3::zero();

	for (uint32_t i = 0; i < m_vertexCount; ++i)
	{
		const Vertex& vertex = m_vertices[i];
		const V3& position = vertex.position;
		extents = V3::Max(extents.xmv, V3::Abs(position));
	}

	m_bounds = Bounds(V3::zero(), extents);

	return S_OK;
}

HRESULT VI::CreateIndicesStream(IndicesStream* out_indices) const
{
	if (!out_indices || !m_arrSubIndicesDesc)
		return E_FAIL;

	Index* indices = new Index[m_indexCount];

	UINT cursor = 0;
	for (size_t i = 0; i < m_subIndecesCount; ++i)
	{
		if (!m_arrSubIndicesDesc[i])
			continue;
		UINT count = m_arrSubIndicesDesc[i]->Count;
		memcpy(indices + cursor, m_arrSubIndicesDesc[i]->SubIncides, sizeof(Index) * count);
		cursor += count;
	}

	*out_indices = IndicesStream(indices, m_indexCount);

	return S_OK;
}

bool VI::AABBInLocalSpace(const Ray& localRay, float& out_dist) const
{
	return m_bounds.Raycast(localRay, out_dist);
}

bool VI::AABBInWorldSpace(const Ray& worldRay, float& out_dist, const M4& localToWorldMatrix) const
{
	M4 worldToLocalMatrix = localToWorldMatrix.inversed;

	Ray localRay;
	localRay.origin = worldToLocalMatrix.MultiplyPoint(worldRay.origin);
	localRay.direction = worldToLocalMatrix.MultiplyVector(worldRay.direction);
	localRay.direction = V3(localRay.direction).normalized;

	return AABBInLocalSpace(localRay, out_dist);
}

bool VI::RaycastInLocalSpace(V3& out_localHit, const Ray& localRay, bool in90Degrees) const
{
	if (!m_vertices || !m_arrSubIndicesDesc)
		return false;

	if (localRay.direction.sqrMagnitude < 0.99f)
		return false;

	float dist;
	Triangle lightBlend;

	//if (!m_bounds.Raycast(localRay, dist))
	//	return false;

	for (uint i = 0; i < m_subIndecesCount; ++i)
	{
		uint triCount = m_arrSubIndicesDesc[i]->Count / 3;
		TriangleIndex* triangles = (TriangleIndex*)m_arrSubIndicesDesc[i]->SubIncides;

		for (uint j = 0; j < triCount; ++j)
		{
			lightBlend.p0 = m_vertices[triangles[j]._0].position;
			lightBlend.p1 = m_vertices[triangles[j]._1].position;
			lightBlend.p2 = m_vertices[triangles[j]._2].position;

			if (in90Degrees)
			{
				if (lightBlend.RaycastIn90Degrees(localRay, dist))
				{
					out_localHit = localRay.GetPoint(dist);
					return true;
				}
			}
			else
			{
				if (lightBlend.Raycast(localRay, dist))
				{
					out_localHit = localRay.GetPoint(dist);
					return true;
				}
			}
		}
	}

	return false;
}

bool VI::RaycastInWorldSpace(V3& out_worldHit, const Ray& worldRay, const M4& localToWorldMatrix, bool in90Degrees) const
{
	M4 worldToLocalMatrix = localToWorldMatrix.inversed;

	Ray localRay;
	localRay.origin = worldToLocalMatrix.MultiplyPoint(worldRay.origin);
	localRay.direction = worldToLocalMatrix.MultiplyVector(worldRay.direction);
	localRay.direction = V3(localRay.direction).normalized;

	V3 localHit;
	if (RaycastInLocalSpace(localHit, localRay, in90Degrees))
	{
		out_worldHit = localToWorldMatrix.MultiplyPoint(localHit);
		return true;
	}

	return false;
}

