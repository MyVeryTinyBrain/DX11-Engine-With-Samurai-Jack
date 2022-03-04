#pragma once

ENGINE_BEGIN
class GraphicSystem;
class RenderTarget;
class DepthStencil;
class ENGINE_API DeferredRenderTarget
{
private:

	DeferredRenderTarget(Com<ID3D11Device> device, uint width, uint height);

public:

	static DeferredRenderTarget* Create(Com<ID3D11Device> device, uint width, uint height);

	~DeferredRenderTarget();

public:

	bool Resize(Com<ID3D11Device> device, uint width, uint height);

	void Clear(Com<ID3D11DeviceContext> deviceContext);
	void ClearForwards(Com<ID3D11DeviceContext> deviceContext);
	void ClearPostProcessings(Com<ID3D11DeviceContext> deviceContext);

public:

	void SetDeferredRenderTargets(GraphicSystem* graphicSystem);
	void SetForwardRenderTargets(GraphicSystem* graphicSystem);
	void SetForwardLightRenderTargets(GraphicSystem* graphicSystem);
	void SetDeferredLightAccumulateRenderTargets(GraphicSystem* graphicSystem);
	void SetForwardLightAccumulateRenderTargets(GraphicSystem* graphicSystem);
	void SetDeferredLightBlendRenderTargets(GraphicSystem* graphicSystem);
	void SetForwardLightBlendRenderTargets(GraphicSystem* graphicSystem);

public:

	inline RenderTarget* GetDiffuse() const { return m_Diffuse; }
	inline RenderTarget* GetNormal() const { return m_Normal; }
	inline RenderTarget* GetDepth_Light_Occlusion_Shadow() const { return m_Depth_Light_Occlusion_Shadow; }
	inline RenderTarget* GetSpecular_Power() const { return m_Specular_Power; }
	inline RenderTarget* GetEmissive() const { return m_Emissive; }
	inline RenderTarget* GetReflection_ReflectMask() const { return m_Reflection_ReflectMask; }

	inline RenderTarget* GetForwardDiffuse() const { return m_forwardDiffuse; }
	inline RenderTarget* GetForwardNormal() const { return m_forwardNormal; }
	inline RenderTarget* GetForwardDepth_Light_Occlusion_Shadow() const { return m_forwardDepth_Light_Occlusion_Shadow; }
	inline RenderTarget* GetForwardSpecular_Power() const { return m_forwardSpecular_Power; }

	inline RenderTarget* GetLight() const { return m_light; }
	inline RenderTarget* GetSpecular() const { return m_specular; }
	inline RenderTarget* GetLightBlend() const { return m_lightBlend; }

	inline RenderTarget* GetForwardLight() const { return m_forwardLight; }
	inline RenderTarget* GetForwardSpecular() const { return m_forwardSpecular; }
	inline RenderTarget* GetForwardLightBlend() const { return m_forwardLightBlend; }

	inline RenderTarget* GetResult() const { return m_result; }

	inline RenderTarget* GetBridge() const { return m_bridge; }
	inline RenderTarget* GetBridgeHalf() const { return m_bridgeHalf; }
	inline RenderTarget* GetSSAO() const { return m_ssao; }
	inline RenderTarget* GetBloom() const { return m_bloom; }
	inline RenderTarget* GetSSR() const { return m_ssr; }
	inline RenderTarget* GetDOF() const { return m_dof; }

	_declspec(property(get = GetDiffuse)) RenderTarget* diffuse;
	_declspec(property(get = GetNormal)) RenderTarget* normal;
	_declspec(property(get = GetDepth_Light_Occlusion_Shadow)) RenderTarget* depth_Light_Occlusion_Shadow;
	_declspec(property(get = GetSpecular_Power)) RenderTarget* specular_Power;
	_declspec(property(get = GetEmissive)) RenderTarget* emissive;
	_declspec(property(get = GetReflection_ReflectMask)) RenderTarget* reflection_ReflectMask;

	_declspec(property(get = GetForwardDiffuse)) RenderTarget* forwardDiffuse;
	_declspec(property(get = GetForwardNormal)) RenderTarget* forwardNormal;
	_declspec(property(get = GetForwardDepth_Light_Occlusion_Shadow)) RenderTarget* forwardDepth_Light_Occlusion_Shadow;
	_declspec(property(get = GetForwardSpecular_Power)) RenderTarget* forwardSpecular_Power;

	_declspec(property(get = GetLight)) RenderTarget* light;
	_declspec(property(get = GetSpecular)) RenderTarget* specular;
	_declspec(property(get = GetLightBlend)) RenderTarget* lightBlend;

	_declspec(property(get = GetForwardLight)) RenderTarget* forwardLight;
	_declspec(property(get = GetForwardSpecular)) RenderTarget* forwardSpecular;
	_declspec(property(get = GetForwardLightBlend)) RenderTarget* forwardLightBlend;

	_declspec(property(get = GetResult)) RenderTarget* result;

	_declspec(property(get = GetBridge)) RenderTarget* bridge;
	_declspec(property(get = GetBridgeHalf)) RenderTarget* bridgeHalf;
	_declspec(property(get = GetSSAO)) RenderTarget* ssao;
	_declspec(property(get = GetBloom)) RenderTarget* bloom;
	_declspec(property(get = GetSSR)) RenderTarget* ssr;
	_declspec(property(get = GetDOF)) RenderTarget* dof;

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
	RenderTargets								m_forwardRenderTargets;
	RenderTargets								m_postProcessingRenderTargets;

	RenderTarget*								m_Diffuse = nullptr; // RGBA(Diffuse)
	RenderTarget*								m_Normal = nullptr; // RGB(Normal)
	RenderTarget*								m_Depth_Light_Occlusion_Shadow = nullptr; // R(Depth)G(LightMask)B(OcclusionMask)A(ShadowMask)
	RenderTarget*								m_Specular_Power = nullptr; // RGB(SpecularMask)A(Power)
	RenderTarget*								m_Emissive = nullptr; // RGBA(Emissive)
	RenderTarget*								m_Reflection_ReflectMask = nullptr; // R(Reflection)G(ReflectMask)

	RenderTarget*								m_forwardDiffuse = nullptr;
	RenderTarget*								m_forwardNormal = nullptr; 
	RenderTarget*								m_forwardDepth_Light_Occlusion_Shadow = nullptr; 
	RenderTarget*								m_forwardSpecular_Power = nullptr; 

	RenderTarget*								m_light = nullptr;
	RenderTarget*								m_specular = nullptr;
	RenderTarget*								m_lightBlend = nullptr;

	RenderTarget*								m_forwardLight = nullptr;
	RenderTarget*								m_forwardSpecular = nullptr;
	RenderTarget*								m_forwardLightBlend = nullptr;

	RenderTarget*								m_result = nullptr;

	RenderTarget*								m_bridge = nullptr;
	RenderTarget*								m_bridgeHalf = nullptr;
	RenderTarget*								m_ssao = nullptr;
	RenderTarget*								m_bloom = nullptr;
	RenderTarget*								m_ssr = nullptr;
	RenderTarget*								m_dof = nullptr;
};

ENGINE_END
