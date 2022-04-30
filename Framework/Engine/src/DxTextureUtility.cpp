#include "EnginePCH.h"
#include "DxTextureUtility.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

HRESULT DxTextureUtility::CreateTexture2D(
	Com<ID3D11Device> device, 
	const TextureCreateDesc& tcDesc,
	const TextureResourceDesc& trDesc,
	ID3D11Texture2D** out_texture,
	ID3D11ShaderResourceView** out_srv, 
	ID3D11RenderTargetView** out_rtv)
{
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC tex2dDesc = {};
	Byte* pixelBytes = nullptr;
	D3D11_SUBRESOURCE_DATA tex2dData = {};
	D3D11_SUBRESOURCE_DATA* pTex2dData = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};

	auto FailedReleaseVariables = [&]()
	{
		SafeRelease(*out_texture);
		SafeRelease(*out_srv);
		SafeRelease(*out_rtv);
	};

	if (out_texture) *out_texture = nullptr;
	if (out_srv) *out_srv = nullptr;
	if (out_rtv) *out_rtv = nullptr;

	if (!device)
		return E_FAIL;

	if (!out_texture)
		return E_FAIL;

	if (trDesc.CreateSRV && !out_srv)
		return E_FAIL;

	if (trDesc.CreateRTV && !out_rtv)
		return E_FAIL;

	if (tcDesc.Size.x * tcDesc.Size.y == 0)
		return E_FAIL;

	tex2dDesc.Width = tcDesc.Size.x;
	tex2dDesc.Height = tcDesc.Size.y;
	tex2dDesc.MipLevels = tcDesc.Option.GenerateMipmap ? 0 : 1;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.Format = tcDesc.Format;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.Usage = (D3D11_USAGE)tcDesc.Option.Usage;
	tex2dDesc.BindFlags = (trDesc.CreateSRV ? D3D11_BIND_SHADER_RESOURCE : 0) | (trDesc.CreateRTV ? D3D11_BIND_RENDER_TARGET : 0);
	tex2dDesc.CPUAccessFlags = (tcDesc.Option.CPURead ? D3D11_CPU_ACCESS_READ : 0) | (tcDesc.Option.CPUWrite ? D3D11_CPU_ACCESS_WRITE : 0);

	srvDesc.Format = tcDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = tcDesc.Option.GenerateMipmap ? -1 : 1;

	rtvDesc.Format = tcDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	if (tcDesc.SetColor)
	{
		SetTexture2DPixels(&tex2dData, &pixelBytes, tcDesc.Color, tcDesc.Size, tcDesc.ColorSize);
		pTex2dData = &tex2dData;
	}

	if (FAILED(hr = device->CreateTexture2D(&tex2dDesc, pTex2dData, out_texture)))
	{
		FailedReleaseVariables();
		return hr;
	}

	if (trDesc.CreateSRV && FAILED(hr = device->CreateShaderResourceView(*out_texture, &srvDesc, out_srv)))
	{
		FailedReleaseVariables();
		return hr;
	}

	if (trDesc.CreateRTV && FAILED(hr = device->CreateRenderTargetView(*out_texture, &rtvDesc, out_rtv)))
	{
		FailedReleaseVariables();
		return hr;
	}

	SafeDeleteArray(pixelBytes);

	return S_OK;
}

HRESULT DxTextureUtility::LoadTexture2D(
	Com<ID3D11Device> device, 
	const TextureLoadDesc& tlDesc,
	const TextureResourceDesc& trDesc,
	ID3D11Texture2D** out_texture, 
	ID3D11ShaderResourceView** out_srv, 
	ID3D11RenderTargetView** out_rtv)
{
	HRESULT hr = S_OK;
	DirectX::ScratchImage scratchImage;
	D3D11_TEXTURE2D_DESC tex2dDesc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};

	auto FailedReleaseVariables = [&]()
	{
		if (out_texture) SafeRelease(*out_texture);
		if (out_srv) SafeRelease(*out_srv);
		if (out_rtv) SafeRelease(*out_rtv);
	};

	if (out_texture) *out_texture = nullptr;
	if (out_srv) *out_srv = nullptr;
	if (out_rtv) *out_rtv = nullptr;
	
	if (FAILED(hr = LoadScratchImage(tlDesc.Path, tlDesc.Option.GenerateMipmap, &scratchImage)))
	{
		return hr;
	}
	
	if (FAILED(hr = DirectX::CreateTextureEx(
		device.Get(),
		scratchImage.GetImages(),
		scratchImage.GetImageCount(),
		scratchImage.GetMetadata(),
		(D3D11_USAGE)tlDesc.Option.Usage,
		(trDesc.CreateSRV ? D3D11_BIND_SHADER_RESOURCE : 0) | (trDesc.CreateRTV ? D3D11_BIND_RENDER_TARGET : 0),
		(tlDesc.Option.CPURead ? D3D11_CPU_ACCESS_READ : 0) | (tlDesc.Option.CPUWrite ? D3D11_CPU_ACCESS_WRITE : 0),
		0,
		false,
		(ID3D11Resource**)out_texture)))
	{
		FailedReleaseVariables();
		return hr;
	}

	(*out_texture)->GetDesc(&tex2dDesc);

	srvDesc.Format = tex2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = tex2dDesc.MipLevels;

	rtvDesc.Format = tex2dDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	if (trDesc.CreateSRV && FAILED(hr = device->CreateShaderResourceView(*out_texture, &srvDesc, out_srv)))
	{
		FailedReleaseVariables();
		return hr;
	}

	if (trDesc.CreateRTV && FAILED(hr = device->CreateRenderTargetView(*out_texture, &rtvDesc, out_rtv)))
	{
		FailedReleaseVariables();
		return hr;
	}

	return S_OK;
}

HRESULT DxTextureUtility::LoadScratchImage(const TCHAR* Path, bool generateMipmap, DirectX::ScratchImage* out_scratchImage)
{
	HRESULT hr = S_OK;

	// Convert to lower alphabets.
	tstring tstrPath = Path;
	std::transform(tstrPath.begin(), tstrPath.end(), tstrPath.begin(), ::tolower);
	fs::path lowerPath = tstrPath;
	tstring extension = lowerPath.extension();

	// There is no extension.
	if (!lowerPath.has_extension())
		return E_FAIL;

	if (extension == TEXT(".dds"))
	{
		hr = DirectX::LoadFromDDSFile(lowerPath.wstring().c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *out_scratchImage);
	}
	else if (extension == TEXT(".tga"))
	{
		hr = DirectX::LoadFromTGAFile(lowerPath.wstring().c_str(), nullptr, *out_scratchImage);
	}
	else
	{
		hr = DirectX::LoadFromWICFile(lowerPath.wstring().c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *out_scratchImage);
	}

	if (FAILED(hr))
	{
		return hr;
	}

	if (generateMipmap)
	{
		// levels: Number of mip map levels including base. A value of 0 indicates creating a full mipmap chain down to 1x1
		DirectX::ScratchImage mipmapScratchImage;
		//TEX_FILTER_DEFAULT
		hr = DirectX::GenerateMipMaps(out_scratchImage->GetImages(), out_scratchImage->GetImageCount(), out_scratchImage->GetMetadata(), TEX_FILTER_MIRROR | TEX_FILTER_MIRROR_U | TEX_FILTER_MIRROR_V, 0, mipmapScratchImage);
		*out_scratchImage = std::move(mipmapScratchImage);
	}

	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

void DxTextureUtility::SetTexture2DPixels(D3D11_SUBRESOURCE_DATA* inout_tex2dData, Byte** out_bytes, Color color, uint2 size, TextureCreationColorSize colorSize)
{
	uint numPixels = size.x * size.y;
	uint sizePixels = numPixels * (uint)colorSize / 8;
	Byte* arrPixelColors = new Byte[sizePixels];
	uint rowPitch = size.x * (uint)colorSize / 8;

	for (uint y = 0; y < size.y; ++y)
	{
		for (uint x = 0; x < size.x; ++x)
		{
			uint address = TexCoordToAddress(uint2(x, y), rowPitch);
			Byte* dst = arrPixelColors + address;

			b128* b128dst = (b128*)(arrPixelColors) + x + y * size.x;
			uint64* uint64dst = (uint64*)(arrPixelColors) + x + y * size.x;
			uint32* uint32dst = (uint32*)(arrPixelColors) + x + y * size.x;
			uint16* uint16dst = (uint16*)(arrPixelColors) + x + y * size.x;
			uint8* uint8dst = (uint8*)(arrPixelColors) + x + y * size.x;

			switch (colorSize)
			{
				case TextureCreationColorSize::_128:
					*b128dst = color.ToB128();
					break;
				case TextureCreationColorSize::_64:
					*uint64dst = color.ToUint64();
					break;
				case TextureCreationColorSize::_32:
					*uint32dst = color.ToUint32();
					break;
				case TextureCreationColorSize::_16:
					*uint16dst = color.ToUint16();
					break;
				case TextureCreationColorSize::_8:
					*uint8dst = color.ToUint8();
					break;
			}
		}
	}

	inout_tex2dData->pSysMem = arrPixelColors;
	inout_tex2dData->SysMemPitch = rowPitch;

	// The distance(in bytes) from the beginning of one depth level to the next.
	// System - memory - slice pitch is only used for 3D texture data as it has no meaning for the other resource types.
	inout_tex2dData->SysMemSlicePitch = size.x * size.y * (uint)colorSize / 8;

	*out_bytes = arrPixelColors;
}
