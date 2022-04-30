#pragma once

#include "DxTextureCreateType.h"

ENGINE_BEGIN

namespace DxTextureUtility
{
	HRESULT CreateTexture2D(
		Com<ID3D11Device> device,
		const TextureCreateDesc& tcDesc,
		const TextureResourceDesc& trDesc,
		ID3D11Texture2D** out_texture,
		ID3D11ShaderResourceView** out_srv,
		ID3D11RenderTargetView** out_rtv
	);

	HRESULT LoadTexture2D(
		Com<ID3D11Device> device,
		const TextureLoadDesc& tlDesc,
		const TextureResourceDesc& trDesc,
		ID3D11Texture2D** out_texture,
		ID3D11ShaderResourceView** out_srv,
		ID3D11RenderTargetView** out_rtv
	);

	HRESULT LoadScratchImage(const TCHAR* Path, bool generateMipmap, DirectX::ScratchImage* out_scratchImage);

	void SetTexture2DPixels(D3D11_SUBRESOURCE_DATA* inout_tex2dData, Byte** out_bytes, Color color, uint2 size, TextureCreationColorSize colorSize);

	inline uint TexCoordToAddress(uint2 texCoord, uint rowPitch) { return texCoord.y * rowPitch + (texCoord.x * 4); }
};

ENGINE_END
