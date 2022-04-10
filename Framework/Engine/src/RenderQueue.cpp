#include "EnginePCH.h"
#include "RenderQueue.h"

#include "RenderQueueStandard.h"
#include "RenderQueueTransparent.h"
#include "RenderQueueInstance.h"

#include "GraphicSystem.h"
#include "CBufferManager.h"
#include "InstanceBufferManager.h"

#include "DeferredRenderTarget.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "LightManager.h"
#include "ILight.h"

#include "RenderQueueLight.h"
#include "PostProcessing.h"

RenderQueue::RenderQueue(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferManager) :
	m_graphicSystem(graphicSystem),
	m_CBufferManager(cBufferManager),
	m_instanceBufferManager(instanceBufferManager)
{
}

RenderQueue::~RenderQueue()
{
	SafeDelete(m_light);
	SafeDelete(m_priority);
	SafeDelete(m_priorityInstance);
	SafeDelete(m_standard);
	SafeDelete(m_standardInstance);
	SafeDelete(m_alphaTest);
	SafeDelete(m_alphaTestInstance);
	SafeDelete(m_transparent);
	SafeDelete(m_transparentInstance);
	SafeDelete(m_overlay);
	SafeDelete(m_overlayInstance);
}

HRESULT RenderQueue::Initialize()
{
	HRESULT hr = S_OK;

	m_light = new RenderQueueLight();
	if (FAILED(hr = m_light->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;

	m_priority = new RenderQueueStandard(m_light);
	m_priorityInstance = new RenderQueueInstance(m_light, false);
	m_standard = new RenderQueueStandard(m_light);
	m_standardInstance = new RenderQueueInstance(m_light, false);
	m_alphaTest = new RenderQueueStandard(m_light);
	m_alphaTestInstance = new RenderQueueInstance(m_light, false);
	m_transparent = new RenderQueueTransparent(m_light);
	m_transparentInstance = new RenderQueueInstance(m_light, true);
	m_overlay = new RenderQueueTransparent(m_light);
	m_overlayInstance = new RenderQueueInstance(m_light, true);

	if (FAILED(hr = m_priority->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_priorityInstance->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_standard->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_standardInstance->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_alphaTest->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_alphaTestInstance->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_transparent->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_transparentInstance->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_overlay->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;
	if (FAILED(hr = m_overlayInstance->Initialize(m_graphicSystem, m_CBufferManager, m_instanceBufferManager)))
		return hr;

	return S_OK;
}

bool RenderQueue::Add(const RenderRequest& input)
{
	m_light->AddInput(input);

	switch (input.essential.renderGroup)
	{
		case RenderGroup::Priority:
			if (!input.essential.instance)
				return m_priority->AddInput(input);
			else
				return m_priorityInstance->AddInput(input);

		case RenderGroup::Standard:
			if (!input.essential.instance)
				return m_standard->AddInput(input);
			else
				return m_standardInstance->AddInput(input);

		case RenderGroup::AlphaTest:
			if (!input.essential.instance)
				return m_alphaTest->AddInput(input);
			else
				return m_alphaTestInstance->AddInput(input);

		case RenderGroup::Transparent:
			if (!input.essential.instance)
				return m_transparent->AddInput(input);
			else
				return m_transparentInstance->AddInput(input);

		case RenderGroup::Overlay:
			if (!input.essential.instance)
				return m_overlay->AddInput(input);
			else
				return m_overlayInstance->AddInput(input);

		default:
			return false;
	}

	return false;
}

void RenderQueue::Render(ICamera* camera)
{
	if (!camera->IsWorking())
		return;

	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	drt->ReadyToDraw(m_graphicSystem->deviceContext);

	m_light->RenderDepthes(camera);

	Render_Deferred(camera);
	Render_Forward(camera);
	Render_Result(camera);

	m_graphicSystem->RollbackRenderTarget();

	m_graphicSystem->postProcessing->DrawToScreen(drt->diffuse->srv, uint2(0, 0), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->normal->srv, uint2(100, 0), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->depth->srv, uint2(200, 0), uint2(100, 100), PostProcessing::CopyType::LinearDepth);
	m_graphicSystem->postProcessing->DrawToScreen(drt->light_Occlusion_Shadow->srv, uint2(300, 0), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->specular_Power->srv, uint2(400, 0), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->emissive->srv, uint2(500, 0), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->reflection_ReflectionBlur_ReflectMask->srv, uint2(600, 0), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->light->srv, uint2(000, 100), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->specular->srv, uint2(100, 100), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->lightBlend->srv, uint2(200, 100), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->ssao->srv, uint2(000, 200), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->ssr->srv, uint2(100, 200), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->dof->srv, uint2(200, 200), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->bloom->srv, uint2(300, 200), uint2(100, 100), PostProcessing::CopyType::Default);
	m_graphicSystem->postProcessing->DrawToScreen(drt->result->srv, uint2(000, 300), uint2(100, 100), PostProcessing::CopyType::Default);
}

void RenderQueue::Clear()
{
	m_priority->Clear();
	m_priorityInstance->Clear();
	m_standard->Clear();
	m_standardInstance->Clear();
	m_alphaTest->Clear();
	m_alphaTestInstance->Clear();
	m_transparent->Clear();
	m_transparentInstance->Clear();
	m_overlay->Clear();
	m_overlayInstance->Clear();

	m_light->Clear();
}

void RenderQueue::Render_Deferred(ICamera* camera)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	drt->SetDeferredRenderTargets(m_graphicSystem);

	m_priority->Render(camera);
	m_priorityInstance->Render(camera);
	m_standard->Render(camera);
	m_standardInstance->Render(camera);
	m_alphaTest->Render(camera);
	m_alphaTestInstance->Render(camera);

	m_light->Render(camera);

	m_graphicSystem->postProcessing->DrawToTextrue(drt->lightBlend->srv, drt->result->rtv, uint2(drt->result->width, drt->result->height), PostProcessing::CopyType::Alphatest);

	m_graphicSystem->postProcessing->PostProcess(camera, PostProcessing::Step::Deferred);
}

void RenderQueue::Render_Forward(ICamera* camera)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	drt->SetForwardRenderTargets(m_graphicSystem);

	m_transparent->Render(camera);
	m_transparentInstance->Render(camera);

	Render_Emissive(camera);

	m_graphicSystem->postProcessing->PostProcess(camera, PostProcessing::Step::After);

	drt->SetForwardRenderTargets(m_graphicSystem);

	m_overlay->Render(camera);
	m_overlayInstance->Render(camera);
}

void RenderQueue::Render_Emissive(ICamera* camera)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();

	m_graphicSystem->postProcessing->DrawToTextrue(drt->emissive->srv, drt->result->rtv, uint2(drt->result->width, drt->result->height), PostProcessing::CopyType::Alphablend);
}

void RenderQueue::Render_Result(ICamera* camera)
{
	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();

	m_graphicSystem->postProcessing->DrawToTextrue(drt->result->srv, m_graphicSystem->backBufferRenderTargetView, uint2((uint)m_graphicSystem->width, (uint)m_graphicSystem->height), PostProcessing::CopyType::Default);
}
