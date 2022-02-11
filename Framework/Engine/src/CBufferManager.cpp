#include "EnginePCH.h"
#include "CBufferManager.h"
#include "GraphicSystem.h"

CBufferManager::CBufferManager(GraphicSystem* graphicSystem)
{
	m_graphicSystem = graphicSystem;
}

CBufferManager::~CBufferManager() 
{
    SafeRelease(m_worldMatrixBuffer);
    SafeRelease(m_cameraBuffer);
    SafeRelease(m_boneMatricesUsageBuffer);
    SafeRelease(m_boneMatricesBuffer);
}

HRESULT CBufferManager::Initialize()
{
    HRESULT hr = S_OK;

    if (!m_graphicSystem)
        return E_FAIL;

    if (FAILED(hr = CreateBuffer(sizeof(WorldMatrixCBuffer), &m_worldMatrixBuffer)))
        return hr;

    if (FAILED(hr = CreateBuffer(sizeof(CameraCBuffer), &m_cameraBuffer)))
        return hr;

    if (FAILED(hr = CreateBuffer(sizeof(BoneMatricesUsageCBuffer), &m_boneMatricesUsageBuffer)))
        return hr;

    if (FAILED(hr = CreateBuffer(sizeof(BoneMatricesCBuffer), &m_boneMatricesBuffer)))
        return hr;

    return S_OK;
}

HRESULT CBufferManager::BeginApply(Com<ID3DX11Effect> effect)
{
    if (!effect)
        return E_FAIL;

    m_settedEffect = effect;

    return S_OK;
}

void CBufferManager::EndApply()
{
    m_settedEffect = nullptr;
}

void CBufferManager::ApplyWorldMatrixBuffer(const M4& world)
{
    D3D11_MAPPED_SUBRESOURCE resource = {};
    if (FAILED(MapBuffer(m_worldMatrixBuffer, &resource)))
        return;
    
    if (WorldMatrixCBuffer* buffer = (WorldMatrixCBuffer*)resource.pData)
    {
        // Transpose Matrix
        buffer->_WorldMatrix = world.transposed;
    }

    if (FAILED(UnmapBuffer(m_worldMatrixBuffer)))
        return;

    if (FAILED(SetCBuffer(m_worldMatrixBuffer, m_settedEffect, "WorldMatrixCBuffer")))
        return;
}

void CBufferManager::ApplyCameraBuffer(const V3& position, const V3& direction, const M4& view, const M4& proj, float Near, float Far)
{
    D3D11_MAPPED_SUBRESOURCE resource = {};
    if (FAILED(MapBuffer(m_cameraBuffer, &resource)))
        return;

    if (CameraCBuffer* buffer = (CameraCBuffer*)resource.pData)
    {
        buffer->_ViewPosition = position;
        buffer->_ViewDirection = direction;

        // Transpose Matrix
        buffer->_ViewMatrix = view.transposed;
        buffer->_ProjectionMatrix = proj.transposed;

        buffer->_Near = Near;
        buffer->_Far = Far;
    }

    if (FAILED(UnmapBuffer(m_cameraBuffer)))
        return;

    if (FAILED(SetCBuffer(m_cameraBuffer, m_settedEffect, "CameraCBuffer")))
        return;
}

void CBufferManager::ApplyBoneMatricesUsageBuffer(bool use)
{
    D3D11_MAPPED_SUBRESOURCE resource = {};
    if (FAILED(MapBuffer(m_boneMatricesUsageBuffer, &resource)))
        return;

    if (BoneMatricesUsageCBuffer* buffer = (BoneMatricesUsageCBuffer*)resource.pData)
    {
        if (use)
            buffer->_BoneMatricesUsage = uint4(1, 1, 1, 1);
        else
            buffer->_BoneMatricesUsage = uint4(0, 0, 0, 0);
    }

    if (FAILED(UnmapBuffer(m_boneMatricesUsageBuffer)))
        return;

    if (FAILED(SetCBuffer(m_boneMatricesUsageBuffer, m_settedEffect, "BoneMatricesUsageCBuffer")))
        return;
}

BoneMatricesCBuffer* CBufferManager::GetBoneMatricesBufferData()
{
    return &m_boneMatricesBufferData;
}

void CBufferManager::ApplyBoneMatrices()
{
    D3D11_MAPPED_SUBRESOURCE resource = {};
    if (FAILED(MapBuffer(m_boneMatricesBuffer, &resource)))
        return;

    if (BoneMatricesCBuffer* buffer = (BoneMatricesCBuffer*)resource.pData)
    {
        memcpy(buffer, &m_boneMatricesBufferData, sizeof(BoneMatricesCBuffer));
    }

    if (FAILED(UnmapBuffer(m_boneMatricesBuffer)))
        return;

    if (FAILED(SetCBuffer(m_boneMatricesBuffer, m_settedEffect, "BoneMatricesCBuffer")))
        return;
}

HRESULT CBufferManager::CreateBuffer(size_t size, ID3D11Buffer** out_buffer)
{
    if (!out_buffer)
        return E_FAIL;

    D3D11_BUFFER_DESC cbufferDesc = {};
    cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbufferDesc.ByteWidth = UINT(size);
    cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbufferDesc.MiscFlags = 0;
    cbufferDesc.StructureByteStride = 0;

    HRESULT hr = S_OK;
    
    if (FAILED(hr = m_graphicSystem->device->CreateBuffer(&cbufferDesc, NULL, out_buffer)))
        return hr;

    return S_OK;
}

HRESULT CBufferManager::MapBuffer(ID3D11Buffer* buffer, D3D11_MAPPED_SUBRESOURCE* out_mappedSubresource)
{
    if (!buffer)
        return E_FAIL;

    auto context = m_graphicSystem->deviceContext;

    HRESULT hr = S_OK;

    return context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, out_mappedSubresource);
}

HRESULT CBufferManager::UnmapBuffer(ID3D11Buffer* buffer)
{
    if (!buffer)
        return E_FAIL;

    auto context = m_graphicSystem->deviceContext;

    context->Unmap(buffer, 0);

    return S_OK;
}

HRESULT CBufferManager::SetCBuffer(ID3D11Buffer* buffer, Com<ID3DX11Effect> effect, const char* CBufferName)
{
    if (!effect)
        return E_FAIL;

    auto hBuffer = effect->GetConstantBufferByName(CBufferName);
    
    if (hBuffer->IsValid())
    {
        return hBuffer->SetConstantBuffer(buffer);
    }
    else
    {
        return E_FAIL;
    }

    return E_FAIL;
}
