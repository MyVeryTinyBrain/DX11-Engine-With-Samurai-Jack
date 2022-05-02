#pragma once

#include "DxTextureCreateType.h"

ENGINE_BEGIN

namespace DxTextureUtility
{
	ENGINE_API HRESULT CreateTexture2D(
		Com<ID3D11Device> device,
		const TextureCreateDesc& tcDesc,
		const TextureResourceDesc& trDesc,
		ID3D11Texture2D** out_texture,
		ID3D11ShaderResourceView** out_srv,
		ID3D11RenderTargetView** out_rtv
	);

	ENGINE_API HRESULT LoadTexture2D(
		Com<ID3D11Device> device,
		const TextureLoadDesc& tlDesc,
		const TextureResourceDesc& trDesc,
		ID3D11Texture2D** out_texture,
		ID3D11ShaderResourceView** out_srv,
		ID3D11RenderTargetView** out_rtv
	);

	ENGINE_API HRESULT LoadScratchImage(const TCHAR* Path, bool generateMipmap, uint mipLevle, DirectX::ScratchImage* out_scratchImage);

	ENGINE_API void SetTexture2DPixels(D3D11_SUBRESOURCE_DATA* inout_tex2dData, Byte** out_bytes, Color color, uint2 size, TextureCreationColorSize colorSize);

	ENGINE_API uint TexCoordToAddress(uint2 texCoord, uint rowPitch);
};

ENGINE_END
