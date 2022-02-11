#pragma once

#include "Texture.h"

ENGINE_BEGIN

class ENGINE_API RenderTexture2D : public Texture
{
protected:
	
	RenderTexture2D(
		ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName,
		class RenderTarget* renderTarget);

	virtual ~RenderTexture2D();

public:

	bool Resize(unsigned int width, unsigned int height);

public:

	virtual Com<ID3D11Resource> GetTexture() const override;

	virtual Com<ID3D11ShaderResourceView> GetSRV() const override;

	Com<ID3D11RenderTargetView> GetRTV() const;

	inline class RenderTarget* GetRenderTarget() const { return m_renderTarget; }

	unsigned int GetWidth() const;

	unsigned int GetHeight() const;

	float GetAspect() const;

	_declspec(property(get = GetRTV)) Com<ID3D11RenderTargetView> renderTargetView;
	_declspec(property(get = GetRenderTarget)) class RenderTarget* renderTarget;
	_declspec(property(get = GetWidth)) unsigned int width;
	_declspec(property(get = GetHeight)) unsigned int height;
	_declspec(property(get = GetAspect)) float aspect;

public:

	static ResourceRef<RenderTexture2D> CreateManagedRenderTexture2D(
		ResourceManagement* management, const tstring& resourceKey, 
		unsigned int width, unsigned int height);

	static ResourceRef<RenderTexture2D> CreateManagedRenderTexture2D(
		ResourceManagement* management, const tstring& resourceKey, const tstring& groupName, 
		unsigned int width, unsigned int height);

	static ResourceRef<RenderTexture2D> CreateUnmanagedRenderTexture2D(
		ResourceManagement* management, 
		unsigned int width, unsigned int height);

private:

	class RenderTarget*			m_renderTarget = nullptr;
};

ENGINE_END
