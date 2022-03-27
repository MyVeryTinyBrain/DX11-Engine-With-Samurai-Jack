#include "EnginePCH.h"
#include "ShaderVariable.h"
#include "ShaderVariableInfo.h"

ShaderVariable::ShaderVariable(const ShaderVariableInfo* info)
{
	m_info = info;
	m_valueBytesSize = info->Size;

	if (m_info->IsRawValue())
	{
		m_valueBytes = new Byte[info->Size]{};
	}
	else if (m_info->IsSRV() && m_info->IsArray())
	{
		m_arrSRV = new Com<ID3D11ShaderResourceView>[m_info->Elements]{};
		m_arrSRVPtr = new ID3D11ShaderResourceView* [m_info->Elements]{};
	}
}

ShaderVariable::~ShaderVariable()
{
	SafeDeleteArray(m_arrSRV);
	SafeDeleteArray(m_arrSRVPtr);
	SafeDeleteArray(m_valueBytes);
}

void ShaderVariable::Apply()
{
	ID3DX11EffectStringVariable* hString = nullptr;
	ID3DX11EffectScalarVariable* hScalar = nullptr;
	ID3DX11EffectVectorVariable* hVector = nullptr;
	ID3DX11EffectMatrixVariable* hMatrix = nullptr;
	ID3DX11EffectShaderResourceVariable* hSRV = nullptr;

	switch (m_info->Type)
	{
		case ShaderVariableType::Unknown:
		case ShaderVariableType::Other:
			break;
		case ShaderVariableType::Struct:
			m_info->Handle->SetRawValue(m_valueBytes, 0, (uint32_t)m_valueBytesSize);
			break;
		case ShaderVariableType::String:
			hString = m_info->Handle->AsString();
			if (!hString->IsValid()) break;
			hString->SetRawValue(m_valueBytes, 0, (uint32_t)m_valueBytesSize);
			break;
		case ShaderVariableType::Bool:
			hScalar = m_info->Handle->AsScalar();
			if (!hScalar->IsValid()) break;
			hScalar->SetBool(*((bool*)m_valueBytes));
			break;
		case ShaderVariableType::Int:
			hScalar = m_info->Handle->AsScalar();
			if (!hScalar->IsValid()) break;
			hScalar->SetInt(*((int*)m_valueBytes));
			break;
		case ShaderVariableType::UInt:
			hScalar = m_info->Handle->AsScalar();
			if (!hScalar->IsValid()) break;
			hScalar->SetRawValue(m_valueBytes, 0, (uint32_t)m_valueBytesSize);
			break;
		case ShaderVariableType::Float:
			hScalar = m_info->Handle->AsScalar();
			if (!hScalar->IsValid()) break;
			hScalar->SetFloat(*((float*)m_valueBytes));
			break;
		case ShaderVariableType::Vector:
			hVector = m_info->Handle->AsVector();
			if (!hVector->IsValid()) break;
			hVector->SetFloatVector((float*)m_valueBytes);
			break;
		case ShaderVariableType::Matrix:
			hMatrix = m_info->Handle->AsMatrix();
			if (!hMatrix->IsValid()) break;
			hMatrix->SetMatrix((float*)m_valueBytes); // Doesn't Transpose Matrix
			break;
		case ShaderVariableType::Texture1D:
		case ShaderVariableType::Texture2D:
		case ShaderVariableType::Texture3D:
		case ShaderVariableType::Texture1DArray:
		case ShaderVariableType::Texture2DArray:
			hSRV = m_info->Handle->AsShaderResource();
			if (!hSRV->IsValid()) break;
			if (!m_info->IsArray())
			{
				hSRV->SetResource(m_srv.Get());
			}
			else
			{
				for (uint i = 0; i < m_info->Elements; ++i)
					m_arrSRVPtr[i] = m_arrSRV[i].Get();
				hSRV->SetResourceArray(m_arrSRVPtr, 0, m_info->Elements);
			}
			break;
		default:
			break;
	}
}

void ShaderVariable::SetSRV(Com<ID3D11ShaderResourceView> srv)
{
	m_srv = srv;
}

void ShaderVariable::SetSRVByIndex(Com<ID3D11ShaderResourceView> srv, uint index)
{
	m_arrSRV[index] = srv;
}

void ShaderVariable::SetArrSRV(Com<ID3D11ShaderResourceView>* arrSRV, uint count)
{
	for (uint i = 0; i < count; ++i)
		m_arrSRV[i] = arrSRV[i];
}

void ShaderVariable::SetRawValue(const void* src, size_t size)
{
	memcpy(m_valueBytes, src, size);
}

Com<ID3D11ShaderResourceView> ShaderVariable::GetSRV() const
{
	return m_srv;
}

Com<ID3D11ShaderResourceView> ShaderVariable::GetSRVByIndex(uint index) const
{
	return m_arrSRV[index];
}

Com<ID3D11ShaderResourceView>* ShaderVariable::GetArrSRV() const
{
	return m_arrSRV;
}

void ShaderVariable::GetRawValue(void* out_ptr) const
{
	memcpy(out_ptr, m_valueBytes, m_valueBytesSize);
}

size_t ShaderVariable::GetRawValueSize() const
{
	return m_info->Size;
}

uint ShaderVariable::GetElementCount() const
{
	return m_info->Elements;
}

bool ShaderVariable::IsSRV() const
{
	return m_info->IsSRV();

}

bool ShaderVariable::IsRawValue() const
{
	return m_info->IsRawValue();

}

bool ShaderVariable::IsArray() const
{
	return m_info->IsArray();
}
