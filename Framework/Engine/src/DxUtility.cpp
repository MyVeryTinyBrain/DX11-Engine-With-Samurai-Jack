#include "EnginePCH.h"
#include "DxUtility.h"
#include "..\include\DxUtility.h"

void DxUtility::ResetShaderResource(Com<ID3D11DeviceContext> dc)
{
	ID3D11ShaderResourceView* nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
	dc->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRV);
	dc->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRV);
}

void DxUtility::SetRenderTargets(Com<ID3D11DeviceContext> dc, uint numRTV, ID3D11RenderTargetView* const* arrRTV, ID3D11DepthStencilView* dsv)
{
	ResetShaderResource(dc);
	dc->OMSetRenderTargets(numRTV, arrRTV, dsv);
}

void DxUtility::GetRenderTargets(Com<ID3D11DeviceContext> dc, ID3D11RenderTargetView** out_ArrRTV, ID3D11DepthStencilView** out_dsv)
{
	if (out_ArrRTV) *out_ArrRTV = {};
	if (out_dsv) *out_dsv = nullptr;

	dc->OMGetRenderTargets(8, out_ArrRTV, out_dsv);

	if (out_ArrRTV)
	{
		for (uint i = 0; i < 8; ++i)
		{
			if (out_ArrRTV[i])
				out_ArrRTV[i]->Release();
		}
	}

	if (out_dsv)
	{
		if (*out_dsv)
			(*out_dsv)->Release();
	}
}

void DxUtility::SetViewport(Com<ID3D11DeviceContext> dc, uint width, uint height)
{
	D3D11_VIEWPORT viewportDesc = {};
	viewportDesc.Width = float(width);
	viewportDesc.Height = float(height);
	viewportDesc.TopLeftX = viewportDesc.TopLeftY = 0;
	viewportDesc.MinDepth = 0.0f;
	viewportDesc.MaxDepth = 1.0f;

	dc->RSSetViewports(1, &viewportDesc);
}

void DxUtility::SetViewport(Com<ID3D11DeviceContext> dc, uint2 size)
{
	DxUtility::SetViewport(dc, size.x, size.y);
}

void DxUtility::SetViewportByRect(Com<ID3D11DeviceContext> dc, RECT rect)
{
	D3D11_VIEWPORT viewportDesc = {};
	viewportDesc.Width = float(rect.right - rect.left);
	viewportDesc.Height = float(rect.bottom - rect.top);
	viewportDesc.TopLeftX = float(rect.left);
	viewportDesc.TopLeftY = float(rect.top);
	viewportDesc.MinDepth = 0.0f;
	viewportDesc.MaxDepth = 1.0f;

	dc->RSSetViewports(1, &viewportDesc);
}

RECT DxUtility::GetViewportAsRect(Com<ID3D11DeviceContext> dc)
{
	uint num = 1;
	D3D11_VIEWPORT viewport;
	dc->RSGetViewports(&num, &viewport);

	RECT rect;
	rect.left = LONG(viewport.TopLeftX);
	rect.top = LONG(viewport.TopLeftY);
	rect.right = LONG(viewport.TopLeftX + viewport.Width);
	rect.bottom = LONG(viewport.TopLeftY + viewport.Height);

	return rect;
}

uint2 DxUtility::GetViewport(Com<ID3D11DeviceContext> dc)
{
	uint num = 1;
	D3D11_VIEWPORT viewport;
	dc->RSGetViewports(&num, &viewport);

	return uint2(uint(viewport.Width), uint(viewport.Height));
}

void DxUtility::ClearRTV(Com<ID3D11DeviceContext> dc, const Color& color, ID3D11RenderTargetView* rtv)
{
	dc->ClearRenderTargetView(rtv, (FLOAT*)&color);
}

void DxUtility::ClearDSV(Com<ID3D11DeviceContext> dc, ID3D11DepthStencilView* dsv)
{
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DxUtility::ExecuteDeferredContext(Com<ID3D11DeviceContext> immediate, Com<ID3D11DeviceContext> deferred)
{
	ID3D11CommandList* commandList = nullptr;
	deferred->FinishCommandList(FALSE, &commandList);
	if (commandList)
	{
		immediate->ExecuteCommandList(commandList, TRUE);
		commandList->Release();
	}
}

void DxUtility::CopyContextState(Com<ID3D11DeviceContext> dest, Com<ID3D11DeviceContext> src)
{
	dest->ClearState();

	ID3D11RenderTargetView* arrRTV[8] = {};
	ID3D11DepthStencilView* dsv = nullptr;
	DxUtility::SetRenderTargets(src, 8, arrRTV, dsv);
	DxUtility::SetRenderTargets(dest, 8, arrRTV, dsv);

	uint2 viewport = DxUtility::GetViewport(src);
	DxUtility::SetViewport(dest, viewport);
}

HRESULT DxUtility::CreateSwapChain(ID3D11Device* device, HWND hWnd, unsigned int width, unsigned int height, unsigned int refreshPerSec, bool vsync, bool fullScreen, IDXGISwapChain** out_swapChain)
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

HRESULT DxUtility::CreateBackBufferView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView** out_backBufferView)
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

	if (FAILED(device->CreateRenderTargetView(backBufferTex, nullptr, out_backBufferView)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeBackBufferView::ID3D11Device::CreateRenderTargetView");
	}

	ReleaseVars();

	return S_OK;
}

HRESULT DxUtility::CreateDepthStencilView(ID3D11Device* device, unsigned int width, unsigned int height, ID3D11DepthStencilView** out_depthStencilRTV)
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
