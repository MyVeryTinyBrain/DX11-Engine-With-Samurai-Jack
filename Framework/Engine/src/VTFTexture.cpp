#include "EnginePCH.h"
#include "VTFTexture.h"

VTFTexture::VTFTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv, uint2 size)
{
	m_texture = texture;
	m_srv = srv;
	m_size = size;
}

VTFTexture* VTFTexture::Create(Com<ID3D11Device> device, uint matrixCount)
{
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	uint2 size = uint2(matrixCount, 4);

	HRESULT hr = S_OK;
	if(FAILED(hr = CreateTexture2D(device, size.x, size.y, &texture, &srv)))
		return nullptr;

	return new VTFTexture(texture, srv, size);
}

VTFTexture::~VTFTexture()
{
	Release();
}

void VTFTexture::Release()
{
	m_size = uint2(0, 0);
	SafeDelete(m_texture);
	SafeDelete(m_srv);
}

bool VTFTexture::Resize(Com<ID3D11Device> device, uint matrixCount)
{
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	uint2 size = uint2(matrixCount, 4);

	if (FAILED(CreateTexture2D(device, m_size.x, m_size.y, &texture, &srv)))
		return false;

	Release();

	m_size = size;
	m_texture = texture;
	m_srv = srv;

	return true;
}

bool VTFTexture::Map(Com<ID3D11DeviceContext> deviceContext)
{
	if (!deviceContext)
		return false;

	if (FAILED(deviceContext->Map(m_texture, 0, D3D11_MAP_READ_WRITE, 0, &m_resource)))
	{
		m_resource = {};
		return false;
	}

	return true;
}

bool VTFTexture::SetMatrix(uint index, const M4& matrix)
{
	if (!m_resource.pData)
		return false;

	if (index > m_size.x)
		return false;

	UCHAR* bytes = (UCHAR*)m_resource.pData;

	uint rowi0 = TexCoordToAddress(uint2(index, 0), m_resource.RowPitch);
	uint rowi1 = TexCoordToAddress(uint2(index, 1), m_resource.RowPitch);
	uint rowi2 = TexCoordToAddress(uint2(index, 2), m_resource.RowPitch);
	uint rowi3 = TexCoordToAddress(uint2(index, 3), m_resource.RowPitch);

	XMVECTOR* row0 = (XMVECTOR*)(bytes + rowi0);
	XMVECTOR* row1 = (XMVECTOR*)(bytes + rowi1);
	XMVECTOR* row2 = (XMVECTOR*)(bytes + rowi2);
	XMVECTOR* row3 = (XMVECTOR*)(bytes + rowi3);

	*row0 = matrix.xmm.r[0];
	*row1 = matrix.xmm.r[1];
	*row2 = matrix.xmm.r[2];
	*row3 = matrix.xmm.r[3];

	return true;
}

bool VTFTexture::GetMatrix(uint index, M4& out_matrix) const
{
	if (!m_resource.pData)
		return false;

	if (index > m_size.x)
		return false;

	UCHAR* bytes = (UCHAR*)m_resource.pData;

	uint rowi0 = TexCoordToAddress(uint2(index, 0), m_resource.RowPitch);
	uint rowi1 = TexCoordToAddress(uint2(index, 1), m_resource.RowPitch);
	uint rowi2 = TexCoordToAddress(uint2(index, 2), m_resource.RowPitch);
	uint rowi3 = TexCoordToAddress(uint2(index, 3), m_resource.RowPitch);

	XMVECTOR* row0 = (XMVECTOR*)(bytes + rowi0);
	XMVECTOR* row1 = (XMVECTOR*)(bytes + rowi1);
	XMVECTOR* row2 = (XMVECTOR*)(bytes + rowi2);
	XMVECTOR* row3 = (XMVECTOR*)(bytes + rowi3);

	out_matrix.xmm.r[0] = *row0;
	out_matrix.xmm.r[1] = *row1;
	out_matrix.xmm.r[2] = *row2;
	out_matrix.xmm.r[3] = *row3;

	return true;
}

void VTFTexture::Unmap(Com<ID3D11DeviceContext> deviceContext)
{
	deviceContext->Unmap(m_texture, 0);
	m_resource = {};
}

HRESULT VTFTexture::CreateTexture2D(
	Com<ID3D11Device> device, unsigned int width, unsigned int height, 
	ID3D11Texture2D** out_texture, ID3D11ShaderResourceView** out_srv)
{
	if (!device)
		return E_FAIL;

	if (!out_texture)
		return E_FAIL;

	if (!out_srv)
		return E_FAIL;

	if (width * height == 0)
		return E_FAIL;

	*out_texture = nullptr;
	*out_srv = nullptr;

	HRESULT hr = S_OK;
	uint32_t pixelColor = Color::clear();
	uint32_t* arrPixelColors = new uint32_t[width * height];
	std::fill(arrPixelColors, arrPixelColors + (width * height), pixelColor);

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = arrPixelColors;
	initData.SysMemPitch = width * sizeof(uint32_t);
	initData.SysMemSlicePitch = width * height * sizeof(uint32_t);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	auto ReleaseVars = [&]()
	{
		SafeRelease(texture);
		SafeRelease(srv);
	};

	if (FAILED(hr = device->CreateTexture2D(&textureDesc, &initData, &texture)))
	{
		SafeDeleteArray(arrPixelColors);
		ReleaseVars();
		return hr;
	}
	SafeDeleteArray(arrPixelColors);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(hr = device->CreateShaderResourceView(texture, &srvDesc, &srv)))
	{
		ReleaseVars();
		return hr;
	}

	*out_texture = texture;
	*out_srv = srv;

	return S_OK;
}
