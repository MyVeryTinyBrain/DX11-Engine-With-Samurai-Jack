#include "EnginePCH.h"
#include "TechniqueDesc.h"
#include "PassDesc.h"

TechniqueDesc::TechniqueDesc(ID3DX11EffectTechnique* technique, const vector<PassDesc*>& passes)
{
	m_technique = technique;
	m_passDescs = passes;
}

TechniqueDesc::~TechniqueDesc()
{
	for (auto& passDesc : m_passDescs)
		SafeDelete(passDesc);
	m_passDescs.clear();

	SafeRelease(m_technique);
}

Com<ID3DX11EffectTechnique> TechniqueDesc::GetTechnique() const
{
	return m_technique;
}

uint TechniqueDesc::GetPassDescCount() const
{
	return uint(m_passDescs.size());
}

PassDesc* TechniqueDesc::GetPassDesc(uint index) const
{
	if (index >= uint(m_passDescs.size()))
		return nullptr;

	return m_passDescs[index];
}

HRESULT TechniqueDesc::GetPassIndexByName(const string& name, uint& out_index) const
{
	HRESULT hr = S_OK;

	for (uint i = 0; i < uint(m_passDescs.size()); ++i)
	{
		Com<ID3DX11EffectPass> pass = m_passDescs[i]->GetPass();

		D3DX11_PASS_DESC desc = {};
		if (FAILED(hr = pass->GetDesc(&desc)))
			return hr;

		if (name == desc.Name)
		{
			out_index = i;
			return S_OK;
		}
	}

	return E_FAIL;
}

TechniqueDesc* TechniqueDesc::CreateTechqniueDesc(Com<ID3D11Device> device, ID3DX11EffectTechnique* technique, tstring& out_error)
{
	if (!device)
		return nullptr;

	if (!technique)
		return nullptr;

	D3DX11_TECHNIQUE_DESC techniqueDesc = {};
	if (FAILED(technique->GetDesc(&techniqueDesc)))
		return nullptr;

	vector<PassDesc*> passes;
	passes.reserve(techniqueDesc.Passes);

	auto ReleaseVars = [&]()
	{
		for (auto& pass : passes)
			SafeDelete(pass);
		passes.clear();
	};

	for (uint32_t i = 0; i < techniqueDesc.Passes; ++i)
	{
		ID3DX11EffectPass* pass = technique->GetPassByIndex(i);
		if (!pass)
		{
			ReleaseVars();
			return nullptr;
		}

		PassDesc* passDesc = PassDesc::CreatePassDesc(device, pass, out_error);
		passes.push_back(passDesc);
		if (!passDesc)
		{
			ReleaseVars();
			return nullptr;
		}
	}

	return new TechniqueDesc(technique, passes);
}