#include "EnginePCH.h"
#include "CompiledShaderDesc.h"
#include "TechniqueDesc.h"
#include "PassDesc.h"

CompiledShaderDesc::CompiledShaderDesc(ID3DX11Effect* effect, const vector<TechniqueDesc*>& techniqueDescs, const vector<ShaderVariableInfo>& infos)
{
	m_effect = effect;
	m_techniqueDescs = techniqueDescs;
	m_variableInfos = infos;
}

CompiledShaderDesc::~CompiledShaderDesc()
{
	for (auto& technique : m_techniqueDescs)
		SafeDelete(technique);
	m_techniqueDescs.clear();
	m_variableInfos.clear();
	SafeRelease(m_effect);
}

Com<ID3DX11Effect> CompiledShaderDesc::GetEffect() const
{
	return m_effect;
}

uint CompiledShaderDesc::GetTechniqueDescCount() const
{
	return uint(m_techniqueDescs.size());
}

TechniqueDesc* CompiledShaderDesc::GetTechniqueDesc(uint index) const
{
	if (index >= uint(m_techniqueDescs.size()))
		return nullptr;
	
	return m_techniqueDescs[index];
}

HRESULT CompiledShaderDesc::GetTechniqueIndexByName(const string& name, uint& out_index) const
{
	HRESULT hr = S_OK;

	for (uint i = 0; i < uint(m_techniqueDescs.size()); ++i)
	{
		Com<ID3DX11EffectTechnique> technique = m_techniqueDescs[i]->GetTechnique();

		D3DX11_TECHNIQUE_DESC desc = {};
		if (FAILED(hr = technique->GetDesc(&desc)))
			return hr;

		if (name == desc.Name)
		{
			out_index = i;
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CompiledShaderDesc::GetPassIndexByName(const string& name, uint techniqueIndex, uint& out_index) const
{
	if (techniqueIndex >= m_techniqueDescs.size())
		return E_FAIL;

	TechniqueDesc* techniqueDesc = m_techniqueDescs[techniqueIndex];
	return techniqueDesc->GetPassIndexByName(name, out_index);
}

HRESULT CompiledShaderDesc::SetRawValue(const string& name, const void* data, size_t size)
{
	HRESULT hr = S_OK;

	if (!m_effect)
		return E_FAIL;

	if (!data)
		return E_FAIL;

	ID3DX11EffectVariable* hValue = m_effect->GetVariableByName(name.c_str());
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetRawValue(data, 0, uint32_t(size))))
		return hr;

	return S_OK;
}

HRESULT CompiledShaderDesc::SetFloat(const string& name, float value)
{
	HRESULT hr = S_OK;

	if (!m_effect)
		return E_FAIL;

	ID3DX11EffectScalarVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsScalar();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetFloat(value)))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

HRESULT CompiledShaderDesc::SetBool(const string& name, bool value)
{
	HRESULT hr = S_OK;

	if (!m_effect)
		return E_FAIL;
	
	ID3DX11EffectScalarVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsScalar();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetBool(value)))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

HRESULT CompiledShaderDesc::SetVector(const string& name, const V4& value)
{
	HRESULT hr = S_OK;

	if (!m_effect)
		return E_FAIL;

	ID3DX11EffectVectorVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsVector();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetFloatVector(&value.x)))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

HRESULT CompiledShaderDesc::SetColor(const string& name, const Color& value)
{
	HRESULT hr = S_OK;

	if (!m_effect)
		return E_FAIL;

	ID3DX11EffectVectorVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsVector();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetFloatVector(&value.value.x)))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

HRESULT CompiledShaderDesc::SetMatrix(const string& name, const M4& value)
{
	HRESULT hr = S_OK;

	if (!m_effect)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsMatrix();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	// Doesn't Transpose Matrix
	if (FAILED(hr = hValue->SetMatrix(&value.m00)))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

HRESULT CompiledShaderDesc::SetTexture(const string& name, Com<ID3D11ShaderResourceView> texture)
{
	HRESULT hr = S_OK;

	if (nullptr == m_effect)
		return E_FAIL;

	if (!texture)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsShaderResource();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetResource(texture.Get())))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

HRESULT CompiledShaderDesc::SetTextures(const string& name, ID3D11ShaderResourceView** textures, uint count)
{
	HRESULT hr = S_OK;

	if (nullptr == m_effect)
		return E_FAIL;

	if (!textures)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsShaderResource();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetResourceArray(textures, 0, count)))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

CompiledShaderDesc* CompiledShaderDesc::CreateCompiledShaderFromFile(Com<ID3D11Device> device, const tstring& path, tstring& out_error)
{
	ID3DBlob* compileShader = nullptr;
	ID3DBlob* compileShaderErrorMessage = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeRelease(compileShaderErrorMessage);
		SafeRelease(compileShader);
	};

	HRESULT hr = S_OK;

	if(FAILED(hr = CompileShader(path, &compileShader, &compileShaderErrorMessage)))
	{
		ReleaseVars();
		return nullptr;
	}

	CompiledShaderDesc* shader = CreateCompiledShader(device, compileShader, out_error);

	ReleaseVars();
	return shader;
}

HRESULT CompiledShaderDesc::CompileShader(const tstring& path, ID3DBlob** out_memory, ID3DBlob** out_errorMsg)
{
	uint32_t compileFlag1 = 0;
	uint32_t compileFlag2 = 0;

#ifdef _DEBUG
	compileFlag1 =
		D3DCOMPILE_DEBUG |
		D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compileFlag1 = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr = S_OK;

	if (FAILED(hr = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		nullptr,
		"fx_5_0",
		compileFlag1, compileFlag2,
		out_memory, out_errorMsg)))
	{
		SafeDelete(out_memory);
		SafeDelete(out_errorMsg);
		return hr;
	}

	return S_OK;
}

CompiledShaderDesc* CompiledShaderDesc::CreateCompiledShader(Com<ID3D11Device> device, ID3DBlob* memory, tstring& out_error)
{
	if (!device || !memory)
		return nullptr;

	ID3DX11Effect* effect = nullptr;
	vector<TechniqueDesc*> techniqueDescs;
	vector<ShaderVariableInfo> infos;

	auto ReleaseVars = [&]()
	{
		for (auto& techniqueDesc : techniqueDescs)
			SafeDelete(techniqueDesc);
		techniqueDescs.clear();

		SafeRelease(effect);
	};

	HRESULT hr = S_OK;

	if (FAILED(hr = D3DX11CreateEffectFromMemory(
		memory->GetBufferPointer(),
		memory->GetBufferSize(),
		0,
		device.Get(),
		&effect)))
	{
		ReleaseVars();
		return nullptr;
	}

	D3DX11_EFFECT_DESC effectDesc = {};
	if (FAILED(effect->GetDesc(&effectDesc)))
	{
		ReleaseVars();
		return nullptr;
	}

	techniqueDescs.reserve(effectDesc.Techniques);

	for (uint32_t i = 0; i < effectDesc.Techniques; ++i)
	{
		ID3DX11EffectTechnique* technique = effect->GetTechniqueByIndex(i);
		if (!technique)
		{
			ReleaseVars();
			return nullptr;
		}

		TechniqueDesc* techniqueDesc = TechniqueDesc::CreateTechqniueDesc(device, technique, out_error);
		if (!techniqueDesc)
		{
			ReleaseVars();
			return nullptr;
		}

		techniqueDescs.push_back(techniqueDesc);
	}

	infos.reserve(effectDesc.GlobalVariables);

	for (uint32_t i = 0; i < effectDesc.GlobalVariables; ++i)
	{
		ID3DX11EffectVariable* handle = effect->GetVariableByIndex(i);
		ID3DX11EffectType* type = handle->GetType();
		D3DX11_EFFECT_VARIABLE_DESC variableDesc;
		D3DX11_EFFECT_TYPE_DESC typeDesc;
		if (FAILED(hr = handle->GetDesc(&variableDesc)))
		{
			ReleaseVars();
			return nullptr;
		}
		if (FAILED(hr = type->GetDesc(&typeDesc)))
		{
			ReleaseVars();
			return nullptr;
		}
		if (variableDesc.Semantic && !strcmp(variableDesc.Semantic, "ENGINE"))
		{
			SafeRelease(handle);
			SafeRelease(type);
			continue;
		}
		
		ShaderVariableInfo info;
		info.Name = variableDesc.Name;
		info.Semantic = variableDesc.Semantic != nullptr ? variableDesc.Semantic : "";
		info.Type = TypeOf(typeDesc);
		info.Size = typeDesc.Stride;
		info.Elements = typeDesc.Elements;
		info.Handle = handle;

		SafeRelease(type);

		infos.push_back(info);
	}

	return new CompiledShaderDesc(effect, techniqueDescs, infos);
}

ShaderVariableType CompiledShaderDesc::TypeOf(const D3DX11_EFFECT_TYPE_DESC& typeDesc)
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

HRESULT CompiledShaderDesc::SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex)
{
	HRESULT hr = S_OK;

	const TechniqueDesc* techniqueDesc = GetTechniqueDesc(techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	const PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	Com<ID3D11InputLayout> inputLayout = passDesc->GetInputLayout();
	if (!inputLayout)
		return E_FAIL;

	Com<ID3DX11EffectPass> pass = passDesc->GetPass();
	if (!pass)
		return E_FAIL;

	deviceContext->IASetInputLayout(inputLayout.Get());

	return S_OK;
}

HRESULT CompiledShaderDesc::ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex)
{
	HRESULT hr = S_OK;

	const TechniqueDesc* techniqueDesc = GetTechniqueDesc(techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	const PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	Com<ID3D11InputLayout> inputLayout = passDesc->GetInputLayout();
	if (!inputLayout)
		return E_FAIL;

	Com<ID3DX11EffectPass> pass = passDesc->GetPass();
	if (!pass)
		return E_FAIL;
	
	if (FAILED(hr = pass->Apply(0, deviceContext.Get())))
		return hr;

	return S_OK;
}

const ShaderVariableInfo* CompiledShaderDesc::FindVariableByName(const string& name)
{
	auto find_it = std::find_if(m_variableInfos.begin(), m_variableInfos.end(),
		[&](const ShaderVariableInfo& e)
		{
			return e.Name == name;
		});

	if (find_it == m_variableInfos.end())
		return nullptr;
	else
		return find_it._Ptr;
}

const ShaderVariableInfo* CompiledShaderDesc::FindVariableByIndex(uint index)
{
	if (index >= GetVariableCount())
		return nullptr;

	return &m_variableInfos[index];
}
