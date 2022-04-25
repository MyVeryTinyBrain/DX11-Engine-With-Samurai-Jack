#pragma once

#include "VITypes.h"

ENGINE_BEGIN
class GraphicSystem;
class ENGINE_API InstanceBufferManager
{
public:

	InstanceBufferManager(GraphicSystem* graphicSystem);

	~InstanceBufferManager();

	HRESULT Initialize();

public:

	HRESULT Resize(uint count);

	HRESULT Reserve(uint count);

	HRESULT BeginSetDatas(Com<ID3D11DeviceContext> deviceContext, uint maxCount);

	void EndSetDatas(Com<ID3D11DeviceContext> deviceContext);

	HRESULT SetData(uint index, InstanceData* pInstanceData);

	Com<ID3D11Buffer> GetBuffer();

private:

	void Release();

private:

	GraphicSystem*				m_graphicSystem = nullptr;

	uint						m_count = 0;

	ID3D11Buffer*				m_buffer = nullptr;

	D3D11_MAPPED_SUBRESOURCE	m_mappedSubresource= {};
};
ENGINE_END
