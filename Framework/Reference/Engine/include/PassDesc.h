#pragma once

#include "ShaderTypes.h"

ENGINE_BEGIN

class ENGINE_API PassDesc
{
protected:

	PassDesc(
		ID3DX11EffectPass* pass, ID3D11InputLayout* inputLayout, 
		const string& name, const vector<string>& sementics, 
		RenderGroup renderGroup, int renderGroupOrder,
		bool cullingFlag, bool instancingFlag, 
		bool drawShadowFlag, bool shadowPassFlag);

public:

	~PassDesc();

public:

	inline Com<ID3DX11EffectPass> GetPass() const { return m_pass; }
	inline Com<ID3D11InputLayout> GetInputLayout() const { return m_inputLayout; }

	inline RenderGroup GetRenderGroup() const { return m_renderGroup; }
	inline int GetRenderGroupOrder() const { return m_renderGroupOrder; }
	inline bool IsCulling() const { return m_isCulling; }
	inline bool IsInstancing() const { return m_isInstancing; }
	inline bool IsDrawShadow() const { return m_isDrawingShadow; }
	inline bool IsShadowPass() const { return m_isShadowPass; }

private:

	static bool CreateInputElements(ID3DX11EffectPass* pass, D3D11_INPUT_ELEMENT_DESC** out_arrElements, uint* out_count);
	static ID3D11InputLayout* CreateInputLayout(Com<ID3D11Device> device, ID3DX11EffectPass* pass, string& out_name, vector<string>& out_sementics);

	static bool ExtractRenderGroup(ID3DX11EffectPass* pass, RenderGroup* out_renderGroup);
	static bool ExtractRenderGroupOrder(ID3DX11EffectPass* pass, int* out_renderGroupOrder);
	static bool ExtractCullingFlag(ID3DX11EffectPass* pass, bool* out_flag);
	static bool ExtractInstancingFlag(ID3DX11EffectPass* pass, bool* out_flag);
	static bool ExtractDrawShadowFlag(ID3DX11EffectPass* pass, bool* out_flag);
	static bool ExtractShadowPassFlag(ID3DX11EffectPass* pass, bool* out_flag);

public:

	static PassDesc* CreatePassDesc(Com<ID3D11Device> device, ID3DX11EffectPass* pass, tstring& out_error);

private:

	static PassDesc* CreateDefaultPassDesc(Com<ID3D11Device> device, ID3DX11EffectPass* pass);

private:

	ID3DX11EffectPass*	m_pass = nullptr;
	ID3D11InputLayout*	m_inputLayout = nullptr;

	string				m_name = "";

	vector<string>		m_sementics;

	RenderGroup			m_renderGroup = RenderGroup::Standard; // string RenderGroup {"Priority, Standard, Transparent, Overlay"}

	int					m_renderGroupOrder = 0; // int RenderGroupOrder 

	bool				m_isCulling = true; // bool Cull
	bool				m_isInstancing = false; // bool Instancing

	bool				m_isDrawingShadow = true; // bool DrawShadow
	bool				m_isShadowPass = false; // bool ShadowPass

};

ENGINE_END
