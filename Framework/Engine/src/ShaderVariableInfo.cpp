#include "EnginePCH.h"
#include "ShaderVariableInfo.h"

ShaderVariableInfo::ShaderVariableInfo(D3DX11_EFFECT_VARIABLE_DESC variableDesc, D3DX11_EFFECT_TYPE_DESC typeDesc, ID3DX11EffectVariable* handle)
{
	m_name = variableDesc.Name;
	m_semantic = variableDesc.Semantic != nullptr ? variableDesc.Semantic : "";
	m_type = TypeOf(typeDesc);
	m_size = typeDesc.PackedSize;
	m_elements = typeDesc.Elements;
	m_handle = handle;

	SetupAnnotations(variableDesc);
}

ShaderVariableInfo* ShaderVariableInfo::Create(ID3DX11EffectVariable* handle)
{
	if (!handle)
		return nullptr;

	ID3DX11EffectType* type = handle->GetType();
	D3DX11_EFFECT_VARIABLE_DESC variableDesc;
	D3DX11_EFFECT_TYPE_DESC typeDesc;

	HRESULT hr = S_OK;

	if (!type)
	{
		return nullptr;
	}
	if (FAILED(hr = handle->GetDesc(&variableDesc)))
	{
		SafeRelease(type);
		return nullptr;
	}
	if (FAILED(hr = type->GetDesc(&typeDesc)))
	{
		SafeRelease(type);
		return nullptr;
	}

	ShaderVariableInfo* info = new ShaderVariableInfo(variableDesc, typeDesc, handle);

	SafeRelease(type);

	return info;
}

ShaderVariableInfo::~ShaderVariableInfo()
{
	SafeRelease(m_handle);
}

ShaderVariableInfo& ShaderVariableInfo::operator=(const ShaderVariableInfo& rhs)
{
	m_name = rhs.m_name;
	m_semantic = rhs.m_semantic;
	m_type = rhs.m_type;
	m_size = rhs.m_size;
	m_elements = rhs.m_elements;
	m_handle = rhs.m_handle;
	rhs.m_handle->AddRef();

	return *this;
}

ShaderVariableInfo& ShaderVariableInfo::operator=(ShaderVariableInfo&& rhs) noexcept
{
	m_name = rhs.m_name;
	m_semantic = rhs.m_semantic;
	m_type = rhs.m_type;
	m_size = rhs.m_size;
	m_elements = rhs.m_elements;
	m_handle = rhs.m_handle;

	rhs.m_handle = nullptr;

	return *this;
}

bool ShaderVariableInfo::IsSRV() const
{
	switch (m_type)
	{
		case ShaderVariableType::Unknown:
		case ShaderVariableType::Other:
		case ShaderVariableType::Texture1D:
		case ShaderVariableType::Texture2D:
		case ShaderVariableType::Texture3D:
		case ShaderVariableType::Texture1DArray:
		case ShaderVariableType::Texture2DArray:
			return true;
	}

	return false;
}

void ShaderVariableInfo::SetupAnnotations(D3DX11_EFFECT_VARIABLE_DESC variableDesc)
{
	for (uint i = 0; i < variableDesc.Annotations; ++i)
	{
		ID3DX11EffectVariable* annotation = m_handle->GetAnnotationByIndex(i);
		ID3DX11EffectType* type = annotation->GetType();
		D3DX11_EFFECT_VARIABLE_DESC annotationVariableDesc = {};
		D3DX11_EFFECT_TYPE_DESC annotationTypeDesc = {};

		ID3DX11EffectStringVariable* hString = nullptr;
		ID3DX11EffectScalarVariable* hScalar = nullptr;
		ID3DX11EffectVectorVariable* hVector = nullptr;

		auto ReleaseVars = [&]()
		{
			SafeRelease(annotation);
			SafeRelease(type);
			SafeRelease(hString);
			SafeRelease(hScalar);
			SafeRelease(hVector);
		};

		if (!annotation || !type || FAILED(annotation->GetDesc(&annotationVariableDesc)) || FAILED(type->GetDesc(&annotationTypeDesc)))
		{
			ReleaseVars();
			continue;
		}

		Annotation info = {};

		info.name = annotationVariableDesc.Name;
		std::transform(info.name.begin(), info.name.end(), info.name.begin(), ::tolower);

		switch (annotationTypeDesc.Class)
		{
			case D3D_SVC_VECTOR:
				hVector = annotation->AsVector();
				if (!hVector->IsValid())
					continue;

				info.type = Annotation::Type::Vector;
				hVector->GetFloatVector((float*)&info.vector);
				break;
		}

		if (info.type == Annotation::Type::Unknown)
		{
			switch (annotationTypeDesc.Type)
			{
				case D3D_SVT_BOOL:
				case D3D_SVT_INT:
				case D3D_SVT_FLOAT:
				case D3D_SVT_UINT:
				case D3D_SVT_UINT8:
				case D3D_SVT_DOUBLE:
				{
					hScalar = annotation->AsScalar();
					if (!hScalar->IsValid())
						continue;

					info.type = Annotation::Type::Scalar;
					hScalar->GetFloat(&info.scalar);
				}
				break;
				case D3D_SVT_STRING:
				{
					hString = annotation->AsString();
					if (!hString->IsValid())
						continue;

					info.type = Annotation::Type::Text;
					LPCSTR buffer;
					hString->GetString(&buffer);
					info.text = buffer;
					std::transform(info.text.begin(), info.text.end(), info.text.begin(), ::tolower);
				}
				break;
			}
		}

		if (info.type != Annotation::Type::Unknown)
			m_annotations.push_back(info);

		ReleaseVars();
	}
}

ShaderVariableType ShaderVariableInfo::TypeOf(const D3DX11_EFFECT_TYPE_DESC& typeDesc)
{
	switch (typeDesc.Class)
	{
		case D3D_SVC_VECTOR:
			return ShaderVariableType::Vector;
		case D3D_SVC_MATRIX_ROWS:
		case D3D_SVC_MATRIX_COLUMNS:
			return ShaderVariableType::Matrix;
		case D3D_SVC_STRUCT:
			return ShaderVariableType::Struct;
	}

	switch (typeDesc.Type)
	{
		case D3D_SVT_VOID:
			return ShaderVariableType::Other;
		case D3D_SVT_BOOL:
			return ShaderVariableType::Bool;
		case D3D_SVT_INT:
			return ShaderVariableType::Int;
		case D3D_SVT_UINT:
			return ShaderVariableType::UInt;
		case D3D_SVT_FLOAT:
			return ShaderVariableType::Float;
		case D3D_SVT_DOUBLE:
			return ShaderVariableType::Double;
		case D3D_SVT_STRING:
			return ShaderVariableType::String;
		case D3D_SVT_TEXTURE1D:
			return ShaderVariableType::Texture1D;
		case D3D_SVT_TEXTURE2D:
			return ShaderVariableType::Texture2D;
		case D3D_SVT_TEXTURE3D:
			return ShaderVariableType::Texture3D;
		case D3D_SVT_TEXTURE1DARRAY:
			return ShaderVariableType::Texture1DArray;
		case D3D_SVT_TEXTURE2DARRAY:
			return ShaderVariableType::Texture2DArray;
		default:
			return ShaderVariableType::Other;
	}
}
