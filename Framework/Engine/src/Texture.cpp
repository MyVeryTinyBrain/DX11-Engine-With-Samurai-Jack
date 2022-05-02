#include "EnginePCH.h"
#include "Texture.h"

Texture::Texture(
	ResourceManagement* management, bool managed, const tstring& path) :
	ResourceObject(management, managed, path)
{
}

bool Texture::RegenerateMipmap(Com<ID3D11DeviceContext> deviceContext)
{
	Com<ID3D11ShaderResourceView> srv = GetSRV();

	if (!deviceContext || !srv)
		return false;

	deviceContext->GenerateMips(srv.Get());

	return true;
}

