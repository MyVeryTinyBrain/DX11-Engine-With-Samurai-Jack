#pragma once

ENGINE_BEGIN

class ENGINE_API ITexture
{
public:

	ITexture() = default;

	virtual ~ITexture() = default;

public:

	virtual Com<ID3D11Resource> GetTexture() const = 0;

	virtual Com<ID3D11ShaderResourceView> GetSRV() const = 0;
};

ENGINE_END
