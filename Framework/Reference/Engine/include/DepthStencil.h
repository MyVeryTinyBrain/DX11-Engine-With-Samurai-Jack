#pragma once

#include "ITexture.h"

ENGINE_BEGIN

class ENGINE_API DepthStencil : public ITexture
{
public:

	enum class Type { WRITEONLY_DEPTH_STENCIL, SRV_DEPTH };

private:

	DepthStencil(
		ID3D11Resource* texture, ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* srv,
		const D3D11_TEXTURE2D_DESC& desc, DepthStencil::Type type);

public:

	~DepthStencil();

	static HRESULT Create(Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DepthStencil::Type type, DepthStencil** out_depthStencil);

public:

	bool Resize(Com<ID3D11Device> device, unsigned int width, unsigned int height);

	void Clear(Com<ID3D11DeviceContext> deviceContext);

public:

	Com<ID3D11Resource> GetTexture() const override;

	Com<ID3D11DepthStencilView> GetDSV() const;

	Com<ID3D11ShaderResourceView> GetSRV() const override;

	DepthStencil::Type GetType() const;

	unsigned int GetWidth() const;

	unsigned int GetHeight() const;

	_declspec(property(get = GetTexture)) Com<ID3D11Resource> texture;
	_declspec(property(get = GetDSV)) Com<ID3D11DepthStencilView> dsv;
	_declspec(property(get = GetSRV)) Com<ID3D11ShaderResourceView> srv;
	_declspec(property(get = GetType)) DepthStencil::Type type;
	_declspec(property(get = GetWidth)) unsigned int width;
	_declspec(property(get = GetHeight)) unsigned int height;

private:

	static HRESULT CreateTexture2D(
		Com<ID3D11Device> device, unsigned int width, unsigned int height, bool dynamic, DepthStencil::Type type,
		ID3D11Resource** out_texture, ID3D11DepthStencilView** out_dsv, ID3D11ShaderResourceView** out_srv, D3D11_TEXTURE2D_DESC* out_desc);

private:

	ID3D11Resource*					m_texture = nullptr;

	ID3D11DepthStencilView*			m_dsv = nullptr;

	ID3D11ShaderResourceView*		m_srv = nullptr;

	D3D11_TEXTURE2D_DESC			m_desc = {};

	DepthStencil::Type				m_type = DepthStencil::Type::WRITEONLY_DEPTH_STENCIL;
};

ENGINE_END
