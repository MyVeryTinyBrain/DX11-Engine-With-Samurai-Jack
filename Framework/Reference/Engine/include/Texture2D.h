#pragma once

#include "Texture.h"

ENGINE_BEGIN

class ENGINE_API Texture2D : public Texture
{
public:

	enum class Format { DDS, TGA, WIC };

protected:

	Texture2D(
		ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName,
		ID3D11Resource* texture, ID3D11ShaderResourceView* srv, 
		Format format, const D3D11_TEXTURE2D_DESC& desc);

	virtual ~Texture2D();

public:

	// Copy to unmanaged Texture2D
	ResourceRef<Texture2D> Copy() const;

	// Copy to the dest Texture2D of the same size.
	// Fail to copy if their sizes are different.
	// Fail to copy if destTex is D3D11_USAGE_IMMUTABLE.
	bool CopyTo(ResourceRef<Texture2D> destTex);

public:

	virtual Com<ID3D11Resource> GetTexture() const override;

	virtual Com<ID3D11ShaderResourceView> GetSRV() const override;

	unsigned int GetWidth() const;

	unsigned int GetHeight() const;

	_declspec(property(get = GetWidth)) unsigned int width;
	_declspec(property(get = GetHeight)) unsigned int height;

public:

	static ResourceRef<Texture2D> CreateManagedTexture2DFromFile(ResourceManagement* management, const tstring& path, bool warning = true);

	static ResourceRef<Texture2D> CreateManagedTexture2DFromFile(ResourceManagement* management, const tstring& path, const tstring& groupName, bool warning = true);

	static ResourceRef<Texture2D> CreateUnmanagedTexture2DFromFile(ResourceManagement* management, const tstring& path, bool warning = true);

	static ResourceRef<Texture2D> CreateManagedTexture2D(ResourceManagement* management, const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height);

	static ResourceRef<Texture2D> CreateManagedTexture2D(ResourceManagement* management, const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height);

	static ResourceRef<Texture2D> CreateUnmanagedTexture2D(ResourceManagement* management, const Color& color, unsigned int width, unsigned int height);

	static ResourceRef<Texture2D> CreateManagedDynamicTexture2D(ResourceManagement* management, const tstring& resourceKey, const Color& color, unsigned int width, unsigned int height);

	static ResourceRef<Texture2D> CreateManagedDynamicTexture2D(ResourceManagement* management, const tstring& resourceKey, const tstring& groupName, const Color& color, unsigned int width, unsigned int height);

	static ResourceRef<Texture2D> CreateUnmanagedDynamicTexture2D(ResourceManagement* management, const Color& color, unsigned int width, unsigned int height);

protected:

	static HRESULT LoadTexture2DFromFile(
		Com<ID3D11Device> device, const tstring& path, 
		ID3D11Resource** out_texture, ID3D11ShaderResourceView** out_srv, Format* out_format, D3D11_TEXTURE2D_DESC* out_desc);

	static HRESULT CreateTexture2D(
		Com<ID3D11Device> device, const Color& color, unsigned int width, unsigned int height, bool dynamic, 
		ID3D11Resource** out_texture, ID3D11ShaderResourceView** out_srv, Format* out_format, D3D11_TEXTURE2D_DESC* out_desc);

private:

	ID3D11Resource*					m_texture = nullptr;

	ID3D11ShaderResourceView*		m_srv = nullptr;

	Format							m_format = Format::WIC;

	D3D11_TEXTURE2D_DESC			m_desc = {};
};

ENGINE_END
