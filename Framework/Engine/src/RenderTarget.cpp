#include "EnginePCH.h"
#include "RenderTarget.h"
#include "DxTextureUtility.h"

RenderTarget::RenderTarget(
	ID3D11Resource* texture, ID3D11ShaderResourceView* srv, 
	ID3D11RenderTargetView* rtv, const D3D11_TEXTURE2D_DESC& desc) :
	m_texture(texture),
	m_srv(srv),
	m_rtv(rtv),
	m_desc(desc)
{
}

HRESULT RenderTarget::Create(Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DXGI_FORMAT format, RenderTarget** out_renderTarget)
{
	if (!out_renderTarget)
		return E_FAIL;

	HRESULT hr = S_OK;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(hr = CreateTexture2D(device, width, height, false, format, &texture, &srv, &rtv, &desc)))
		return hr;

	((ID3D11Texture2D*)texture)->GetDesc(&desc);

	*out_renderTarget = new RenderTarget(texture, srv, rtv, desc);

	return S_OK;
}

RenderTarget::~RenderTarget()
{
	SafeRelease(m_texture);
	SafeRelease(m_srv);
	SafeRelease(m_rtv);
}

bool RenderTarget::Resize(Com<ID3D11Device> device, unsigned int width, unsigned int height)
{
	if (GetWidth() == width && GetHeight() == height)
		return true;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(device, width, height, false, m_desc.Format, &texture, &srv, &rtv, &desc)))
		return false;

	((ID3D11Texture2D*)texture)->GetDesc(&desc);

	SafeRelease(m_texture);
	SafeRelease(m_srv);
	SafeRelease(m_rtv);

	m_texture = texture;
	m_srv = srv;
	m_rtv = rtv;
	m_desc = desc;

	return true;
}

void RenderTarget::Clear(Com<ID3D11DeviceContext> deviceContext, const Color& clearColor)
{
	if (!deviceContext)
		return;

	deviceContext->ClearRenderTargetView(m_rtv, (FLOAT*)&clearColor);
}

Com<ID3D11Resource> RenderTarget::GetTexture() const
{
	return m_texture;
}

Com<ID3D11ShaderResourceView> RenderTarget::GetSRV() const
{
	return m_srv;
}

Com<ID3D11RenderTargetView> RenderTarget::GetRTV() const
{
	return m_rtv;
}

unsigned int RenderTarget::GetWidth() const
{
	return m_desc.Width;
}

unsigned int RenderTarget::GetHeight() const
{
	return m_desc.Height;
}

HRESULT RenderTarget::CreateTexture2D(
	Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DXGI_FORMAT format, 
	ID3D11Resource** out_texture, ID3D11ShaderResourceView** out_srv, ID3D11RenderTargetView** out_rtv, D3D11_TEXTURE2D_DESC* out_desc)
{
	TextureCreateDesc tcDesc = {};
	tcDesc.Format = format;
	tcDesc.Size = uint2(width, height);
	tcDesc.Option.Usage = TextureCreationUsage::Default;
	TextureResourceDesc trDesc = {};
	trDesc.CreateSRV = true;
	trDesc.CreateRTV = true;
	return DxTextureUtility::CreateTexture2D(device, tcDesc, trDesc, (ID3D11Texture2D**)out_texture, out_srv, out_rtv);
}
