#include "EnginePCH.h"
#include "ShaderVariable.h"
#include "ShaderVariableInfo.h"
#include "Texture.h"
#include "Texture2D.h"
#include "Material.h"
#include "System.h"
#include "ResourceManagement.h"
#include "BuiltInResources.h"

ShaderVariable::ShaderVariable(Material* material, const ShaderVariableInfo* info)
{
	m_material = material;
	m_info = info;
	m_valueBytesSize = info->Size;

	if (m_info->IsRawValue())
	{
		m_valueBytes = new Byte[info->Size]{};
	}
	else if (m_info->IsTexture())
	{
		uint arrCount = m_info->ArrayCount;
		m_textures = new ResourceRef<Texture> [arrCount];
		m_arrSRV = new ID3D11ShaderResourceView* [arrCount]{};
	}
}

ShaderVariable::ShaderVariable(const ShaderVariable& other)
{
	m_material = other.m_material;
	m_info = other.info;
	m_valueBytesSize = other.m_valueBytesSize;

	if (other.isRawValue)
	{
		m_valueBytes = new Byte[m_valueBytesSize]{};
		memcpy(m_valueBytes, other.m_valueBytes, m_valueBytesSize);
	}
	else
	{
		uint arrCount = other.arrayCount;
		m_textures = new ResourceRef<Texture>[arrCount];
		m_arrSRV = new ID3D11ShaderResourceView * [arrCount];
		for (uint i = 0; i < arrCount; ++i)
			m_textures[i] = other.m_textures[i];
		memcpy(m_arrSRV, other.m_arrSRV, arrCount);
	}
}

ShaderVariable::~ShaderVariable()
{
	SafeDeleteArray(m_textures);
	SafeDeleteArray(m_arrSRV);
	SafeDeleteArray(m_valueBytes);
}

void ShaderVariable::Apply()
{
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
				if (!m_textures[0])
					m_textures[0] = m_material->system->resource->builtIn->whiteTexture;
				hSRV->SetResource(m_textures[0]->shaderResourceView.Get());
			}
			else
			{
				for (uint i = 0; i < m_info->Elements; ++i)
				{
					if (!m_textures[i])
						m_textures[i] = m_material->system->resource->builtIn->whiteTexture;
					m_arrSRV[i] = m_textures[i]->shaderResourceView.Get();
				}
				hSRV->SetResourceArray(m_arrSRV, 0, m_info->Elements);
			}
			break;
		default:
			break;
	}
}

ShaderVariable* ShaderVariable::Copy() const
{
	return new ShaderVariable(*this);
}

void ShaderVariable::SetTexture(ResourceRef<Texture> texture)
{
	m_textures[0] = texture;
}

void ShaderVariable::SetTextureByIndex(ResourceRef<Texture> texture, uint index)
{
	m_textures[index] = texture;
}

void ShaderVariable::SetTextures(ResourceRef<Texture>* textures, uint count)
{
	for (uint i = 0; i < count; ++i)
		m_textures[i] = textures[i];
}

void ShaderVariable::SetRawValue(const void* src, size_t size)
{
	memcpy(m_valueBytes, src, size);
}

void ShaderVariable::SetBool(bool value, uint index)
{
	((bool*)m_valueBytes)[index] = value;
}

void ShaderVariable::SetInt(int value, uint index)
{
	((int*)m_valueBytes)[index] = value;
}

void ShaderVariable::SetUInt(uint value, uint index)
{
	((uint*)m_valueBytes)[index] = value;
}

void ShaderVariable::SetFloat(float value, uint index)
{
	((float*)m_valueBytes)[index] = value;
}

void ShaderVariable::SetDouble(double value, uint index)
{
	((double*)m_valueBytes)[index] = value;
}

void ShaderVariable::SetVector(const V4& value, uint index)
{
	((V4*)m_valueBytes)[index] = value;
}

void ShaderVariable::SetMatrix(const M4& value, uint index)
{
	((M4*)m_valueBytes)[index] = value;
}

ResourceRef<Texture> ShaderVariable::GetTexture() const
{
	return m_textures[0];
}

ResourceRef<Texture> ShaderVariable::GetTextureByIndex(uint index) const
{
	return m_textures[index];
}

ResourceRef<Texture>* ShaderVariable::GetTextures() const
{
	return m_textures;
}

void ShaderVariable::GetRawValue(void* out_ptr) const
{
	memcpy(out_ptr, m_valueBytes, m_valueBytesSize);
}

void ShaderVariable::GetRawValue(void* out_ptr, size_t size) const
{
	memcpy(out_ptr, m_valueBytes, Clamp((uint)size, 0u, (uint)m_valueBytesSize));
}

size_t ShaderVariable::GetRawValueSize() const
{
	return m_info->Size;
}

uint ShaderVariable::GetElementCount() const
{
	return m_info->Elements;
}

uint ShaderVariable::GetArrayCount() const
{
	return m_info->ArrayCount;
}

bool ShaderVariable::IsTexture() const
{
	return m_info->IsTexture();

}

bool ShaderVariable::IsRawValue() const
{
	return m_info->IsRawValue();

}

bool ShaderVariable::IsArray() const
{
	return m_info->IsArray();
}
