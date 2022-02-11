#pragma once

#include "ITexture.h"

ENGINE_BEGIN

class ENGINE_API RenderTarget : public ITexture
{
private:

	RenderTarget(
		ID3D11Resource* texture, ID3D11ShaderResourceView* srv,
		ID3D11RenderTargetView* rtv, const D3D11_TEXTURE2D_DESC& desc);

public:

	// Default Foramt: DXGI_FORMAT_R32G32B32A32_FLOAT
	static HRESULT Create(Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DXGI_FORMAT format, RenderTarget** out_renderTarget);

	~RenderTarget();

public:

	bool Resize(Com<ID3D11Device> device, unsigned int width, unsigned int height);

	void Clear(Com<ID3D11DeviceContext> deviceContext, const Color& clearColor);

public:

	Com<ID3D11Resource> GetTexture() const override;

	Com<ID3D11ShaderResourceView> GetSRV() const override;

	Com<ID3D11RenderTargetView> GetRTV() const;

	unsigned int GetWidth() const;

	unsigned int GetHeight() const;

	inline float GetAspect() const { return float(GetWidth()) / float(GetHeight()); }

	_declspec(property(get = GetTexture)) Com<ID3D11Resource> texture;
	_declspec(property(get = GetSRV)) Com<ID3D11ShaderResourceView> srv;
	_declspec(property(get = GetRTV)) Com<ID3D11RenderTargetView> rtv;
	_declspec(property(get = GetWidth)) unsigned int width;
	_declspec(property(get = GetHeight)) unsigned int height;
	_declspec(property(get = GetAspect)) float aspect;

private:

	static HRESULT CreateTexture2D(
		Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DXGI_FORMAT format,
		ID3D11Resource** out_texture, ID3D11ShaderResourceView** out_srv, ID3D11RenderTargetView** out_rtv, D3D11_TEXTURE2D_DESC* out_desc);

private:

	ID3D11Resource*					m_texture = nullptr;

	ID3D11ShaderResourceView*		m_srv = nullptr;

	ID3D11RenderTargetView*			m_rtv = nullptr;

	D3D11_TEXTURE2D_DESC			m_desc = {};
};

ENGINE_END
