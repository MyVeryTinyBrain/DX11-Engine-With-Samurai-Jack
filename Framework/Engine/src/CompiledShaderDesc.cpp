#include "EnginePCH.h"
#include "CompiledShaderDesc.h"
#include "TechniqueDesc.h"
#include "PassDesc.h"

CompiledShaderDesc::CompiledShaderDesc(ID3DX11Effect* effect, const vector<TechniqueDesc*>& techniqueDescs)
{
	m_effect = effect;
	m_techniqueDescs = techniqueDescs;
}

CompiledShaderDesc::~CompiledShaderDesc()
{
	for (auto& technique : m_techniqueDescs)
		SafeDelete(technique);
	m_techniqueDescs.clear();

	SafeRelease(m_effect);
}

Com<ID3DX11Effect> CompiledShaderDesc::GetEffect() const
{
	return m_effect;
}

size_t CompiledShaderDesc::GetTechniqueDescCount() const
{
	return m_techniqueDescs.size();
}

TechniqueDesc* CompiledShaderDesc::GetTechniqueDesc(size_t index) const
{
	if (index >= m_techniqueDescs.size())
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

	hr = hValue->SetRawValue(data, 0, uint32_t(size));

	if (FAILED(hr))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
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

HRESULT CompiledShaderDesc::SetTextureArray(const string& name, ID3D11ShaderResourceView** textureArray, uint count)
{
	HRESULT hr = S_OK;

	if (nullptr == m_effect)
		return E_FAIL;

	if (!textureArray)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* hValue = m_effect->GetVariableByName(name.c_str())->AsShaderResource();
	if (!hValue->IsValid())
	{
		SafeRelease(hValue);
		return E_FAIL;
	}

	if (FAILED(hr = hValue->SetResourceArray(textureArray, 0, count)))
	{
		SafeRelease(hValue);
		return hr;
	}

	SafeRelease(hValue);
	return S_OK;
}

CompiledShaderDesc* CompiledShaderDesc::CreateCompiledShaderFromFile(Com<ID3D11Device> device, const tstring& path, tstring& out_error)
{
	if (!device)
		return nullptr;

	uint32_t compileFlag1 = 0;
	uint32_t compileFlag2 = 0;

#ifdef _DEBUG
	compileFlag1 = 
		D3DCOMPILE_DEBUG | 
		D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compileFlag1 = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* compileShader = nullptr;
	ID3DBlob* compileShaderErrorMessage = nullptr;
	ID3DX11Effect* effect = nullptr;
	vector<TechniqueDesc*> techniqueDescs;

	auto ReleaseVars = [&]()
	{
		for (auto& techniqueDesc : techniqueDescs)
			SafeDelete(techniqueDesc);
		techniqueDescs.clear();

		SafeRelease(effect);
		SafeRelease(compileShaderErrorMessage);
		SafeRelease(compileShader);
	};

	HRESULT hr = S_OK;

	if (FAILED(hr = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		nullptr,
		"fx_5_0",
		compileFlag1, compileFlag2,
		&compileShader, &compileShaderErrorMessage)))
	{
		ReleaseVars();
		return nullptr;
	}

	if (FAILED(hr = D3DX11CreateEffectFromMemory(
		compileShader->GetBufferPointer(),
		compileShader->GetBufferSize(),
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

	bool hasDefaultTechnique = false;
	bool hasComputeTechnique = false;
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

		if (techniqueDesc->IsComputeTechnique())
			hasComputeTechnique = true;
		else
			hasDefaultTechnique = true;
		if (hasDefaultTechnique && hasComputeTechnique)
		{
			out_error = TEXT("A shader cannot have both a Compute Technique and a Default Technique at the same time.");
			ReleaseVars();
			return nullptr;
		}

		techniqueDescs.push_back(techniqueDesc);
	}

	SafeRelease(compileShaderErrorMessage);
	SafeRelease(compileShader);

	return new CompiledShaderDesc(effect, techniqueDescs);
}

HRESULT CompiledShaderDesc::SetInputLayout(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex)
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

HRESULT CompiledShaderDesc::ApplyPass(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex)
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
