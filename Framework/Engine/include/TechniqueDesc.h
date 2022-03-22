#pragma once

ENGINE_BEGIN
class PassDesc;
class ENGINE_API TechniqueDesc
{
protected:

	TechniqueDesc(ID3DX11EffectTechnique* technique, const vector<PassDesc*>& passes);

public:

	~TechniqueDesc();

public:

	Com<ID3DX11EffectTechnique> GetTechnique() const;

	uint GetPassDescCount() const;

	PassDesc* GetPassDesc(uint index) const;

	HRESULT GetPassIndexByName(const string& name, uint& out_index) const;

public:

	static TechniqueDesc* CreateTechqniueDesc(Com<ID3D11Device> device, ID3DX11EffectTechnique* technique, tstring& out_error);

private:

	ID3DX11EffectTechnique*		m_technique = nullptr;
	vector<PassDesc*>			m_passDescs;
};

ENGINE_END

