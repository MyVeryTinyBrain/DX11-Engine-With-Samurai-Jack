#pragma once

ENGINE_BEGIN

class ENGINE_API VTFTexture final
{
private:

	VTFTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv, uint2 size);

public:

	static VTFTexture* Create(Com<ID3D11Device> device, uint matrixCount);
	~VTFTexture();
	void Release();

public:

	bool Resize(Com<ID3D11Device> device, uint matrixCount);
	bool Map(Com<ID3D11DeviceContext> deviceContext);
	bool SetMatrix(uint index, const M4& matrix);
	bool GetMatrix(uint index, M4& out_matrix) const;
	void Unmap(Com<ID3D11DeviceContext> deviceContext);

public:

	inline Com<ID3D11Resource> GetTexture() const { return m_texture; }
	inline Com<ID3D11ShaderResourceView> GetSRV() const { return m_srv; }
	inline unsigned int GetWidth() const { return m_size.x; }
	inline unsigned int GetHeight() const { return m_size.y; }
	inline unsigned int GetMatrixCount() const { return m_size.x; }

	_declspec(property(get = GetTexture)) Com<ID3D11Resource> texture;
	_declspec(property(get = GetSRV)) Com<ID3D11ShaderResourceView> srv;
	_declspec(property(get = GetWidth)) unsigned int width;
	_declspec(property(get = GetHeight)) unsigned int height;
	_declspec(property(get = GetMatrixCount)) unsigned int matrixCount;

private:

	static HRESULT CreateTexture2D(
		Com<ID3D11Device> device, unsigned int width, unsigned int height,
		ID3D11Texture2D** out_texture, ID3D11ShaderResourceView** out_srv);

	inline static uint TexCoordToAddress(uint2 texCoord, uint rowPitch) { return texCoord.y * rowPitch + (texCoord.x * 4); }

private:

	uint2						m_size = uint2(0, 0); // MatrixCount x 4
	ID3D11Texture2D*			m_texture = nullptr;
	ID3D11ShaderResourceView*	m_srv = nullptr;
	D3D11_MAPPED_SUBRESOURCE	m_resource = {};
};

ENGINE_END
