#pragma once

ENGINE_BEGIN

namespace DxUtility
{
	ENGINE_API void ResetShaderResource(Com<ID3D11DeviceContext> dc);
	ENGINE_API void SetRenderTargets(Com<ID3D11DeviceContext> dc, uint numRTV, ID3D11RenderTargetView* const* arrRTV, ID3D11DepthStencilView* dsv);
	ENGINE_API void GetRenderTargets(Com<ID3D11DeviceContext> dc, ID3D11RenderTargetView** out_ArrRTV, ID3D11DepthStencilView** out_dsv);
	ENGINE_API void SetViewport(Com<ID3D11DeviceContext> dc, uint width, uint height);
	ENGINE_API void SetViewport(Com<ID3D11DeviceContext> dc, uint2 size);
	ENGINE_API void SetViewportByRect(Com<ID3D11DeviceContext> dc, RECT rect);
	ENGINE_API RECT GetViewportAsRect(Com<ID3D11DeviceContext> dc);
	ENGINE_API uint2 GetViewport(Com<ID3D11DeviceContext> dc);
	ENGINE_API void ClearRTV(Com<ID3D11DeviceContext> dc, const Color& color, ID3D11RenderTargetView* rtv);
	ENGINE_API void ClearDSV(Com<ID3D11DeviceContext> dc, ID3D11DepthStencilView* dsv);
	ENGINE_API void ExecuteDeferredContext(Com<ID3D11DeviceContext> immediate, Com<ID3D11DeviceContext> deferred);
	ENGINE_API void CopyContextState(Com<ID3D11DeviceContext> dest, Com<ID3D11DeviceContext> src);

	ENGINE_API HRESULT CreateSwapChain(
		ID3D11Device* device,
		HWND hWnd, unsigned int width, unsigned int height,
		unsigned int refreshPerSec, bool vsync, bool fullScreen,
		IDXGISwapChain** out_swapChain);
	ENGINE_API HRESULT CreateBackBufferView(
		ID3D11Device* device,
		IDXGISwapChain* swapChain,
		ID3D11RenderTargetView** out_backBufferView);
	ENGINE_API HRESULT CreateDepthStencilView(
		ID3D11Device* device,
		unsigned int width, unsigned int height,
		ID3D11DepthStencilView** out_depthStencilRTV);
};

ENGINE_END
