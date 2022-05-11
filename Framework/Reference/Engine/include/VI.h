#pragma once

#include "VIType.h"
#include "SubIndicesDesc.h"

ENGINE_BEGIN

class ENGINE_API VI
{
public:

	VI();

	VI(const VI& rhs);

	~VI();

public:

	// Nomally, const T* types are copy on other memory
	// you can release value of the types
	// 
	// And, Nocopy functions are not to copy to other memory

	HRESULT SetVertices(const Vertex* vertices, UINT count);

	HRESULT SetVerticesNocopy(Vertex** ppVertices, UINT count);

	// it changes subindex count to one
	HRESULT SetIndices(const Index* indices, UINT count);

	// it changes subindex count to one
	HRESULT SetIndicesNocopy(Index** ppIndices, UINT count);

	// it changes subindex count to one
	HRESULT SetIndicesAsTriangles(const TriangleIndex* triangles, UINT count);

	// it changes subindex count to one
	HRESULT SetIndicesAsTrianglesNocopy(TriangleIndex** ppTriangles, UINT count);

	HRESULT SetSubIndices(const Index* indices, UINT count, UINT index);

	HRESULT SetSubIndicesNocopy(Index** ppIndices, UINT count, UINT index);

	HRESULT SetSubIndicesAsTriangles(const TriangleIndex* triangles, UINT count, UINT index);

	HRESULT SetSubIndicesAsTrianglesNocopy(TriangleIndex** ppTriangles, UINT count, UINT index);

	void SetSubIndicesCount(UINT count);

	void SetPrimitiveTopology(PrimitiveTopology value);

	void SetBounds(const Bounds& value) { m_bounds = value; }

public:

	inline bool IsValid() const { return m_vertices && m_arrSubIndicesDesc; }

	inline Vertex* GetVertices() const { return m_vertices; }

	inline UINT GetVertexCount() const { return m_vertexCount; }

	inline UINT GetIndexCount() const { return m_indexCount; }

	inline PrimitiveTopology GetPrimitiveTopology() const { return m_primitive; }

	inline D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopologyAsD3DType() const { return (D3D11_PRIMITIVE_TOPOLOGY)m_primitive; }

	inline UINT GetPrimitiveVertexCount() const { return m_primitiveVertexCount; }

	inline UINT GetSubIndicesCount() const { return m_subIndecesCount; }

	inline SubIndicesDesc* GetSubIndicesDesc(UINT index) const { return m_arrSubIndicesDesc ? m_arrSubIndicesDesc[index] : nullptr; }

	HRESULT GetStartOfSubIndices(UINT index, UINT* out_start) const;

	const Bounds& GetBounds() const { return m_bounds; }

public:

	HRESULT RecalculcateNormals();

	HRESULT RecalculateTangentsBinormals();

	HRESULT RecalculateBounds();

	HRESULT CreateIndicesStream(IndicesStream* out_indices) const;

public:

	bool AABBInLocalSpace(const Ray& localRay, float& out_dist) const;

	bool AABBInWorldSpace(const Ray& worldRay, float& out_dist, const M4& localToWorldMatrix) const;

	bool RaycastInLocalSpace(V3& out_localHit, const Ray& localRay, bool in90Degrees = true) const;
								 
	bool RaycastInWorldSpace(V3& out_worldHit, const Ray& worldRay, const M4& localToWorldMatrix, bool in90Degrees = true) const;

private:

	Vertex*					m_vertices = nullptr;
	UINT					m_vertexCount = 0;

	UINT					m_indexCount = 0;	// Count of all indices
	UINT					m_primitiveCount = 0;
	UINT					m_primitiveVertexCount = 3;
	PrimitiveTopology		m_primitive = PrimitiveTopology::Triangles;

	SubIndicesDesc**		m_arrSubIndicesDesc = nullptr;
	UINT					m_subIndecesCount = 0;

	Bounds					m_bounds = Bounds();
};

ENGINE_END
