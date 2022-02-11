#include "EnginePCH.h"
#include "RenderQueueBase.h"

HRESULT RenderQueueBase::Initialize(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferManager)
{
    if (!graphicSystem ||
        !cBufferManager ||
        !instanceBufferManager)
        return E_FAIL;

    m_graphicSystem = graphicSystem;
    m_CBufferManager = cBufferManager;
    m_instanceBufferManager = instanceBufferManager;

    return S_OK;
}
