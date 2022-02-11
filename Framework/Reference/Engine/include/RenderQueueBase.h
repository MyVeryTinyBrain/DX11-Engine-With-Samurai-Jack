#pragma once

#include "RenderRequestInput.h"
#include "GraphicSystem.h"
#include "CBufferManager.h"
#include "InstanceBufferManager.h"

ENGINE_BEGIN
class GraphicSystem;
class ENGINE_API RenderQueueBase abstract
{
public:

	RenderQueueBase() = default;

	virtual ~RenderQueueBase() = default;

public:

	virtual HRESULT Initialize(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferManager);

	virtual bool IsValidInput(const RenderRequest& input) const = 0;

	virtual bool AddInput(const RenderRequest& input) = 0;

	virtual void Render(ICamera* camera) = 0;

	virtual void Clear() = 0;

protected:

	GraphicSystem*			m_graphicSystem = nullptr;

	CBufferManager*			m_CBufferManager = nullptr;

	InstanceBufferManager*	m_instanceBufferManager = nullptr;
};
ENGINE_END
