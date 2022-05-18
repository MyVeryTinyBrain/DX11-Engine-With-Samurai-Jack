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

	void Clear(Com<ID3D11DeviceContext> dc);
	void ClearPostProcessings(Com<ID3D11DeviceContext> dc);

public:

	bool ReadyToDraw(Com<ID3D11DeviceContext> dc);
	void SetDeferredRenderTargets(Com<ID3D11DeviceContext> dc);
	void SetForwardRenderTargets(Com<ID3D11DeviceContext> dc);
	void SetDeferredLightAccumulateRenderTargets(Com<ID3D11DeviceContext> dc);
	void SetDeferredLightBlendRenderTargets(Com<ID3D11DeviceContext> dc);
	void SetDeferredVolumetricLightBlendTargets(Com<ID3D11DeviceContext> dc);

public:

	void ClearCopyTargets();
	bool DepthWasCopied() const;
	bool ResultWasCopied() const;
	void CopyDepth(Com<ID3D11DeviceContext> dc);
	void CopyResult(Com<ID3D11DeviceContext> dc);

public:

	inline float GetSSRResolutionScale() const { return m_ssrResolutionScale; }
	void SetSSRResolutionScale(Com<ID3D11Device> dc, float value);

public:

	inline DepthStencil* GetDepthStencil() const { return m_depthStencil; }

	inline RenderTarget* GetAlbedo() const { return m_albedo; }
	inline RenderTarget* GetNormal() const { return m_normal; }
	inline RenderTarget* GetDepth() const { return m_depth[0]; }
	inline RenderTarget* GetLight_Shadow() const { return m_light_shadow; }
	inline RenderTarget* GetRoughness_Metallic() const { return m_roughness_metallic; }
	inline RenderTarget* GetEmissive() const { return m_emissive; }
	inline RenderTarget* GetOcclusion_Reflection_ReflectionBlur_ReflectMask() const { return m_occlusion_reflection_reflectionBlur_reflectMask; }

	inline RenderTarget* GetLight() const { return m_light; }
	inline RenderTarget* GetVolumetric() const { return m_volumetric; }
	inline RenderTarget* GetLightBlend() const { return m_lightBlend; }

	inline RenderTarget* GetResult() const { return m_result[0]; }

	inline RenderTarget* GetBridge() const { return m_bridge; }
	inline RenderTarget* GetBridgeHalf() const { return m_bridgeHalf; }
	inline RenderTarget* GetSSAO() const { return m_ssao; }
	inline RenderTarget* GetBloom() const { return m_bloom; }
	inline RenderTarget* GetSSR() const { return m_ssr; }
	inline RenderTarget* GetSSRBlur() const { return m_ssrBlur; }
	inline RenderTarget* GetDOF() const { return m_dof; }

	inline RenderTarget* GetCopyTargetDepth() const { return m_depth[1]; }
	inline RenderTarget* GetCopyTargetResult() const { return m_result[1]; }

	_declspec(property(get = GetDepthStencil)) DepthStencil* depthStencil;

	_declspec(property(get = GetAlbedo)) RenderTarget* albedo;
	_declspec(property(get = GetNormal)) RenderTarget* normal;
	_declspec(property(get = GetDepth)) RenderTarget* depth;
	_declspec(property(get = GetLight_Shadow)) RenderTarget* light_shadow;
	_declspec(property(get = GetRoughness_Metallic)) RenderTarget* roughness_metallic;
	_declspec(property(get = GetEmissive)) RenderTarget* emissive;
	_declspec(property(get = GetOcclusion_Reflection_ReflectionBlur_ReflectMask)) RenderTarget* occlusion_reflection_reflectionBlur_reflectMask;

	_declspec(property(get = GetLight)) RenderTarget* light;
	_declspec(property(get = GetSpecular)) RenderTarget* specular;
	_declspec(property(get = GetVolumetric)) RenderTarget* volumetric;
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

	DepthStencil*								m_depthStencil = nullptr;

	RenderTarget*								m_albedo = nullptr;
	RenderTarget*								m_normal = nullptr;
	RenderTarget*								m_depth[COPYABLE] = {}; // R: Projection Depth, G: View Depth
	RenderTarget*								m_light_shadow = nullptr;
	RenderTarget*								m_roughness_metallic = nullptr;
	RenderTarget*								m_emissive = nullptr;
	RenderTarget*								m_occlusion_reflection_reflectionBlur_reflectMask = nullptr;

	RenderTarget*								m_light = nullptr;
	RenderTarget*								m_volumetric = nullptr;
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
	float										m_ssrResolutionScale = 1.0f;
	RenderTarget*								m_dof = nullptr;
};

ENGINE_END
