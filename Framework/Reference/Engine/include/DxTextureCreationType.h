#pragma once

ENGINE_BEGIN

enum class TextureCreationColorSize
{
	_128 = 128,
	_64 = 64,
	_32 = 32,
	_16 = 16,
	_8 = 8,
};

enum class TextureCreationUsage
{
	Default = D3D11_USAGE_DEFAULT,
	Dynamic = D3D11_USAGE_DYNAMIC,
	Immutable = D3D11_USAGE_IMMUTABLE,
};

struct TextureOptionDesc
{
	TextureCreationUsage		Usage;
	bool						CPURead;
	bool						CPUWrite;
	bool						GenerateMipmap;
};

struct TextureResourceDesc
{
	bool						CreateSRV;
	bool						CreateRTV;
};

struct TextureCreateDesc
{
	bool						SetColor;
	Color						Color;
	// Ex)
	// FLOAT_A32R32G32B32 -> 128
	// FLOAT_R32G32B32 -> 128
	// UNORM_A16R16G16B16 -> 32
	TextureCreationColorSize	ColorSize;
	uint2						Size;
	DXGI_FORMAT					Format;
	TextureOptionDesc			Option;
};

struct TextureLoadDesc
{
	TCHAR						Path[MAX_PATH];
	TextureOptionDesc			Option;
};

ENGINE_END

