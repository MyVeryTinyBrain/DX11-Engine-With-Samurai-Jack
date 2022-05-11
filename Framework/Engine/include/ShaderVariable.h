#pragma once

#include "ShaderType.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Material;
class ShaderVariableInfo;
class Texture;
class ShaderVariable
{
public:

	ShaderVariable(Material* material, const ShaderVariableInfo* info);
	ShaderVariable(const ShaderVariable& other);
	~ShaderVariable();

public:

	void Apply();
	ShaderVariable* Copy() const;

public:

	void SetTexture(ResourceRef<Texture> texture);
	void SetTextureByIndex(ResourceRef<Texture> texture, uint index);
	void SetTextures(ResourceRef<Texture>* textures, uint count);
	void SetRawValue(const void* src, size_t size);
	void SetBool(bool value, uint index);
	void SetInt(int value, uint index);
	void SetUInt(uint value, uint index);
	void SetFloat(float value, uint index);
	void SetDouble(double value, uint index);
	void SetVector(const V4& value, uint index);
	void SetMatrix(const M4& value, uint index);

	ResourceRef<Texture> GetTexture() const;
	ResourceRef<Texture> GetTextureByIndex(uint index) const;
	ResourceRef<Texture>* GetTextures() const;
	void GetRawValue(void* out_ptr) const;
	void GetRawValue(void* out_ptr, size_t size) const;
	const void* GetConstRawValuePointer() const { return m_valueBytes; }
	size_t GetRawValueSize() const;
	uint GetElementCount() const;
	uint GetArrayCount() const;

	inline const ShaderVariableInfo* GetInfo() const { return m_info; }

	_declspec(property(get = GetTexture)) ResourceRef<Texture> texture;
	_declspec(property(get = GetTextures)) ResourceRef<Texture>* textures;
	_declspec(property(get = GetConstRawValuePointer)) const void* constRawValuePointer;
	_declspec(property(get = GetRawValueSize)) size_t rawValueSize;
	_declspec(property(get = GetElementCount)) uint elementCount;
	_declspec(property(get = GetArrayCount)) uint arrayCount;

	_declspec(property(get = GetInfo)) const ShaderVariableInfo* info;

public:

	bool IsTexture() const;
	bool IsRawValue() const;
	bool IsArray() const;

	_declspec(property(get = IsTexture)) bool isTexture;
	_declspec(property(get = IsRawValue)) bool isRawValue;
	_declspec(property(get = IsArray)) bool isArray;

private:

	Material*					m_material = nullptr;
	const ShaderVariableInfo*	m_info = nullptr;
	ResourceRef<Texture>*		m_textures = {};
	ID3D11ShaderResourceView**	m_arrSRV = {};
	Byte*						m_valueBytes = nullptr;
	size_t						m_valueBytesSize = 0;
};

ENGINE_END
