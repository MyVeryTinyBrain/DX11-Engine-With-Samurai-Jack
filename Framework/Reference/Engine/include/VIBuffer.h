#pragma once

#include "VITypes.h"

ENGINE_BEGIN
class VI;
class ENGINE_API VIBuffer
{
protected:

	VIBuffer(
		Com<ID3D11Device> device, Com<ID3D11DeviceContext> deviceContext,
		ID3D11Buffer* vb, D3D11_BUFFER_DESC vbDesc,
		ID3D11Buffer* ib, D3D11_BUFFER_DESC ibDesc,
		VI* vi);

public:

	~VIBuffer();

public:

	HRESULT ApplyVertexBuffer(Com<ID3D11DeviceContext> deviceContext);

	HRESULT ApplyIndexBuffer(Com<ID3D11DeviceContext> deviceContext);

	HRESULT DrawOnce(Com<ID3D11DeviceContext> deviceContext);
	HRESULT DrawOnce(Com<ID3D11DeviceContext> deviceContext, uint primitiveCount);

	HRESULT DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, UINT index);
	HRESULT DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, UINT index, uint primitiveCount);

	HRESULT ApplyVertexAndInstanceBuffer(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceBuffer);

	HRESULT DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount);
	HRESULT DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount, uint primitiveCount);

public:

	inline bool IsValid() const { return m_vi && m_vb && m_ib; }

	inline VI* GetVI() const { return m_vi; }

	inline D3D11_BUFFER_DESC GetVBDesc() const { return m_vbDesc; }

	inline D3D11_BUFFER_DESC GetIBDesc() const { return m_ibDesc; }

public:

	// VI is copy on other memory
	// you can release VI

	static HRESULT CreateVIBuffer(
		Com<ID3D11Device> device, Com<ID3D11DeviceContext> deviceContext,
		const VI* vi,
		D3D11_USAGE vbUsage, UINT vbCPUAccessFlags, UINT vbMiscFlags,
		D3D11_USAGE ibUsage, UINT ibCPUAccessFlags, UINT ibMiscFlags,
		VIBuffer** out_viBuffer
	);

	// VI isn't copy on other memory
	// you can't release VI

	static HRESULT CreateVIBufferNocopy(
		Com<ID3D11Device> device, Com<ID3D11DeviceContext> deviceContext,
		VI** ppVI,
		D3D11_USAGE vbUsage, UINT vbCPUAccessFlags, UINT vbMiscFlags,
		D3D11_USAGE ibUsage, UINT ibCPUAccessFlags, UINT ibMiscFlags,
		VIBuffer** out_viBuffer
	);

	VIBuffer* Copy() const;

private:

	static HRESULT CreateVertexBuffer(
		Com<ID3D11Device> device,
		const Vertex* vertices, UINT vertexCount,
		D3D11_USAGE boneMatricesUsage, UINT CPUAccessFlags, UINT miscFlags, ID3D11Buffer** out_buffer, 
		D3D11_BUFFER_DESC* out_desc);

	static HRESULT CreateIndexBuffer(
		Com<ID3D11Device> device,
		const Index* indices, UINT indexCount,
		D3D11_USAGE boneMatricesUsage, UINT CPUAccessFlags, UINT miscFlags, ID3D11Buffer** out_buffer, 
		D3D11_BUFFER_DESC* out_desc);

public:

	// Update vertex buffer using VI's data.
	HRESULT UpdateVertexBuffer();

	// Update index buffer using VI's data.
	HRESULT UpdateIndexBuffer();

	// Use VI's data to destroy vertex buffer and regenerate it.
	HRESULT RegenerateVertexBuffer();

	// Use VI's data to destroy index buffer and regenerate it.
	HRESULT RegenerateIndexBuffer();

public:

	bool RaycastInLocalSpace(V3& localHit, const Ray& localRay, bool in90Degrees = true) const;

	bool RaycastInWorldSpace(V3& worldHit, const Ray& worldRay, const M4& localToWorldMatrix, bool in90Degrees = true) const;

protected:

	Com<ID3D11Device>			m_device;
	Com<ID3D11DeviceContext>	m_deviceContext;

	ID3D11Buffer*				m_vb = nullptr;
	D3D11_BUFFER_DESC			m_vbDesc = {};

	ID3D11Buffer*				m_ib = nullptr;
	D3D11_BUFFER_DESC			m_ibDesc = {};

	VI*							m_vi = nullptr;
};

ENGINE_END
