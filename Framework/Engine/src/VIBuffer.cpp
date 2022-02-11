#include "EnginePCH.h"
#include "VIBuffer.h"
#include "VI.h"
#include "SubIndicesDesc.h"

VIBuffer::VIBuffer(
	Com<ID3D11Device> device, Com<ID3D11DeviceContext> deviceContext, 
	ID3D11Buffer* vb, D3D11_BUFFER_DESC vbDesc, 
	ID3D11Buffer* ib, D3D11_BUFFER_DESC ibDesc, 
	VI* vi)
{
	m_device = device;
	m_deviceContext = deviceContext;
	m_vb = vb;
	m_vbDesc = vbDesc;
	m_ib = ib;
	m_ibDesc = ibDesc;
	m_vi = vi;
}

VIBuffer::~VIBuffer()
{
	SafeRelease(m_vb);
	SafeRelease(m_ib);
	SafeDelete(m_vi);
}

HRESULT VIBuffer::ApplyVertexBuffer(Com<ID3D11DeviceContext> deviceContext)
{
	if (!m_vb || !m_vi)
		return E_FAIL;

	XM_CONSTEXPR UINT stride = sizeof(Vertex);
	XM_CONSTEXPR UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);

	return S_OK;
}

HRESULT VIBuffer::ApplyIndexBuffer(Com<ID3D11DeviceContext> deviceContext)
{
	if (!m_ib || !m_vi)
		return E_FAIL;

	deviceContext->IASetIndexBuffer(m_ib, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_vi->GetPrimitiveTopologyAsD3DType());

	return S_OK;
}

HRESULT VIBuffer::DrawOnce(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return E_FAIL;

	if (!m_vi)
		return E_FAIL;

	deviceContext->DrawIndexed(m_vi->GetIndexCount(), 0, 0);

	return S_OK;
}

HRESULT VIBuffer::DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, UINT index)
{
	if (!deviceContext)
		return E_FAIL;

	if (!m_vi)
		return E_FAIL;

	HRESULT hr = S_OK;

	UINT start = 0;
	if(FAILED(hr = m_vi->GetStartOfSubIndices(index, &start)))
		return hr;

	SubIndicesDesc* desc = m_vi->GetSubIndicesDesc(index);
	if (!desc)
		return E_FAIL;

	deviceContext->DrawIndexed(desc->Count, start, 0);

	return S_OK;
}

HRESULT VIBuffer::ApplyVertexAndInstanceBuffer(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceBuffer)
{
	if (!m_vb || !m_vi || !instanceBuffer)
		return E_FAIL;

	ID3D11Buffer* buffers[2] = { m_vb, instanceBuffer.Get() };
	XM_CONSTEXPR UINT strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
	XM_CONSTEXPR UINT offsets[2] = { 0, 0 };

	deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);

	return S_OK;
}

HRESULT VIBuffer::DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, size_t subMeshIndex, uint instanceCount)
{
	if (!deviceContext)
		return E_FAIL;

	if (!m_vi)
		return E_FAIL;

	HRESULT hr = S_OK;

	UINT start = 0;
	if (FAILED(hr = m_vi->GetStartOfSubIndices((UINT)subMeshIndex, &start)))
		return hr;

	SubIndicesDesc* desc = m_vi->GetSubIndicesDesc((UINT)subMeshIndex);
	if (!desc)
		return E_FAIL;

	deviceContext->DrawIndexedInstanced(desc->Count, instanceCount, start, 0, 0);

	return S_OK;
}

HRESULT VIBuffer::CreateVIBuffer(
	Com<ID3D11Device> device, Com<ID3D11DeviceContext> deviceContext,
	const VI* vi, 
	D3D11_USAGE vbUsage, UINT vbCPUAccessFlags, UINT vbMiscFlags, 
	D3D11_USAGE ibUsage, UINT ibCPUAccessFlags, UINT ibMiscFlags, 
	VIBuffer** out_viBuffer)
{
	if (!device || !deviceContext || !vi || !out_viBuffer)
		return E_FAIL;

	if (!vi->IsValid())
		return E_FAIL;

	VI* viCpy = new VI(*vi);

	HRESULT hr = S_OK;

	if (FAILED(hr = CreateVIBufferNocopy(
		device, deviceContext,
		&viCpy,
		vbUsage, vbCPUAccessFlags, vbMiscFlags,
		ibUsage, ibCPUAccessFlags, ibMiscFlags,
		out_viBuffer)))
		return hr;

	return S_OK;
}

HRESULT VIBuffer::CreateVIBufferNocopy(Com<ID3D11Device> device, Com<ID3D11DeviceContext> deviceContext, VI** ppVI, D3D11_USAGE vbUsage, UINT vbCPUAccessFlags, UINT vbMiscFlags, D3D11_USAGE ibUsage, UINT ibCPUAccessFlags, UINT ibMiscFlags, VIBuffer** out_viBuffer)
{
	if (!device || !deviceContext || !ppVI || !*ppVI || !out_viBuffer)
		return E_FAIL;

	ID3D11Buffer* vb = nullptr;
	ID3D11Buffer* ib = nullptr;
	D3D11_BUFFER_DESC vbDesc = {};
	D3D11_BUFFER_DESC ibDesc = {};

	auto ReleaseVars = [&]()
	{
		SafeDelete(*ppVI);
		SafeRelease(vb);
		SafeRelease(ib);
	};

	if (!(*ppVI)->IsValid())
	{
		ReleaseVars();
		return E_FAIL;
	}

	HRESULT hr = S_OK;

	IndicesStream indicesStream;
	if (FAILED(hr = (*ppVI)->CreateIndicesStream(&indicesStream)))
	{
		ReleaseVars();
		return hr;
	}

	if (FAILED(hr = CreateVertexBuffer(device, (*ppVI)->GetVertices(), (*ppVI)->GetVertexCount(), vbUsage, vbCPUAccessFlags, vbMiscFlags, &vb, &vbDesc)))
	{
		ReleaseVars();
		return hr;
	}

	if (FAILED(hr = CreateIndexBuffer(device, indicesStream.Indices, (*ppVI)->GetIndexCount(), ibUsage, ibCPUAccessFlags, ibMiscFlags, &ib, &ibDesc)))
	{
		ReleaseVars();
		return hr;
	}

	VIBuffer* viBuffer = new VIBuffer(device, deviceContext, vb, vbDesc, ib, ibDesc, (*ppVI));
	if (!viBuffer)
	{
		ReleaseVars();
		return E_FAIL;
	}

	*out_viBuffer = viBuffer;

	return S_OK;
}

VIBuffer* VIBuffer::Copy() const
{
	VIBuffer* viBuffer = nullptr;

	if(FAILED(CreateVIBuffer(
		m_device, m_deviceContext,
		m_vi,
		m_vbDesc.Usage, m_vbDesc.CPUAccessFlags, m_vbDesc.MiscFlags,
		m_ibDesc.Usage, m_ibDesc.CPUAccessFlags, m_ibDesc.MiscFlags,
		&viBuffer)))
		return nullptr;

	return viBuffer;
}

HRESULT VIBuffer::CreateVertexBuffer(
	Com<ID3D11Device> device, 
	const Vertex* vertices, UINT vertexCount,
	D3D11_USAGE boneMatricesUsage, UINT CPUAccessFlags, UINT miscFlags, 
	ID3D11Buffer** out_buffer, D3D11_BUFFER_DESC* out_desc)
{
	if (!device || !vertices || !out_buffer || !out_desc)
		return E_FAIL;

	XM_CONSTEXPR UINT vertexSize = sizeof(Vertex);
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = vertexSize * vertexCount;
	desc.Usage = boneMatricesUsage;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = CPUAccessFlags;
	desc.MiscFlags = miscFlags;
	desc.StructureByteStride = vertexSize;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices;

	HRESULT hr = S_OK;

	if (FAILED(hr = device->CreateBuffer(&desc, &data, out_buffer)))
		return hr;

	*out_desc = desc;

	return S_OK;
}

HRESULT VIBuffer::CreateIndexBuffer(
	Com<ID3D11Device> device, 
	const Index* indices, UINT indexCount,
	D3D11_USAGE boneMatricesUsage, UINT CPUAccessFlags, UINT miscFlags, 
	ID3D11Buffer** out_buffer, D3D11_BUFFER_DESC* out_desc)
{
	if (!device || !indices || !out_buffer || !out_desc)
		return E_FAIL;

	UINT indexSize = sizeof(Index);
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = indexSize * indexCount;
	desc.Usage = boneMatricesUsage;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = CPUAccessFlags;
	desc.MiscFlags = miscFlags;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indices;

	HRESULT hr = S_OK;

	if (FAILED(hr = device->CreateBuffer(&desc, &data, out_buffer)))
		return hr;

	*out_desc = desc;

	return S_OK;
}

HRESULT VIBuffer::UpdateVertexBuffer()
{
	if (!m_deviceContext || !m_vb || !m_vi)
		return E_FAIL;

	if (m_vbDesc.Usage != D3D11_USAGE_DYNAMIC || !(m_vbDesc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE))
		return E_FAIL;

	if (!m_vi->GetVertices())
		return E_FAIL;

	// 정점의 갯수가 생성된 정점버퍼와 다릅니다.
	// 대신 RegenerateVertexBuffer 를 사용해야 합니다.
	if (m_vi->GetVertexCount() * sizeof(Vertex) != m_vbDesc.ByteWidth)
		return E_FAIL;

	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	if (FAILED(hr = m_deviceContext->Map(m_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		return hr;

	Vertex* vertices = (Vertex*)mappedSubresource.pData;
	memcpy(vertices, m_vi->GetVertices(), sizeof(Vertex) * m_vi->GetVertexCount());

	m_deviceContext->Unmap(m_vb, 0);

	return S_OK;
}

HRESULT VIBuffer::UpdateIndexBuffer()
{
	if (!m_deviceContext || !m_ib || !m_vi)
		return E_FAIL;

	if (m_ibDesc.Usage != D3D11_USAGE_DYNAMIC || !(m_ibDesc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE))
		return E_FAIL;

	// 인덱스의 갯수가 생성된 인덱스버퍼와 다릅니다.
	// 대신 RegenerateIndexBuffer 를 사용해야 합니다.
	if (m_vi->GetIndexCount() * sizeof(Index) != m_ibDesc.ByteWidth)
		return E_FAIL;

	HRESULT hr = S_OK;

	IndicesStream indicesStream;
	if (FAILED(hr = m_vi->CreateIndicesStream(&indicesStream)))
		return hr;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	if (FAILED(hr = m_deviceContext->Map(m_ib, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		return hr;

	Index* indices = (Index*)mappedSubresource.pData;
	memcpy(indices, indicesStream.Indices, sizeof(Index) * m_vi->GetIndexCount());

	m_deviceContext->Unmap(m_ib, 0);

	return S_OK;
}

HRESULT VIBuffer::RegenerateVertexBuffer()
{
	if (!m_device || !m_vi)
		return E_FAIL;

	HRESULT hr = S_OK;

	XM_CONSTEXPR UINT vertexSize = sizeof(Vertex);
	D3D11_BUFFER_DESC desc = m_vbDesc;
	desc.ByteWidth = vertexSize * m_vi->GetVertexCount();

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = m_vi->GetVertices();

	if (!data.pSysMem)
		return E_FAIL;

	ID3D11Buffer* vb = nullptr;

	if (FAILED(hr = m_device->CreateBuffer(&desc, &data, &vb)))
		return hr;

	SafeRelease(m_vb);

	m_vb = vb;
	m_vbDesc = desc;

	return S_OK;
}

HRESULT VIBuffer::RegenerateIndexBuffer()
{
	if (!m_device || !m_vi)
		return E_FAIL;

	HRESULT hr = S_OK;

	IndicesStream indicesStream;
	if (FAILED(hr = m_vi->CreateIndicesStream(&indicesStream)))
		return hr;

	UINT indexSize = sizeof(Index);
	D3D11_BUFFER_DESC desc = m_ibDesc;
	desc.ByteWidth = indexSize * m_vi->GetIndexCount();

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indicesStream.Indices;

	if (!data.pSysMem)
		return E_FAIL;

	ID3D11Buffer* ib = nullptr;

	if (FAILED(hr = m_device->CreateBuffer(&desc, &data, &ib)))
		return hr;

	SafeRelease(m_ib);

	m_ib = ib;
	m_ibDesc = desc;

	return S_OK;
}

bool VIBuffer::RaycastInLocalSpace(V3& localHit, const Ray& localRay, bool in90Degrees) const
{
	if (!m_vi)
		return false;

	return m_vi->RaycastInLocalSpace(localHit, localRay, in90Degrees);
}

bool VIBuffer::RaycastInWorldSpace(V3& worldHit, const Ray& worldRay, const M4& localToWorldMatrix, bool in90Degrees) const
{
	if (!m_vi)
		return false;

	return m_vi->RaycastInWorldSpace(worldHit, worldRay, localToWorldMatrix, in90Degrees);
}
