#include "EnginePCH.h"
#include "RenderTarget.h"

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
	if (!device)
		return E_FAIL;

	if (!out_texture)
		return E_FAIL;

	if (!out_srv)
		return E_FAIL;

	if (!out_rtv)
		return E_FAIL;

	if (!out_desc)
		return E_FAIL;

	if (width * height == 0)
		return E_FAIL;

	*out_texture = nullptr;
	*out_srv = nullptr;
	*out_rtv = nullptr;

	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;
	auto ReleaseVars = [&]()
	{
		SafeRelease(texture);
		SafeRelease(srv);
		SafeRelease(rtv);
	};

	if (FAILED(hr = device->CreateTexture2D(&desc, nullptr, &texture)))
	{
		ReleaseVars();
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(hr = device->CreateShaderResourceView(texture, &srvDesc, &srv)))
	{
		ReleaseVars();
		return hr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	if (FAILED(hr = device->CreateRenderTargetView(texture, &rtvDesc, &rtv)))
	{
		ReleaseVars();
		return hr;
	}

	*out_texture = texture;
	*out_srv = srv;
	*out_rtv = rtv;
	*out_desc = desc;

	return S_OK;
}
