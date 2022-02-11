#pragma once

#include "RenderGroup.h"

ENGINE_BEGIN

class ENGINE_API PassDesc
{
protected:

	PassDesc(
		ID3DX11EffectPass* pass, ID3D11InputLayout* inputLayout, 
		const string& name, const vector<string>& sementics, 
		RenderGroup renderGroup, int renderGroupOrder,
		bool instancingFlag, 
		bool drawShadowFlag, bool shadowCutoffEnable, float shadowCutoffAlpha);

public:

	~PassDesc();

public:

	Com<ID3DX11EffectPass> GetPass() const;

	Com<ID3D11InputLayout> GetInputLayout() const;

	RenderGroup GetRenderGroup() const;

	int GetRenderGroupOrder() const;

	bool IsInstancing() const;

	bool IsDrawShadow() const;

	bool IsShadowCutoffEnable() const;

	float GetShadowCutoffAlphaValue() const;

private:

	static bool CreateInputElements(ID3DX11EffectPass* pass, D3D11_INPUT_ELEMENT_DESC** out_arrElements, size_t* out_count);

	static ID3D11InputLayout* CreateInputLayout(Com<ID3D11Device> device, ID3DX11EffectPass* pass, string& out_name, vector<string>& out_sementics);

	static bool GetRenderGroup(ID3DX11EffectPass* pass, RenderGroup* out_renderGroup);

	static bool GetRenderGroupOrder(ID3DX11EffectPass* pass, int* out_renderGroupOrder);

	static bool GetInstancingFlag(ID3DX11EffectPass* pass, bool* out_flag);

	static bool GetDrawShadowFlag(ID3DX11EffectPass* pass, bool* out_flag);

	static bool GetShadowCutoffEnableFlag(ID3DX11EffectPass* pass, bool* out_flag);

	static bool GetShadowCutoffAlpha(ID3DX11EffectPass* pass, float* out_shadowCutoffAlpha);

public:

	static PassDesc* CreatePassDesc(Com<ID3D11Device> device, ID3DX11EffectPass* pass);

private:

	ID3DX11EffectPass*		m_pass = nullptr;

	ID3D11InputLayout*		m_inputLayout = nullptr;

	string					m_name = "";

	vector<string>			m_sementics;

	RenderGroup				m_renderGroup = RenderGroup::Standard; // string RenderGroup {"Priority, Standard, Transparent, Overlay"}

	int						m_renderGroupOrder = 0; // int RenderGroupOrder 

	bool					m_isInstancing = false; // bool instancing

	bool					m_isDrawingShadow = true; // bool DrawShadow
	bool					m_isShadowCutoffEnable = false; // bool ShadowCutoffEnable
	float					m_shadowCutoffAlpha = 0.0f; // float ShadowCutoffAlpha
};

ENGINE_END
