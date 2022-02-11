#include "EnginePCH.h"
#include "DepthStencil.h"

DepthStencil::DepthStencil(ID3D11Resource* texture, ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* srv, const D3D11_TEXTURE2D_DESC& desc, DepthStencil::Type type) :
	m_texture(texture),
	m_dsv(dsv),
	m_srv(srv),
	m_desc(desc),
	m_type(type)
{
}

DepthStencil::~DepthStencil()
{
	SafeRelease(m_texture);
	SafeRelease(m_dsv);
	SafeRelease(m_srv);
}

HRESULT DepthStencil::Create(Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DepthStencil::Type type, DepthStencil** out_depthStencil)
{
	if (!out_depthStencil)
		return E_FAIL;

	HRESULT hr = S_OK;

	ID3D11Resource* texture = nullptr;
	ID3D11DepthStencilView* dsv = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(hr = CreateTexture2D(device, width, height, false, type, &texture, &dsv, &srv, &desc)))
		return hr;

	*out_depthStencil = new DepthStencil(texture, dsv, srv, desc, type);

	return S_OK;
}

bool DepthStencil::Resize(Com<ID3D11Device> device, unsigned int width, unsigned int height)
{
	if (GetWidth() == width && GetHeight() == height)
		return true;

	ID3D11Resource* texture = nullptr;
	ID3D11DepthStencilView* dsv = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(device, width, height, false, m_type, &texture, &dsv, &srv, &desc)))
		return false;

	SafeRelease(m_texture);
	SafeRelease(m_dsv);
	SafeRelease(m_srv);

	m_texture = texture;
	m_dsv = dsv;
	m_srv = srv;
	m_desc = desc;

	return true;
}

void DepthStencil::Clear(Com<ID3D11DeviceContext> deviceContext)
{
	deviceContext->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

Com<ID3D11Resource> DepthStencil::GetTexture() const
{
	return m_texture;
}

Com<ID3D11DepthStencilView> DepthStencil::GetDSV() const
{
	return m_dsv;
}

Com<ID3D11ShaderResourceView> DepthStencil::GetSRV() const
{
	return m_srv;
}

DepthStencil::Type DepthStencil::GetType() const
{
	return m_type;
}

unsigned int DepthStencil::GetWidth() const
{
	return m_desc.Width;
}

unsigned int DepthStencil::GetHeight() const
{
	return m_desc.Height;
}

HRESULT DepthStencil::CreateTexture2D(
	Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DepthStencil::Type type,
	ID3D11Resource** out_texture, ID3D11DepthStencilView** out_dsv, ID3D11ShaderResourceView** out_srv, D3D11_TEXTURE2D_DESC* out_desc)
{
	if (!device)
		return E_FAIL;

	if (!out_texture)
		return E_FAIL;

	if (!out_dsv)
		return E_FAIL;

	if (!out_desc)
		return E_FAIL;

	if (width * height == 0)
		return E_FAIL;

	ID3D11Texture2D* texture = nullptr;
	ID3D11DepthStencilView* dsv = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(texture);
		SafeRelease(dsv);
		SafeRelease(srv);
	};

	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	switch (type)
	{
		case DepthStencil::Type::WRITEONLY_DEPTH_STENCIL:
			textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case DepthStencil::Type::SRV_DEPTH:
			textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			break;
	}
	
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	switch (type)
	{
		case DepthStencil::Type::SRV_DEPTH:
			textureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
			break;
	}

	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &texture)))
	{
		ReleaseVars();
		RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeDepthStencilBufferView::ID3D11Device::CreateTexture2D");
	}

	switch (type)
	{
		case DepthStencil::Type::WRITEONLY_DEPTH_STENCIL:
			if (FAILED(device->CreateDepthStencilView(texture, nullptr, &dsv)))
			{
				ReleaseVars();
				RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeDepthStencilBufferView::ID3D11Device::CreateDepthStencilView");
			}
			break;
		case DepthStencil::Type::SRV_DEPTH:
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			if (FAILED(device->CreateDepthStencilView(texture, &dsvDesc, &dsv)))
			{
				ReleaseVars();
				RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeDepthStencilBufferView::ID3D11Device::CreateDepthStencilView");
			}
			break;
	}

	switch (type)
	{
		case DepthStencil::Type::SRV_DEPTH:
			if (!out_srv)
				return E_FAIL;

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srvDesc.Texture2D.MipLevels = uint32(-1);
			if (FAILED(device->CreateShaderResourceView(texture, &srvDesc, &srv)))
			{
				ReleaseVars();
				RETURN_E_FAIL_ERROR_MESSAGE("GraphicSystem::InitializeDepthStencilBufferView::ID3D11Device::CreateShaderResourceView");
			}
			break;
	}

	*out_texture = texture;
	*out_dsv = dsv;
	*out_srv = srv;
	*out_desc = textureDesc;

	return S_OK;
}
