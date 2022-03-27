#include "EnginePCH.h"
#include "SpecificShaderVariables.h"
#include "Material.h"
#include "Shader.h"
#include "System.h"
#include "Times.h"
#include "ICamera.h"
#include "ShaderVariableInfo.h"
#include "DeferredRenderTarget.h"
#include "GraphicSystem.h"
#include "RenderTarget.h"

void SSVTime::Apply(ICamera* camera)
{
	float value = m_system->time->accumulatedSinceStartup;
	m_variableInfo->Handle->SetRawValue(&value, 0, sizeof(float));
}

void SSVUnscaledTime::Apply(ICamera* camera)
{
	float value = m_system->time->unscaledAccumulatedSinceStartup;
	m_variableInfo->Handle->SetRawValue(&value, 0, sizeof(float));
}

void SSVDeltaTime::Apply(ICamera* camera)
{
	float value = m_system->time->deltaTime;
	m_variableInfo->Handle->SetRawValue(&value, 0, sizeof(float));
}

void SSVFixedDeltaTime::Apply(ICamera* camera)
{
	float value = m_system->time->fixedDeltaTime;
	m_variableInfo->Handle->SetRawValue(&value, 0, sizeof(float));
}

void SSVGrabTexture::Apply(ICamera* camera)
{
	ID3DX11EffectShaderResourceVariable* hValue = m_variableInfo->Handle->AsShaderResource();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return;
	}

	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	m_system->graphicSystem->deviceContext->CopyResource(drt->copyTargetResult->texture.Get(), drt->result->texture.Get());
	hValue->SetResource(drt->copyTargetResult->srv.Get());

	SafeRelease(hValue);
}

void SSVDepthTexture::Apply(ICamera* camera)
{
	ID3DX11EffectShaderResourceVariable* hValue = m_variableInfo->Handle->AsShaderResource();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return;
	}

	DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
	m_system->graphicSystem->deviceContext->CopyResource(drt->copyTargetDepth->texture.Get(), drt->depth->texture.Get());
	hValue->SetResource(drt->copyTargetDepth->srv.Get());

	SafeRelease(hValue);
}
