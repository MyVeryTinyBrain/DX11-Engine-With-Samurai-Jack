#include "EnginePCH.h"
#include "InstanceBufferManager.h"
#include "GraphicSystem.h"

InstanceBufferManager::InstanceBufferManager(GraphicSystem* graphicSystem)
{
	m_graphicSystem = graphicSystem;
}

InstanceBufferManager::~InstanceBufferManager()
{
    Release();
}

HRESULT InstanceBufferManager::Initialize()
{
    HRESULT hr = S_OK;

    if (!m_graphicSystem)
        return E_FAIL;

    if (FAILED(hr = Reserve(1)))
        return hr;

    return S_OK;
}

HRESULT InstanceBufferManager::Resize(uint count)
{
    HRESULT hr = S_OK;
    InstanceData* newArrDatas = nullptr;
    ID3D11Buffer* newBuffer = nullptr;
    auto ReleaseVars = [&]()
    {
        SafeDeleteArray(newArrDatas);
        SafeRelease(newBuffer);
    };

    newArrDatas = new InstanceData[count]{};

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = UINT(sizeof(InstanceData) * count);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = sizeof(InstanceData);

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = newArrDatas;

    newBuffer = nullptr;
    if (FAILED(hr = m_graphicSystem->device->CreateBuffer(&desc, &subresourceData, &newBuffer)))
    {
        ReleaseVars();
        return hr;
    }

    Release();
    SafeDeleteArray(newArrDatas);
    m_buffer = newBuffer;
    m_count = count;

    return S_OK;
}

HRESULT InstanceBufferManager::Reserve(uint count)
{
    if (count <= m_count)
        return S_OK;

    count = Clamp(count, 1u, UINT_MAX);

    return Resize(count);
}

HRESULT InstanceBufferManager::BeginSetDatas(Com<ID3D11DeviceContext> deviceContext, uint maxCount)
{
    HRESULT hr = S_OK;

    if (m_count < maxCount)
    {
        if (FAILED(hr = Reserve(maxCount)))
            return hr;
    }

    if (FAILED(hr = deviceContext->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedSubresource)))
        return hr;

    return S_OK;
}

void InstanceBufferManager::EndSetDatas(Com<ID3D11DeviceContext> deviceContext)
{
    deviceContext->Unmap(m_buffer, 0);
}

HRESULT InstanceBufferManager::SetData(uint index, InstanceData* pInstanceData)
{
    if (m_count <= index)
    {
        return E_FAIL;
    }

    InstanceData* arrDatas = (InstanceData*)m_mappedSubresource.pData;
    memcpy(&arrDatas[index], pInstanceData, sizeof(InstanceData));

    return S_OK;
}

Com<ID3D11Buffer> InstanceBufferManager::GetBuffer()
{
    return m_buffer;
}

void InstanceBufferManager::Release()
{
    SafeRelease(m_buffer);
}
