#include "EnginePCH.h"
#include "Texture2D.h"
#include "ResourceManagement.h"
#include "System.h"
#include "GraphicSystem.h"
#include "DxTextureUtility.h"

Texture2D::Texture2D(
	ResourceManagement* management, bool managed, const tstring& path, 
	ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv) :
	Texture(management, managed, path),
	m_texture(texture), m_srv(srv)
{
}

Texture2D::~Texture2D()
{
	SafeRelease(m_texture);
	SafeRelease(m_srv);
}

ResourceRef<Texture2D> Texture2D::Copy(Com<ID3D11DeviceContext> deviceContext) const
{
	if (!system || !system->graphic || !system->graphic->device || !deviceContext)
		return nullptr;

	auto device = system->graphic->device;

	D3D11_TEXTURE2D_DESC desc = {};
	ID3D11Texture2D* dest = nullptr;
	ID3D11ShaderResourceView* destSRV = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(dest);
		SafeRelease(destSRV);
	};

	texture2D->GetDesc(&desc);
	if (desc.Usage == D3D11_USAGE_IMMUTABLE)
		desc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(device->CreateTexture2D(&desc, nullptr, &dest)))
	{
		ReleaseVars();
		return nullptr;
	}

	deviceContext->CopyResource(dest, texture2D.Get());

	if (FAILED(device->CreateShaderResourceView(dest, nullptr, &destSRV)))
	{
		ReleaseVars();
		return nullptr;
	}

	return new Texture2D(management, false, path, dest, destSRV);
}

bool Texture2D::CopyTo(Com<ID3D11DeviceContext> deviceContext, ResourceRef<Texture2D> destTex)
{
	if (!deviceContext)
		return false;

	if (width != destTex->width || height != destTex->height)
		return false;

	D3D11_TEXTURE2D_DESC desc = {};
	destTex->texture2D->GetDesc(&desc);

	if (desc.Usage == D3D11_USAGE_IMMUTABLE)
		return false;

	ID3D11Resource* dest = destTex->texture2D.Get();
	deviceContext->CopyResource(dest, texture2D.Get());

	return true;
}

unsigned int Texture2D::GetWidth() const
{
	D3D11_TEXTURE2D_DESC desc = {};
	texture2D->GetDesc(&desc);
	return desc.Width;
}

unsigned int Texture2D::GetHeight() const
{
	D3D11_TEXTURE2D_DESC desc = {};
	texture2D->GetDesc(&desc);
	return desc.Height;
}

ResourceRef<Texture2D> Texture2D::CreateTexture2DM(ResourceManagement* management, const TextureCreateDesc& desc, const tstring& resourceKey)
{
	if (!management)
		return nullptr;

	ResourceRef<Texture2D> find = management->Find(resourceKey);
	if (find)
		return find;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	if (FAILED(CreateTexture2D(management->GetSystem()->graphic->device, desc, &texture, &srv)))
		return nullptr;

	return new Texture2D(management, true, resourceKey, texture, srv);
}

ResourceRef<Texture2D> Texture2D::CreateTexture2DUM(ResourceManagement* management, const TextureCreateDesc& desc)
{
	if (!management)
		return nullptr;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	if (FAILED(CreateTexture2D(management->GetSystem()->graphic->device, desc, &texture, &srv)))
		return nullptr;

	return new Texture2D(management, false, TEXT(""), texture, srv);
}

ResourceRef<Texture2D> Texture2D::LoadTexture2DM(ResourceManagement* management, const TextureOptionDesc& desc, const tstring& path)
{
	if (!management)
		return nullptr;

	ResourceRef<Texture2D> find = management->Find(path);
	if (find)
		return find;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	TextureLoadDesc tlDesc = {};
	_tcscpy_s(tlDesc.Path, path.c_str());
	tlDesc.Option = desc;

	if (FAILED(LoadTexture2D(management->GetSystem()->graphic->device, tlDesc, &texture, &srv)))
		return nullptr;

	return new Texture2D(management, true, path, texture, srv);
}

ResourceRef<Texture2D> Texture2D::LoadTexture2DUM(ResourceManagement* management, const TextureOptionDesc& desc, const tstring& path)
{
	if (!management)
		return nullptr;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	TextureLoadDesc tlDesc = {};
	_tcscpy_s(tlDesc.Path, path.c_str());
	tlDesc.Option = desc;

	if (FAILED(LoadTexture2D(management->GetSystem()->graphic->device, tlDesc, &texture, &srv)))
		return nullptr;

	return new Texture2D(management, false, TEXT(""), texture, srv);
}

HRESULT Texture2D::CreateTexture2D(Com<ID3D11Device> device, const TextureCreateDesc& desc, ID3D11Texture2D** out_texture, ID3D11ShaderResourceView** out_srv)
{
	TextureResourceDesc trDesc = {};
	trDesc.CreateSRV = true;
	return DxTextureUtility::CreateTexture2D(device, desc, trDesc, out_texture, out_srv, nullptr);
}

HRESULT Texture2D::LoadTexture2D(Com<ID3D11Device> device, const TextureLoadDesc& desc, ID3D11Texture2D** out_texture, ID3D11ShaderResourceView** out_srv)
{
	TextureResourceDesc trDesc = {};
	trDesc.CreateSRV = true;
	return DxTextureUtility::LoadTexture2D(device, desc, trDesc, out_texture, out_srv, nullptr);
}
