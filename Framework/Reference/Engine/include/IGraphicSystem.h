#pragma once

ENGINE_BEGIN
class CBufferManager;
class InstanceBufferManager;
class RenderQueue;
class CameraManager;
class DeferredScreenRender;
class LightManager;
class PostProcessing;
class ENGINE_API IGraphicSystem abstract
{
public:

	IGraphicSystem() = default;

	virtual ~IGraphicSystem() = default;

	virtual bool Initialize(HWND hWnd, unsigned int width, unsigned int height, unsigned int refreshPerSec, bool vsync, bool fullScreen) = 0;

	virtual bool Release() = 0;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	virtual void Render() = 0;

	virtual bool Present() = 0;

	virtual Com<ID3D11Device> GetDevice() const = 0;

	virtual Com<ID3D11DeviceContext> GetDeviceContext() const = 0;

	virtual RenderQueue* GetRenderQueue() const = 0;

	virtual CameraManager* GetCameraManager() const = 0;

	virtual PostProcessing* GetPostProcessing() const = 0;
	virtual DeferredScreenRender* GetDeferredScreenRender() const = 0;

	virtual LightManager* GetLightManager() const = 0;

public:

	virtual HRESULT SetViewport
	(unsigned int width, unsigned int height) = 0;

	virtual uint2 GetViewport() const = 0;

	virtual HRESULT RollbackViewport() = 0;

};

ENGINE_END

