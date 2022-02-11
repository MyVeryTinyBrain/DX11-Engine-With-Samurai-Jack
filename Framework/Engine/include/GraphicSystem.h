#pragma once

#include "IGraphicSystem.h"

ENGINE_BEGIN
class ENGINE_API GraphicSystem final : public IGraphicSystem
{
public:

	GraphicSystem() = default;

	virtual ~GraphicSystem();

private:

	virtual bool Initialize(HWND hWnd, unsigned int width, unsigned int height, unsigned int refreshPerSec, bool vsync, bool fullScreen) override;

	virtual bool Release() override;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final override;

	virtual void Render() override;

public:

	bool ClearBackBuffer(const Color & color);

	bool ClearDepthStencilBuffer(float depth, unsigned int stencil);

	virtual bool Present() override;

public:

	virtual Com<ID3D11Device> GetDevice() const override;

	virtual Com<ID3D11DeviceContext> GetDeviceContext() const override;

	virtual RenderQueue* GetRenderQueue() const override;

	virtual CameraManager* GetCameraManager() const override;

	virtual DeferredScreenRender* GetDeferredScreenRender() const override;

	virtual LightManager* GetLightManager() const override;

	_declspec(property(get = GetDevice)) Com<ID3D11Device> device;
	_declspec(property(get = GetDeviceContext)) Com<ID3D11DeviceContext> deviceContext;
	_declspec(property(get = GetRenderQueue)) RenderQueue* renderQueue;
	_declspec(property(get = GetCameraManager)) CameraManager* cameraManager;
	_declspec(property(get = GetDeferredScreenRender)) DeferredScreenRender* deferredScreenRender;
	_declspec(property(get = GetLightManager)) LightManager* lightManager;

public:

	HWND GetWindowHandle() const;

	// Return width of resolution
	float GetWidth() const;

	// Return height of resolution
	float GetHeight() const;

	// Return aspect of resolution
	float GetAspect() const;

	bool SetResolution(uint width, uint height);

	inline Com<ID3D11RenderTargetView> GetBackBufferRenderTargetView() const { return m_backBufferRTV; }

	inline Com<ID3D11DepthStencilView> GetBackBufferDepthStencilView() const { return m_dsv; }

	// Use IDXGISwapChain::ResizeTarget
	//bool SetResolutionAndScreenSize(uint width, uint height, refresh);

	_declspec(property(get = GetWindowHandle)) HWND windowHandle;
	_declspec(property(get = GetWidth)) float width;
	_declspec(property(get = GetHeight)) float height;
	_declspec(property(get = GetAspect)) float aspect;
	_declspec(property(get = GetBackBufferRenderTargetView)) Com<ID3D11RenderTargetView> backBufferRenderTargetView;
	_declspec(property(get = GetBackBufferDepthStencilView)) Com<ID3D11DepthStencilView> backBufferDepthStencilView;

public:

	void ResetShaderResource();

	void RollbackRenderTarget();

	void SetRenderTargets(uint count, ID3D11RenderTargetView* const* arrRTV);

	void SetRenderTargetsWithDepthStencil(uint count, ID3D11RenderTargetView* const* arrRTV, ID3D11DepthStencilView* dsv);

public:

	const Color& GetClearColor() const;

	void SetClearColor(const Color & color);

	_declspec(property(get = GetClearColor, put = SetClearColor)) const Color& clearColor;

private:

	HRESULT CreateSwapChain(
		ID3D11Device * device,
		HWND hWnd, unsigned int width, unsigned int height,
		unsigned int refreshPerSec, bool vsync, bool fullScreen,
		IDXGISwapChain * *out_swapChain);

	HRESULT CreateBackBufferView(
		ID3D11Device * device,
		IDXGISwapChain * swapChain,
		ID3D11RenderTargetView * *out_backBufferView);

	HRESULT CreateDepthStencilView(
		ID3D11Device * device,
		unsigned int width, unsigned int height,
		ID3D11DepthStencilView * *out_depthStencilRTV);

	virtual HRESULT SetViewport
	(unsigned int width, unsigned int height) override;

	virtual uint2 GetViewport() const override;

	virtual HRESULT RollbackViewport() override;

private:

	HWND						m_hWnd = NULL;

	unsigned int				m_width = 0;

	unsigned int				m_height = 0;

	unsigned int				m_refreshPerSec = 0;

	bool						m_vsync = false;

	bool						m_fullScreen = false;

	Color						m_clearColor = Color::black();

	ID3D11Device*				m_device = nullptr;

	ID3D11DeviceContext*		m_deviceContext = nullptr;

	IDXGISwapChain*				m_swapChain = nullptr;

	ID3D11RenderTargetView*		m_backBufferRTV = nullptr;

	ID3D11DepthStencilView*		m_dsv = nullptr;

	CBufferManager*				m_CBufferManager = nullptr;
	InstanceBufferManager*		m_instanceBufferManager = nullptr;

	RenderQueue*				m_renderQueue = nullptr;

	CameraManager*				m_cameraManager = nullptr;

	DeferredScreenRender*		m_deferredScreenRender = nullptr;

	LightManager*				m_lightManager = nullptr;

};
ENGINE_END
