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

	HRESULT Resize(size_t count);

	HRESULT Reserve(size_t count);

	HRESULT BeginSetDatas(size_t maxCount);

	void EndSetDatas();

	HRESULT SetData(uint index, InstanceData* pInstanceData);

	Com<ID3D11Buffer> GetBuffer();

private:

	void Release();

private:

	GraphicSystem*				m_graphicSystem = nullptr;

	size_t						m_count = 0;

	ID3D11Buffer*				m_buffer = nullptr;

	D3D11_MAPPED_SUBRESOURCE	m_mappedSubresource= {};
};
ENGINE_END
