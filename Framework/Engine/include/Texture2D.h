#pragma once

#include "Texture.h"
#include "DxTextureCreateType.h"

ENGINE_BEGIN

class ENGINE_API Texture2D : public Texture
{
protected:

	Texture2D(
		ResourceManagement* management, bool managed, const tstring& path,
		ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv);

	virtual ~Texture2D();

public:

	// Copy to unmanaged Texture2D
	ResourceRef<Texture2D> Copy(Com<ID3D11DeviceContext> deviceContext) const;

	// Copy to the dest Texture2D of the same size.
	// Fail to copy if their sizes are different.
	// Fail to copy if destTex is D3D11_USAGE_IMMUTABLE.
	bool CopyTo(Com<ID3D11DeviceContext> deviceContext, ResourceRef<Texture2D> destTex);

public:

	virtual Com<ID3D11Resource> GetTexture() const override { return m_texture; }
	virtual Com<ID3D11Texture2D> GetTexture2D() const { return m_texture; }
	virtual Com<ID3D11ShaderResourceView> GetSRV() const override { return m_srv; }
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	_declspec(property(get = GetTexture2D)) Com<ID3D11Texture2D> texture2D;
	_declspec(property(get = GetWidth)) unsigned int width;
	_declspec(property(get = GetHeight)) unsigned int height;

public:

	static ResourceRef<Texture2D> CreateTexture2DM(ResourceManagement* management, const TextureCreateDesc& desc, const tstring& resourceKey);
	static ResourceRef<Texture2D> CreateTexture2DUM(ResourceManagement* management, const TextureCreateDesc& desc);
	static ResourceRef<Texture2D> LoadTexture2DM(ResourceManagement* management, const TextureOptionDesc& desc, const tstring& path);
	static ResourceRef<Texture2D> LoadTexture2DUM(ResourceManagement* management, const TextureOptionDesc& desc, const tstring& path);

private:

	static HRESULT CreateTexture2D(Com<ID3D11Device> device, const TextureCreateDesc& desc, ID3D11Texture2D** out_texture, ID3D11ShaderResourceView** out_srv);
	static HRESULT LoadTexture2D(Com<ID3D11Device> device, const TextureLoadDesc& desc, ID3D11Texture2D** out_texture, ID3D11ShaderResourceView** out_srv);

private:

	ID3D11Texture2D*				m_texture = nullptr;
	ID3D11ShaderResourceView*		m_srv = nullptr;
};

ENGINE_END
