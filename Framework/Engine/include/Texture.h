#pragma once

#include "ResourceObject.h"
#include "ITexture.h"

ENGINE_BEGIN

class ENGINE_API Texture : public ResourceObject, public ITexture
{

protected:

	Texture(ResourceManagement* management, bool managed, const tstring& path);

	virtual ~Texture() = default;

public:

	virtual Com<ID3D11Resource> GetTexture() const override = 0;
	virtual Com<ID3D11ShaderResourceView> GetSRV() const override = 0;

	_declspec(property(get = GetTexture)) Com<ID3D11Resource> texture;
	_declspec(property(get = GetSRV)) Com<ID3D11ShaderResourceView> shaderResourceView;

public:

	bool RegenerateMipmap(Com<ID3D11DeviceContext> deviceContext);

};

ENGINE_END
