#include "EnginePCH.h"
#include "Texture2D.h"
#include "ResourceManagement.h"
#include "System.h"
#include "GraphicSystem.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

Texture2D::Texture2D(
	ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName,
	ID3D11Resource* texture, ID3D11ShaderResourceView* srv,
	Format format, const D3D11_TEXTURE2D_DESC& desc) :
	Texture(management, managed, path, groupName),
	m_texture(texture), m_srv(srv),
	m_format(format), m_desc(desc)
{
}

Texture2D::~Texture2D()
{
	SafeRelease(m_texture);
	SafeRelease(m_srv);
}

ResourceRef<Texture2D> Texture2D::Copy() const
{
	if (!system || !system->graphicSystem || !system->graphicSystem->device || !system->graphicSystem->deviceContext)
		return nullptr;

	auto device = system->graphicSystem->device;
	auto deviceContext = system->graphicSystem->deviceContext;

	ID3D11Texture2D* src = nullptr;
	ID3D11Texture2D* dest = nullptr;
	ID3D11ShaderResourceView* destSRV = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(dest);
		SafeRelease(destSRV);
	};

	if (FAILED(m_texture->QueryInterface<ID3D11Texture2D>(&src)))
	{
		ReleaseVars();
		return nullptr;
	}

	if (!src)
	{
		ReleaseVars();
		return nullptr;
	}

	if (FAILED(device->CreateTexture2D(&m_desc, nullptr, &dest)))
	{
		ReleaseVars();
		return nullptr;
	}

	D3D11_TEXTURE2D_DESC desc = m_desc;
	if (desc.Usage == D3D11_USAGE_IMMUTABLE)
		desc.Usage = D3D11_USAGE_DEFAULT;

	deviceContext->CopyResource(dest, src);

	if (FAILED(device->CreateShaderResourceView(dest, nullptr, &destSRV)))
	{
		ReleaseVars();
		return nullptr;
	}

	return new Texture2D(management, false, path, TEXT(""), dest, destSRV, m_format, m_desc);
}

bool Texture2D::CopyTo(ResourceRef<Texture2D> destTex)
{
	if (width != destTex->width || height != destTex->height)
		return false;

	if (destTex->m_desc.Usage == D3D11_USAGE_IMMUTABLE)
		return false;

	auto deviceContext = system->graphicSystem->deviceContext;
	ID3D11Resource* src = m_texture;
	ID3D11Resource* dest = destTex->m_texture;
	deviceContext->CopyResource(dest, src);

	return true;
}

Com<ID3D11Resource> Texture2D::GetTexture() const
{
	return m_texture;
}

Com<ID3D11ShaderResourceView> Texture2D::GetSRV() const
{
	return m_srv;
}

unsigned int Texture2D::GetWidth() const
{
	return m_desc.Width;
}

unsigned int Texture2D::GetHeight() const
{
	return m_desc.Height;
}

ResourceRef<Texture2D> Texture2D::CreateManagedTexture2DFromFile(ResourceManagement* management, const tstring& path, bool warning)
{
	if(!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(path);
	if (find)
		return find;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::WIC;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(LoadTexture2DFromFile(management->GetSystem()->graphicSystem->device, path, &texture, &srv, &format, &desc)))
	{
		if (warning)
		{
			tstring error_message = TEXT("Failed to load texture: ") + path;
			ERROR_MESSAGE_NT(error_message.c_str());
		}
		return nullptr;
	}

	return new Texture2D(management, true, path, TEXT(""), texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateManagedTexture2DFromFile(ResourceManagement* management, const tstring& path, const tstring& groupName, bool warning)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(path);
	if (find)
		return find;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::WIC;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(LoadTexture2DFromFile(management->GetSystem()->graphicSystem->device, path, &texture, &srv, &format, &desc)))
	{
		if (warning)
		{
			tstring error_message = TEXT("Failed to load texture: ") + path;
			ERROR_MESSAGE_NT(error_message.c_str());
		}
		return nullptr;
	}

	return new Texture2D(management, true, path, groupName, texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateUnmanagedTexture2DFromFile(ResourceManagement* management, const tstring& path, bool warning)
{
	if (!management)
		return nullptr;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::WIC;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(LoadTexture2DFromFile(management->GetSystem()->graphicSystem->device, path, &texture, &srv, &format, &desc)))
	{
		if (warning)
		{
			tstring error_message = TEXT("Failed to load texture: ") + path;
			ERROR_MESSAGE_NT(error_message.c_str());
		}
		return nullptr;
	}

	return new Texture2D(management, false, path, TEXT(""), texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateManagedTexture2D(ResourceManagement* management, const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(resourceKey);
	if (find)
		return find;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::DDS;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(management->GetSystem()->graphicSystem->device, color, width, height, false, &texture, &srv, &format, &desc)))
		return nullptr;

	return new Texture2D(management, true, resourceKey, TEXT(""), texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateManagedTexture2D(ResourceManagement* management, const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height)
{
	if (!management)
		return nullptr;

	ResourceRef<Texture2D> find = management->Find(resourceKey);
	if (find)
		return find;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::DDS;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(management->GetSystem()->graphicSystem->device, color, width, height, false, &texture, &srv, &format, &desc)))
		return nullptr;

	return new Texture2D(management, true, resourceKey, groupName, texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateUnmanagedTexture2D(ResourceManagement* management, const Color& color, unsigned int width, unsigned int height)
{
	if (!management)
		return ResourceRef<Texture2D>();

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::DDS;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(management->GetSystem()->graphicSystem->device, color, width, height, false, &texture, &srv, &format, &desc)))
		return ResourceRef<Texture2D>();

	return new Texture2D(management, false, TEXT(""), TEXT(""), texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateManagedDynamicTexture2D(ResourceManagement* management, const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(resourceKey);
	if (find)
		return find;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::DDS;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(management->GetSystem()->graphicSystem->device, color, width, height, true, &texture, &srv, &format, &desc)))
		return nullptr;

	return new Texture2D(management, true, resourceKey, TEXT(""), texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateManagedDynamicTexture2D(ResourceManagement* management, const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(resourceKey);
	if (find)
		return find;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::DDS;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(management->GetSystem()->graphicSystem->device, color, width, height, true, &texture, &srv, &format, &desc)))
		return nullptr;

	return new Texture2D(management, true, resourceKey, groupName, texture, srv, format, desc);
}

ResourceRef<Texture2D> Texture2D::CreateUnmanagedDynamicTexture2D(ResourceManagement* management, const Color& color, unsigned int width, unsigned int height)
{
	if (!management)
		return nullptr;

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	Format format = Format::DDS;
	D3D11_TEXTURE2D_DESC desc = {};

	if (FAILED(CreateTexture2D(management->GetSystem()->graphicSystem->device, color, width, height, true, &texture, &srv, &format, &desc)))
		return nullptr;

	return new Texture2D(management, false, TEXT(""), TEXT(""), texture, srv, format, desc);
}

HRESULT Texture2D::LoadTexture2DFromFile(
	Com<ID3D11Device> device, const tstring& path, 
	ID3D11Resource** out_texture, ID3D11ShaderResourceView** out_srv, Format* out_format, D3D11_TEXTURE2D_DESC* out_desc)
{
	if (!device)
		return E_FAIL;

	if (!out_texture)
		return E_FAIL;

	if (!out_srv)
		return E_FAIL;

	if (!out_format)
		return E_FAIL;

	if (!out_desc)
		return E_FAIL;

	*out_texture = nullptr;
	*out_srv = nullptr;
	*out_format = Format::WIC;

	HRESULT hr = S_OK;

	// Convert to lower alphabets.
	tstring lowerPath = path;
	std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);
	fs::path p = lowerPath;

	// There is no extension.
	if (!p.has_extension())
		return E_FAIL;

	DirectX::ScratchImage scratchImage; 

	if (p.extension() == TEXT(".dds"))
	{
		hr = DirectX::LoadFromDDSFile(p.wstring().c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scratchImage);
		*out_format = Format::DDS;
	}
	else if (p.extension() == TEXT(".tga"))
	{
		hr = DirectX::LoadFromTGAFile(p.wstring().c_str(), nullptr, scratchImage);
		*out_format = Format::TGA;
	}
	else
	{
		hr = DirectX::LoadFromWICFile(p.wstring().c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratchImage);
		*out_format = Format::WIC;
	}

	// Failed to load.
	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	auto ReleaseVars = [&]()
	{
		SafeRelease(texture);
		SafeRelease(srv);
	};

	if (FAILED(hr = DirectX::CreateTexture(device.Get(), scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), (ID3D11Resource**)&texture)))
	{
		ReleaseVars();
		return hr;
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	texture->GetDesc(&textureDesc);

	if (FAILED(hr = device->CreateShaderResourceView(texture, nullptr, &srv)))
	{
		ReleaseVars();
		return hr;
	}

	*out_texture = texture;
	*out_srv = srv;
	*out_desc = textureDesc;

	return S_OK;

}

HRESULT Texture2D::CreateTexture2D(
	Com<ID3D11Device> device, const Color& color, unsigned int width, unsigned int height, bool dynamic, 
	ID3D11Resource** out_texture, ID3D11ShaderResourceView** out_srv, Format* out_format, D3D11_TEXTURE2D_DESC* out_desc)
{
	if (!device)
		return E_FAIL;

	if (!out_texture)
		return E_FAIL;

	if (!out_srv)
		return E_FAIL;

	if (!out_format)
		return E_FAIL;

	if (!out_desc)
		return E_FAIL;

	if (width * height == 0)
		return E_FAIL;

	*out_texture = nullptr;
	*out_srv = nullptr;
	*out_format = Format::DDS;

	HRESULT hr = S_OK;
	uint32_t pixelColor = color;
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
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
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
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(hr = device->CreateShaderResourceView(texture, &srvDesc, &srv)))
	{
		ReleaseVars();
		return hr;
	}

	*out_texture = texture;
	*out_srv = srv;
	*out_desc = textureDesc;

	return S_OK;
}
