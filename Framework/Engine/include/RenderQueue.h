#pragma once

#include "RenderRequestInput.h"

ENGINE_BEGIN
class RenderQueueStandard;
class RenderQueueTransparent;
class RenderQueueInstance;

class GraphicSystem;
class CBufferManager;
class InstanceBufferManager;

class RenderQueueLight;
class ENGINE_API RenderQueue
{
public:

	RenderQueue(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferManager);

	~RenderQueue();

	HRESULT Initialize();

public:

	bool Add(const RenderRequest& input);

	void Render(ICamera* camera);

	void Clear();

private:

	void Render_Deferred(ICamera* camera);
	void Render_Forward(ICamera* camera);

private:

	GraphicSystem*								m_graphicSystem = nullptr;
	CBufferManager*								m_CBufferManager = nullptr;

	InstanceBufferManager*						m_instanceBufferManager = nullptr;

	RenderQueueLight*							m_light = nullptr;

	RenderQueueStandard*						m_priority = nullptr;
	RenderQueueInstance*						m_priorityInstance = nullptr;

	RenderQueueStandard*						m_standard = nullptr;
	RenderQueueInstance*						m_standardInstance = nullptr;

	RenderQueueStandard*						m_alphaTest = nullptr;
	RenderQueueInstance*						m_alphaTestInstance = nullptr;

	RenderQueueTransparent*						m_transparent = nullptr;
	RenderQueueInstance*						m_transparentInstance = nullptr;

	RenderQueueTransparent*						m_overlay = nullptr;
	RenderQueueInstance*						m_overlayInstance = nullptr;
};

ENGINE_END
