#include "EnginePCH.h"
#include "DeferredScreenRender.h"
#include "GraphicSystem.h"
#include "PrimitiveVI.h"
#include "VIBuffer.h"
#include "VI.h"
#include "CompiledShaderDesc.h"
#include "TechniqueDesc.h"
#include "PassDesc.h"
#include "RenderTarget.h"
#include "Light.h"
#include "CBufferManager.h"
#include "InstanceBufferManager.h"
#include "RenderRequestInput.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

DeferredScreenRender::DeferredScreenRender(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferMaanger)
{
	m_graphicSystem = graphicSystem;
	m_CBufferManager = cBufferManager;
	m_instanceBufferManager = instanceBufferMaanger;
}

DeferredScreenRender::~DeferredScreenRender()
{
	SafeDelete(m_quad);
	SafeDelete(m_normalizedQuad);
	SafeDelete(m_shaderScreen);
}

HRESULT DeferredScreenRender::Initialize()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = SetupQuads()))
		return hr;

	if (FAILED(hr = SetupShaders()))
		return hr;

	return S_OK;
}

void DeferredScreenRender::DrawTextureInClient(Com<ID3D11ShaderResourceView> in_srv, uint x, uint y, uint width, uint height, Blend blend)
{
	RECT rect = {};
	GetClientRect(m_graphicSystem->windowHandle, &rect);
	uint cw = rect.right;
	uint ch = rect.bottom;
	float dw = float(cw) * 0.5f;
	float dh = float(ch) * 0.5f * -1.0f;

	VI* vi = m_quad->GetVI();
	Vertex* vertices = vi->GetVertices();
	vertices[0].position = V3(float(x) / dw - 1.0f, float(y) / dh + 1.0f, 0.0f);
	vertices[1].position = V3(float(x + width) / dw - 1.0f, float(y) / dh + 1.0f, 0.0f);
	vertices[2].position = V3(float(x + width) / dw - 1.0f, float(y + height) / dh + 1.0f, 0.0f);
	vertices[3].position = V3(float(x) / dw - 1.0f, float(y + height) / dh + 1.0f, 0.0f);
	m_quad->UpdateVertexBuffer();

	uint techniqueIndex = 0;
	uint passIndex = 0;
	switch (blend)
	{
		case Engine::DeferredScreenRender::Blend::Blend:
			m_shaderScreen->GetPassIndexByName("Blend", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::AlphaTest:
			m_shaderScreen->GetPassIndexByName("AlphaTest", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::PerspectiveDepthVisualize:
			m_shaderScreen->GetPassIndexByName("PerspectiveDepthVisualize", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::OrthoDepthVisualize:
			m_shaderScreen->GetPassIndexByName("OrthoDepthVisualize", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::None:
			m_shaderScreen->GetPassIndexByName("NonBlend", techniqueIndex, passIndex);
			break;
	}

	// Setup Shader Variables
	m_shaderScreen->SetTexture("_Diffuse", in_srv);
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, passIndex);

	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_quad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_quad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_quad->DrawOnce(m_graphicSystem->deviceContext);
}

void DeferredScreenRender::DeferredDrawTexture(Com<ID3D11ShaderResourceView> in, Com<ID3D11RenderTargetView> out, Blend blend)
{
	// Set render target view array
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = out.Get();
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	// Select pass index
	uint techniqueIndex = 0;
	uint passIndex = 0;
	switch (blend)
	{
		case Engine::DeferredScreenRender::Blend::Blend:
			m_shaderScreen->GetPassIndexByName("Blend", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::AlphaTest:
			m_shaderScreen->GetPassIndexByName("AlphaTest", techniqueIndex, passIndex);
			break;
		case Engine::DeferredScreenRender::Blend::None:
			m_shaderScreen->GetPassIndexByName("NonBlend", techniqueIndex, passIndex);
			break;
	}

	// Setup Shader Variables
	m_shaderScreen->SetTexture("_Diffuse", in);

	// Draw
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, passIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Rollback to main render target
	m_graphicSystem->RollbackRenderTarget();
}

void DeferredScreenRender::DeferredDrawBlurTexture(
	uint2 originalViewport, uint2 outViewport, uint blur,
	Com<ID3D11ShaderResourceView> in, Com<ID3D11RenderTargetView> out)
{
	// Change Viewport
	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	iGraphicSystem->SetViewport(outViewport.x, outViewport.y);

	// Set render target view array
	ID3D11RenderTargetView* arrRTV[8] = {};
	arrRTV[0] = out.Get();
	//m_graphicSystem->SetRenderTargets(1, arrRTV);
	m_graphicSystem->SetRenderTargetsWithDepthStencil(1, arrRTV, nullptr);

	// Select pass index
	uint techniqueIndex = 0;
	uint horizontalBlurPassIndex = 0;
	uint verticalBlurPassIndex = 0;
	m_shaderScreen->GetPassIndexByName("HorizontalBlur", techniqueIndex, horizontalBlurPassIndex);
	m_shaderScreen->GetPassIndexByName("VerticalBlur", techniqueIndex, verticalBlurPassIndex);

	// Setup Shader Variables
	m_shaderScreen->SetTexture("_Diffuse", in);
	m_shaderScreen->SetRawValue("_Blur", &blur, sizeof(blur));

	// Draw by horizontal blur
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, horizontalBlurPassIndex);
	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, horizontalBlurPassIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Draw by vertical blur
	m_shaderScreen->SetInputLayout(m_graphicSystem->deviceContext, techniqueIndex, verticalBlurPassIndex);
	m_shaderScreen->ApplyPass(m_graphicSystem->deviceContext, techniqueIndex, verticalBlurPassIndex);
	m_normalizedQuad->ApplyVertexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->ApplyIndexBuffer(m_graphicSystem->deviceContext);
	m_normalizedQuad->DrawOnce(m_graphicSystem->deviceContext);

	// Rollback Viewport
	iGraphicSystem->SetViewport(originalViewport.x, originalViewport.y);
}

HRESULT DeferredScreenRender::SetupQuads()
{
	// Setup Quad
	{
		VI* vi = PrimitiveVI::CreateQuad();
		HRESULT hr = VIBuffer::CreateVIBufferNocopy(
			m_graphicSystem->device, m_graphicSystem->deviceContext,
			&vi,
			D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
			D3D11_USAGE_IMMUTABLE, 0, 0,
			&m_quad);
		if (FAILED(hr))
			RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to create VIBuffer(Quad)");
	}

	// Setup Normalized Quad
	{
		VI* vi = PrimitiveVI::CreateNormalizedQuad();
		HRESULT hr = VIBuffer::CreateVIBufferNocopy(
			m_graphicSystem->device, m_graphicSystem->deviceContext,
			&vi,
			D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0,
			D3D11_USAGE_IMMUTABLE, 0, 0,
			&m_normalizedQuad);
		if (FAILED(hr))
			RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to create VIBuffer(Normalized Quad)");
	}

	return S_OK;
}

HRESULT DeferredScreenRender::SetupShaders()
{
	m_shaderScreen = CompiledShaderDesc::CreateCompiledShaderFromFile(m_graphicSystem->device, TEXT("../Shader/DeferredScreen.fx"));
	if (!m_shaderScreen)
		RETURN_E_FAIL_ERROR_MESSAGE("ScreenRender::Initialize::Failed to load ../Shader/DeferredScreen.fx");

	return S_OK;
}
