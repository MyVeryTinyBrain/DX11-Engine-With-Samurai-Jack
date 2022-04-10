#pragma once

ENGINE_BEGIN
class GraphicSystem;
class RenderTarget;
class DepthStencil;
class ENGINE_API DeferredRenderTarget
{
	enum { COPYABLE = 2 };

private:

	DeferredRenderTarget(Com<ID3D11Device> device, uint width, uint height);

public:

	static DeferredRenderTarget* Create(Com<ID3D11Device> device, uint width, uint height);

	~DeferredRenderTarget();

public:

	bool Resize(Com<ID3D11Device> device, uint width, uint height);

	void Clear(Com<ID3D11DeviceContext> deviceContext);
	void ClearPostProcessings(Com<ID3D11DeviceContext> deviceContext);

public:

	bool ReadyToDraw(Com<ID3D11DeviceContext> deviceContext);
	void SetDeferredRenderTargets(GraphicSystem* graphicSystem);
	void SetForwardRenderTargets(GraphicSystem* graphicSystem);
	void SetDeferredLightAccumulateRenderTargets(GraphicSystem* graphicSystem);
	void SetDeferredLightBlendRenderTargets(GraphicSystem* graphicSystem);

public:

	void ClearCopyTargets();
	bool DepthWasCopied() const;
	bool ResultWasCopied() const;
	void CopyDepth(Com<ID3D11DeviceContext> deviceContext);
	void CopyResult(Com<ID3D11DeviceContext> deviceContext);

public:

	inline RenderTarget* GetDiffuse() const { return m_Diffuse; }
	inline RenderTarget* GetNormal() const { return m_Normal; }
	inline RenderTarget* GetDepth() const { return m_Depth[0]; }
	inline RenderTarget* GetLight_Occlusion_Shadow() const { return m_Light_Occlusion_Shadow; }
	inline RenderTarget* GetSpecular_Power() const { return m_Specular_Power; }
	inline RenderTarget* GetEmissive() const { return m_Emissive; }
	inline RenderTarget* GetReflection_ReflectionBlur_ReflectMask() const { return m_Reflection_ReflectionBlur_ReflectMask; }

	inline RenderTarget* GetLight() const { return m_light; }
	inline RenderTarget* GetSpecular() const { return m_specular; }
	inline RenderTarget* GetLightBlend() const { return m_lightBlend; }

	inline RenderTarget* GetResult() const { return m_result[0]; }

	inline RenderTarget* GetBridge() const { return m_bridge; }
	inline RenderTarget* GetBridgeHalf() const { return m_bridgeHalf; }
	inline RenderTarget* GetSSAO() const { return m_ssao; }
	inline RenderTarget* GetBloom() const { return m_bloom; }
	inline RenderTarget* GetSSR() const { return m_ssr; }
	inline RenderTarget* GetSSRBlur() const { return m_ssrBlur; }
	inline RenderTarget* GetDOF() const { return m_dof; }

	inline RenderTarget* GetCopyTargetDepth() const { return m_Depth[1]; }
	inline RenderTarget* GetCopyTargetResult() const { return m_result[1]; }

	_declspec(property(get = GetDiffuse)) RenderTarget* diffuse;
	_declspec(property(get = GetNormal)) RenderTarget* normal;
	_declspec(property(get = GetDepth)) RenderTarget* depth;
	_declspec(property(get = GetLight_Occlusion_Shadow)) RenderTarget* light_Occlusion_Shadow;
	_declspec(property(get = GetSpecular_Power)) RenderTarget* specular_Power;
	_declspec(property(get = GetEmissive)) RenderTarget* emissive;
	_declspec(property(get = GetReflection_ReflectionBlur_ReflectMask)) RenderTarget* reflection_ReflectionBlur_ReflectMask;

	_declspec(property(get = GetLight)) RenderTarget* light;
	_declspec(property(get = GetSpecular)) RenderTarget* specular;
	_declspec(property(get = GetLightBlend)) RenderTarget* lightBlend;

	_declspec(property(get = GetResult)) RenderTarget* result;

	_declspec(property(get = GetBridge)) RenderTarget* bridge;
	_declspec(property(get = GetBridgeHalf)) RenderTarget* bridgeHalf;
	_declspec(property(get = GetSSAO)) RenderTarget* ssao;
	_declspec(property(get = GetBloom)) RenderTarget* bloom;
	_declspec(property(get = GetSSR)) RenderTarget* ssr;
	_declspec(property(get = GetSSRBlur)) RenderTarget* ssrBlur;
	_declspec(property(get = GetDOF)) RenderTarget* dof;

	_declspec(property(get = GetCopyTargetDepth)) RenderTarget* copyTargetDepth;
	_declspec(property(get = GetCopyTargetResult)) RenderTarget* copyTargetResult;

public:

	inline uint GetWidth() const { return m_width; }
	inline uint GetHeight() const { return m_height; }

	_declspec(property(get = GetWidth)) uint width;
	_declspec(property(get = GetHeight)) uint height;

private:

	uint										m_width;
	uint										m_height;

	using RenderTargets = vector<RenderTarget*>;
	RenderTargets								m_renderTargets;
	RenderTargets								m_postProcessingRenderTargets;
	RenderTargets								m_copyTargets;

	RenderTarget*								m_Diffuse = nullptr; // RGBA(Diffuse)
	RenderTarget*								m_Normal = nullptr; // RGB(Normal)
	RenderTarget*								m_Depth[COPYABLE] = {}; // R=G=B(Depth)
	RenderTarget*								m_Light_Occlusion_Shadow = nullptr; // R(LightMask)G(OcclusionMask)B(ShadowMask)
	RenderTarget*								m_Specular_Power = nullptr; // RGB(SpecularMask)A(Power)
	RenderTarget*								m_Emissive = nullptr; // RGBA(Emissive)
	RenderTarget*								m_Reflection_ReflectionBlur_ReflectMask = nullptr; // R(Reflection)G(ReflectionBlur)B(ReflectMask)

	RenderTarget*								m_light = nullptr;
	RenderTarget*								m_specular = nullptr;
	RenderTarget*								m_lightBlend = nullptr;

	RenderTarget*								m_result[COPYABLE] = {};

	bool										m_depthWasCopied = false;
	bool										m_resultWasCopied = false;

	RenderTarget*								m_bridge = nullptr;
	RenderTarget*								m_bridgeHalf = nullptr;
	RenderTarget*								m_ssao = nullptr;
	RenderTarget*								m_bloom = nullptr;
	RenderTarget*								m_ssr = nullptr;
	RenderTarget*								m_ssrBlur = nullptr;
	RenderTarget*								m_dof = nullptr;
};

ENGINE_END
