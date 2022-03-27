#pragma once

#include "ShaderTypes.h"

ENGINE_BEGIN
class ShaderVariableInfo;
class ShaderVariable
{
public:

	ShaderVariable(const ShaderVariableInfo* info);
	~ShaderVariable();

public:

	void Apply();

public:

	void SetSRV(Com<ID3D11ShaderResourceView> arrSRV);
	void SetSRVByIndex(Com<ID3D11ShaderResourceView> srv, uint index);
	void SetArrSRV(Com<ID3D11ShaderResourceView>* arrSRV, uint count);
	void SetRawValue(const void* src, size_t size);

	Com<ID3D11ShaderResourceView> GetSRV() const;
	Com<ID3D11ShaderResourceView> GetSRVByIndex(uint index) const;
	Com<ID3D11ShaderResourceView>* GetArrSRV() const;
	void GetRawValue(void* out_ptr) const;
	size_t GetRawValueSize() const;
	uint GetElementCount() const;

	inline const ShaderVariableInfo* GetInfo() const { return m_info; }

	_declspec(property(get = GetSRV)) Com<ID3D11ShaderResourceView> srv;
	_declspec(property(get = GetArrSRV)) Com<ID3D11ShaderResourceView>* arrSRV;
	_declspec(property(get = GetRawValueSize)) size_t rawValueSize;
	_declspec(property(get = GetElementCount)) uint elementCount;

	_declspec(property(get = GetInfo)) const ShaderVariableInfo* info;

public:

	bool IsSRV() const;
	bool IsRawValue() const;
	bool IsArray() const;

	_declspec(property(get = IsSRV)) bool isSRV;
	_declspec(property(get = IsRawValue)) bool isRawValue;
	_declspec(property(get = IsArray)) bool isArray;

private:

	const ShaderVariableInfo*		m_info = nullptr;
	Com<ID3D11ShaderResourceView>	m_srv = nullptr;
	Com<ID3D11ShaderResourceView>*	m_arrSRV = {};
	ID3D11ShaderResourceView**		m_arrSRVPtr = {};
	Byte*							m_valueBytes = nullptr;
	size_t							m_valueBytesSize = 0;
};

ENGINE_END
