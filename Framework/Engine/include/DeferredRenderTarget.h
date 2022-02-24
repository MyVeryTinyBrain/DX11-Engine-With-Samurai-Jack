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

	inline RenderTarget* GetDiffuse() const { return m_diffuse; }
	inline RenderTarget* GetNormal() const { return m_normal; }
	inline RenderTarget* GetWorldPosition() const { return m_worldPosition; }
	inline RenderTarget* GetDepthLightOcclusionShadow() const { return m_depthLightOcclusionShadow; }
	inline RenderTarget* GetSpecularPower() const { return m_specularPower; }
	inline RenderTarget* GetEmissive() const { return m_emissive; }

	inline RenderTarget* GetForwardDiffuse() const { return m_forwardDiffuse; }
	inline RenderTarget* GetForwardNormal() const { return m_forwardNormal; }
	inline RenderTarget* GetForwardWorldPosition() const { return m_forwardWorldPosition; }
	inline RenderTarget* GetForwardDepthLightOcclusionShadow() const { return m_forwardDepthLightOcclusionShadow; }
	inline RenderTarget* GetForwardSpecularPower() const { return m_forwardSpecularPower; }

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
	inline RenderTarget* GetDOF() const { return m_dof; }
	inline RenderTarget* GetBloom() const { return m_bloom; }

	_declspec(property(get = GetDiffuse)) RenderTarget* diffuse;
	_declspec(property(get = GetNormal)) RenderTarget* normal;
	_declspec(property(get = GetWorldPosition)) RenderTarget* worldPosition;
	_declspec(property(get = GetDepthLightOcclusionShadow)) RenderTarget* depthLightOcclusionShadow;
	_declspec(property(get = GetSpecularPower)) RenderTarget* specularPower;
	_declspec(property(get = GetEmissive)) RenderTarget* emissive;

	_declspec(property(get = GetForwardDiffuse)) RenderTarget* forwardDiffuse;
	_declspec(property(get = GetForwardNormal)) RenderTarget* forwardNormal;
	_declspec(property(get = GetForwardWorldPosition)) RenderTarget* forwardWorldPosition;
	_declspec(property(get = GetForwardDepthLightOcclusionShadow)) RenderTarget* forwardDepthLightOcclusionShadow;
	_declspec(property(get = GetForwardSpecularPower)) RenderTarget* forwardSpecularPower;

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
	_declspec(property(get = GetDOF)) RenderTarget* dof;
	_declspec(property(get = GetBloom)) RenderTarget* bloom;

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

	RenderTarget*								m_diffuse = nullptr; // RGBA(Diffuse)
	RenderTarget*								m_normal = nullptr; // RGB(Normal)
	RenderTarget*								m_worldPosition = nullptr; // RGB(WorldPositionDirection)A(WorldPositionDistance)
	RenderTarget*								m_depthLightOcclusionShadow = nullptr; // R(Depth)G(LightMask)B(OcclusionMask)A(ShadowMask)
	RenderTarget*								m_specularPower = nullptr; // RGB(SpecularMask)A(Power)
	RenderTarget*								m_emissive = nullptr; // RGBA(Emissive)

	RenderTarget*								m_forwardDiffuse = nullptr;
	RenderTarget*								m_forwardNormal = nullptr; 
	RenderTarget*								m_forwardWorldPosition = nullptr; 
	RenderTarget*								m_forwardDepthLightOcclusionShadow = nullptr; 
	RenderTarget*								m_forwardSpecularPower = nullptr; 

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
	RenderTarget*								m_dof = nullptr;
	RenderTarget*								m_bloom = nullptr;
};

ENGINE_END
