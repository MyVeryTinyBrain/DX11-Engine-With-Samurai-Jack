#include "EnginePCH.h"
#include "GraphicSystem.h"
#include "CBufferManager.h"
#include "InstanceBufferManager.h"
#include "RenderQueue.h"
#include "CameraManager.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "PostProcessing.h"
#include "LightManager.h"
#include "DxUtility.h"
#include "DeferredRenderTarget.h"

GraphicSystem::~GraphicSystem()
{
	Release();
}

bool GraphicSystem::Initialize(HWND hWnd, unsigned int width, unsigned int height, unsigned int refreshPerSec, bool vsync, bool fullScreen)
{
	m_hWnd = hWnd;
	m_width = width;
	m_height = height;
	m_refreshPerSec = refreshPerSec;
	m_vsync = vsync;
	m_fullScreen = fullScreen;

	unsigned int flag = 0;

#ifdef _DEBUG
	flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGIAdapter* adapter = DxUtility::GetBestAdapter();
	D3D_FEATURE_LEVEL feature;
	if (!adapter)
	{
		if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flag, nullptr, 0, D3D11_SDK_VERSION, &m_device, &feature, &m_deviceContext)))
			return false;
	}
	else
	{
		if (FAILED(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, 0, flag, nullptr, 0, D3D11_SDK_VERSION, &m_device, &feature, &m_deviceContext)))
		{
			SafeRelease(adapter);
			return false;
		}
	}
	SafeRelease(adapter);
	
	if (FAILED(DxUtility::CreateSwapChain(m_device, hWnd, width, height, refreshPerSec, vsync, fullScreen, &m_swapChain)))
		return false;

	if (FAILED(DxUtility::CreateBackBufferView(m_device, m_swapChain, &m_rtv)))
		return false;

	if (FAILED(DxUtility::CreateDepthStencilView(m_device, width, height, &m_dsv)))
		return false;

	m_deviceContext->OMSetRenderTargets(1, &m_rtv, m_dsv);

	DxUtility::SetViewport(m_deviceContext, width, height);

	m_CBufferManager = new CBufferManager(this);
	if (FAILED(m_CBufferManager->Initialize()))
		return false;

	m_instanceBufferManager = new InstanceBufferManager(this);
	if (FAILED(m_instanceBufferManager->Initialize()))
		return false;

	m_renderQueue = new RenderQueue(this, m_CBufferManager, m_instanceBufferManager);
	if (FAILED(m_renderQueue->Initialize()))
		return false;

	m_cameraManager = new CameraManager;

	m_postProcessing = new PostProcessing(this, m_CBufferManager, m_instanceBufferManager);
	if (FAILED(m_postProcessing->Initialize()))
		return false;

	m_lightManager = new LightManager;

	return true;
}

bool GraphicSystem::Release()
{
	if (m_swapChain)
	{
		// 종료 전에 윈도우 모드가 아니라면 스왑체인 해제시 예외가 발생합니다.
		m_swapChain->SetFullscreenState(false, nullptr);
	}

	SafeDelete(m_lightManager);
	SafeDelete(m_postProcessing);
	SafeDelete(m_cameraManager);
	SafeDelete(m_renderQueue);
	SafeDelete(m_instanceBufferManager);
	SafeDelete(m_CBufferManager);
	SafeRelease(m_dsv);
	SafeRelease(m_rtv);
	SafeRelease(m_swapChain);
	SafeRelease(m_deviceContext);
	SafeRelease(m_device);
	return true;
}

void GraphicSystem::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
				break;;

			RECT rect;
			::GetClientRect(hWnd, &rect);
			SetSwapchainResolution((uint)(rect.right - rect.left), (uint)(rect.bottom - rect.top));
		}
		break;
	}
}

void GraphicSystem::Render()
{
	DxUtility::ClearRTV(m_deviceContext, m_clearColor, m_rtv);
	DxUtility::ClearDSV(m_deviceContext, m_dsv);

	Color clearColorLerped = Color::Lerp(m_clearColor, Color::white(), 1 - m_clearColor.a);
	
	for (uint i = 0; i < m_cameraManager->GetCameraCount(); ++i)
	{
		ICamera* camera = m_cameraManager->GetCamera(i);
		if (!camera->IsWorking())
			continue;

		DeferredRenderTarget* drt = camera->GetDeferredRenderTarget();
		bool hasRenderTexture2D = camera->HasRenderTexture2D();

		DxUtility::ClearRTV(m_deviceContext, m_clearColor, drt->result->rtv.Get());
		DxUtility::ClearDSV(m_deviceContext, drt->depthStencil->dsv.Get());

		m_renderQueue->Render(camera);

		if (!hasRenderTexture2D)
		{
			m_postProcessing->DrawToTextrue(drt->result->srv, m_rtv, uint2(m_width, m_height), PostProcessing::CopyType::Default);
		}
		else
		{
			m_postProcessing->DrawToTextrue(drt->result->srv, camera->GetRenderTarget()->rtv.Get(), uint2(camera->GetRenderTarget()->width, camera->GetRenderTarget()->height), PostProcessing::CopyType::Default);
		}
	}

	m_renderQueue->Clear();

	DxUtility::SetViewport(m_deviceContext, m_width, m_height);
	DxUtility::SetRenderTargets(m_deviceContext, 1, &m_rtv, m_dsv);
}

bool GraphicSystem::Present()
{
	if (!m_swapChain)
		return false;

	if (m_vsync)
		m_swapChain->Present(1, 0);
	else
		m_swapChain->Present(0, 0);

	return true;
}

Com<ID3D11Device> GraphicSystem::GetDevice() const
{
	return Com<ID3D11Device>(m_device);
}

Com<ID3D11DeviceContext> GraphicSystem::GetDeviceContext() const
{
	return Com<ID3D11DeviceContext>(m_deviceContext);
}

RenderQueue* GraphicSystem::GetRenderQueue() const
{
	return m_renderQueue;
}

CameraManager* GraphicSystem::GetCameraManager() const
{
	return m_cameraManager;
}

PostProcessing* GraphicSystem::GetPostProcessing() const
{
	return m_postProcessing;
}

LightManager* GraphicSystem::GetLightManager() const
{
	return m_lightManager;
}

HWND GraphicSystem::GetWindowHandle() const
{
	return m_hWnd;
}

float GraphicSystem::GetWidth() const
{
	return float(m_width);
}

float GraphicSystem::GetHeight() const
{
	return float(m_height);
}

float GraphicSystem::GetAspect() const
{
	return float(m_width) / float(m_height);
}

bool GraphicSystem::SetSwapchainResolution(uint width, uint height)
{
	SafeRelease(m_rtv);
	SafeRelease(m_dsv);

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	if (FAILED(m_swapChain->GetDesc(&swapChainDesc)))
		return false;

	HRESULT hrSwapChainResize = m_swapChain->ResizeBuffers(1, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);

	ID3D11RenderTargetView* backBufferRTV = nullptr;
	ID3D11DepthStencilView* depthStencilRTV = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(depthStencilRTV);
		SafeRelease(backBufferRTV);
	};

	if (FAILED(DxUtility::CreateBackBufferView(m_device, m_swapChain, &backBufferRTV)))
	{
		ReleaseVars();
		RETURN_FALSE_ERROR_MESSAGE("GraphicSystem::SetResolution::CreateBackBufferView");
	}

	if (FAILED(DxUtility::CreateDepthStencilView(m_device, width, height, &depthStencilRTV)))
	{
		ReleaseVars();
		RETURN_FALSE_ERROR_MESSAGE("GraphicSystem::SetResolution::CreateDepthStencilBufferView");
	}

	DxUtility::SetViewport(m_deviceContext, width, height);

	m_deviceContext->OMSetRenderTargets(1, &backBufferRTV, depthStencilRTV);

	{
		SafeRelease(m_dsv);
		SafeRelease(m_rtv);

		m_rtv = backBufferRTV;
		m_dsv = depthStencilRTV;
	}

	m_width = width;
	m_height = height;
	float fWidth = float(width);
	float fHeight = float(height);
	float aspect = fWidth / fHeight;
	ICameraManager* iCameraManager = m_cameraManager;
	iCameraManager->OnResolutionChanged(fWidth, fHeight, aspect);

	return true;
}
