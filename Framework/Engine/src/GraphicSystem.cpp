#include "EnginePCH.h"
#include "GraphicSystem.h"
#include "CBufferManager.h"
#include "InstanceBufferManager.h"
#include "RenderQueue.h"
#include "CameraManager.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "PostProcessing.h"
#include "DeferredScreenRender.h"
#include "LightManager.h"

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
	flag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature;
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flag, nullptr, 0, D3D11_SDK_VERSION, &m_device, &feature, &m_deviceContext)))
		return false;

	if (FAILED(CreateSwapChain(m_device, hWnd, width, height, refreshPerSec, vsync, fullScreen, &m_swapChain)))
		return false;

	if (FAILED(CreateBackBufferView(m_device, m_swapChain, &m_backBufferRTV)))
		return false;

	if (FAILED(CreateDepthStencilView(m_device, width, height, &m_dsv)))
		return false;

	m_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_dsv);

	if (FAILED(SetViewport(width, height)))
		return false;

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

	m_deferredScreenRender = new DeferredScreenRender(this, m_CBufferManager, m_instanceBufferManager);
	if (FAILED(m_deferredScreenRender->Initialize()))
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
	SafeDelete(m_deferredScreenRender);
	SafeDelete(m_cameraManager);
	SafeDelete(m_renderQueue);
	SafeDelete(m_instanceBufferManager);
	SafeDelete(m_CBufferManager);
	SafeRelease(m_dsv);
	SafeRelease(m_backBufferRTV);
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
			SetResolution((uint)(rect.right - rect.left), (uint)(rect.bottom - rect.top));
		}
		break;
	}
}

void GraphicSystem::Render()
{
	Color clearColorLerped = Color::Lerp(m_clearColor, Color::white(), 1 - m_clearColor.a);
	
	for (size_t i = 0; i < m_cameraManager->GetCameraCount(); ++i)
	{
		ICamera* camera = m_cameraManager->GetCamera(i);

		ID3D11RenderTargetView* currentRTV = nullptr;
		ID3D11DepthStencilView* currentDSV = nullptr;
		RenderTarget* cameraRenderTarget = camera->GetRenderTarget();
		DepthStencil* cameraDepthStencil = camera->GetDepthStencil();
		ID3D11RenderTargetView* camRTV = nullptr;
		ID3D11DepthStencilView* camDSV = nullptr;
		if (cameraRenderTarget && cameraDepthStencil)
		{
			camRTV = cameraRenderTarget->rtv.Get();
			camDSV = cameraDepthStencil->dsv.Get();
			SetViewport(cameraRenderTarget->width, cameraRenderTarget->height);
		}

		if (camRTV && camDSV)
		{
			m_deviceContext->OMGetRenderTargets(1, &currentRTV, &currentDSV);

			m_deviceContext->OMSetRenderTargets(1, &camRTV, camDSV);
			m_deviceContext->ClearRenderTargetView(camRTV, (FLOAT*)&clearColorLerped);
			m_deviceContext->ClearDepthStencilView(camDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		else
		{
			ClearBackBuffer(clearColorLerped);
			ClearDepthStencilBuffer(1.0f, 0);
		}

		m_renderQueue->Render(camera);

		if (camRTV && camDSV)
		{
			m_deviceContext->OMSetRenderTargets(1, &currentRTV, currentDSV);
			SafeRelease(currentRTV);
			SafeRelease(currentDSV);

			RollbackViewport();
		}
	}

	m_renderQueue->Clear();
}

bool GraphicSystem::ClearBackBuffer(const Color& color)
{
	if (!m_backBufferRTV)
		return false;

	m_deviceContext->ClearRenderTargetView(m_backBufferRTV, (FLOAT*)&color);

	return true;
}

bool GraphicSystem::ClearDepthStencilBuffer(float depth, unsigned int stencil)
{
	if (!m_dsv)
		return false;

	m_deviceContext->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);

	return true;
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

DeferredScreenRender* GraphicSystem::GetDeferredScreenRender() const
{
	return m_deferredScreenRender;
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

bool GraphicSystem::SetResolution(uint width, uint height)
{
	SafeRelease(m_backBufferRTV);
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

	if (FAILED(CreateBackBufferView(m_device, m_swapChain, &backBufferRTV)))
	{
		ReleaseVars();
		RETURN_FALSE_ERROR_MESSAGE("GraphicSystem::SetResolution::CreateBackBufferView");
	}

	if (FAILED(CreateDepthStencilView(m_device, width, height, &depthStencilRTV)))
	{
		ReleaseVars();
		RETURN_FALSE_ERROR_MESSAGE("GraphicSystem::SetResolution::CreateDepthStencilBufferView");
	}

	if (FAILED(SetViewport(width, height)))
	{
		ReleaseVars();
		RETURN_FALSE_ERROR_MESSAGE("GraphicSystem::SetResolution::SetViewport");
	}

	m_deviceContext->OMSetRenderTargets(1, &backBufferRTV, depthStencilRTV);

	{
		SafeRelease(m_dsv);
		SafeRelease(m_backBufferRTV);

		m_backBufferRTV = backBufferRTV;
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

void GraphicSystem::ResetShaderResource()
{
	ID3D11ShaderResourceView* nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
	m_deviceContext->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRV);
	m_deviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRV);
}

void GraphicSystem::RollbackRenderTarget()
{
	ResetShaderResource();
	m_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_dsv);
}

void GraphicSystem::SetRenderTargets(uint count, ID3D11RenderTargetView* const* arrRTV)
{
	ResetShaderResource();
	m_deviceContext->OMSetRenderTargets(count, arrRTV, m_dsv);
}

void GraphicSystem::SetRenderTargetsWithDepthStencil(uint count, ID3D11RenderTargetView* const* arrRTV, ID3D11DepthStencilView* dsv)
{
	ResetShaderResource();
	m_deviceContext->OMSetRenderTargets(count, arrRTV, dsv);
}

const Color& GraphicSystem::GetClearColor() const
{
	return m_clearColor;
}

void GraphicSystem::SetClearColor(const Color& color)
{
	m_clearColor = color;
}

HRESULT GraphicSystem::CreateSwapChain(ID3D11Device* device, HWND hWnd, unsigned int width, unsigned int height, unsigned int refreshPerSec, bool vsync, bool fullScreen, IDXGISwapChain** out_swapChain)
{
	if (!out_swapChain)
		return E_FAIL;

	IDXGIDevice* dxgiDevice = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIFactory* dxgiFactory = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(dxgiFactory);
		SafeRelease(dxgiAdapter);
		SafeRelease(dxgiDevice);
	};

	if (!device || !hWnd)
		return E_FAIL;

	if (FAILED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeSwapChain::ID3D11Device::QueryInterface");
	}

	if (FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeSwapChain::IDXGIDevice::GetParent");
	}

	if (FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeSwapChain::IDXGIAdapter::GetParent");
	}

	DXGI_SWAP_CHAIN_DESC desc = {};

	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	if (vsync)
	{
		desc.BufferDesc.RefreshRate.Numerator = refreshPerSec;
		desc.BufferDesc.RefreshRate.Denominator = 1;
	}
	else
	{
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;

	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 1;
	desc.OutputWindow = hWnd;
	desc.Windowed = !fullScreen;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(dxgiFactory->CreateSwapChain(device, &desc, out_swapChain)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeSwapChain::IDXGIFactory::CreateSwapChain");
	}

	ReleaseVars();

	return S_OK;
}

HRESULT GraphicSystem::CreateBackBufferView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView** out_backBufferView)
{
	if (!out_backBufferView)
		return E_FAIL;

	ID3D11Texture2D* backBufferTex = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(backBufferTex);
	};

	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTex)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeBackBufferView::IDXGISwapChain::GetBuffer");
	}

	if (FAILED(m_device->CreateRenderTargetView(backBufferTex, nullptr, out_backBufferView)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeBackBufferView::ID3D11Device::CreateRenderTargetView");
	}

	ReleaseVars();

	return S_OK;
}

HRESULT GraphicSystem::CreateDepthStencilView(ID3D11Device* device, unsigned int width, unsigned int height, ID3D11DepthStencilView** out_depthStencilRTV)
{
	if (!out_depthStencilRTV)
		return E_FAIL;

	ID3D11Texture2D* depthStencilBufferTex = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(depthStencilBufferTex);
	};

	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;

	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&desc, nullptr, &depthStencilBufferTex)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeDepthStencilBufferView::ID3D11Device::CreateTexture2D");
	}

	if (FAILED(device->CreateDepthStencilView(depthStencilBufferTex, nullptr, out_depthStencilRTV)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeDepthStencilBufferView::ID3D11Device::CreateDepthStencilView");
	}

	ReleaseVars();

	return S_OK;
}

HRESULT GraphicSystem::SetViewport(unsigned int width, unsigned int height)
{
	D3D11_VIEWPORT viewportDesc = {};
	viewportDesc.Width = float(width);
	viewportDesc.Height = float(height);
	viewportDesc.TopLeftX = viewportDesc.TopLeftY = 0;
	viewportDesc.MinDepth = 0.0f;
	viewportDesc.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewportDesc);

	return S_OK;
}

uint2 GraphicSystem::GetViewport() const
{
	uint num = 1;
	D3D11_VIEWPORT viewport;
	m_deviceContext->RSGetViewports(&num, &viewport);

	return uint2(uint(viewport.Width), uint(viewport.Height));
}

HRESULT GraphicSystem::RollbackViewport()
{
	return SetViewport(m_width, m_height);
}
