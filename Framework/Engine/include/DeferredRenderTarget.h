#pragma once

ENGINE_BEGIN
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

	void Clear(Com<ID3D11DeviceContext> deviceContext, const Color& clearColor);

public:

	inline RenderTarget* GetDiffuse() const { return m_diffuse; }
	inline RenderTarget* GetNormal() const { return m_normal; }
	inline RenderTarget* GetWorldPosition() const { return m_worldPosition; }
	inline RenderTarget* GetDepthLightOcclusionShadow() const { return m_depthLightOcclusionShadow; }
	inline RenderTarget* GetSpecularPower() const { return m_specularPower; }

	inline RenderTarget* GetLight() const { return m_light; }
	inline RenderTarget* GetSpecular() const { return m_specular; }
	inline RenderTarget* GetLightBlend() const { return m_lightBlend; }

	inline RenderTarget* GetDiv2a() const { return m_div2a; }
	inline RenderTarget* GetDiv2b() const { return m_div2b; }

	_declspec(property(get = GetDiffuse)) RenderTarget* diffuse;
	_declspec(property(get = GetNormal)) RenderTarget* normal;
	_declspec(property(get = GetWorldPosition)) RenderTarget* worldPosition;
	_declspec(property(get = GetDepthLightOcclusionShadow)) RenderTarget* depthLightOcclusionShadow;
	_declspec(property(get = GetSpecularPower)) RenderTarget* specularPower;

	_declspec(property(get = GetLight)) RenderTarget* light;
	_declspec(property(get = GetSpecular)) RenderTarget* specular;
	_declspec(property(get = GetLightBlend)) RenderTarget* lightBlend;

	_declspec(property(get = GetDiv2a)) RenderTarget* div2a;
	_declspec(property(get = GetDiv2b)) RenderTarget* div2b;

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

	RenderTarget*								m_diffuse = nullptr; // RGBA(Diffuse)
	RenderTarget*								m_normal = nullptr; // RGB(Normal)
	RenderTarget*								m_worldPosition = nullptr; // RGB(WorldPositionDirection)A(WorldPositionDistance)
	RenderTarget*								m_depthLightOcclusionShadow = nullptr; // R(Depth)G(LightMask)B(OcclusionMask)A(ShadowMask)
	RenderTarget*								m_specularPower = nullptr; // RGB(SpecularMask)A(Power)
	RenderTarget*								m_emissive = nullptr; // RGBA(Emissive)

	RenderTarget*								m_light = nullptr;
	RenderTarget*								m_specular = nullptr;
	RenderTarget*								m_lightBlend = nullptr;

	RenderTarget*								m_div2a = nullptr;
	RenderTarget*								m_div2b = nullptr;

};

ENGINE_END
