#pragma once

#include "RenderRequestInput.h"

ENGINE_BEGIN
class GraphicSystem;
class CBufferManager;
class InstanceBufferManager;
class VIBuffer;
class CompiledShaderDesc;
class ILight;
class ICamera;
class RenderTarget;
class DepthStencil;
class ENGINE_API DeferredScreenRender
{
public:

	enum class Blend { Blend, AlphaTest, PerspectiveDepthVisualize, OrthoDepthVisualize, None };

public:

	DeferredScreenRender(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferMaanger);

	~DeferredScreenRender();

	HRESULT Initialize();

public:

	void DrawTextureInClient(Com<ID3D11ShaderResourceView> in_srv, uint x, uint y, uint width, uint height, Blend blend = Blend::None);

	void DeferredDrawTexture(Com<ID3D11ShaderResourceView> in, Com<ID3D11RenderTargetView> out, Blend blend = Blend::None);

	void DeferredDrawBlurTexture(
		uint2 originalViewport, uint2 outViewport, uint blur, 
		Com<ID3D11ShaderResourceView> in, Com<ID3D11RenderTargetView> out);

	void DeferredStepLightDepthWrite(
		uint2 originalViewport, 
		ILight* in_light, ICamera* in_camera, 
		const unordered_map<IMesh*, vector<RenderRequest>>& shadowRenderInstanceRequests, const vector<RenderRequest>& shadowRenderSkinnedRequests,
		DepthStencil* const* out_lightDepthes);

	void DeferredStepLighting(
		ILight* in_light, ICamera* in_camera, 
		const RenderTarget* in_normal, const RenderTarget* in_worldPositions, const RenderTarget* in_depthLightOcclusionShadow, const RenderTarget* in_specularPower,
		DepthStencil* const* in_lightDepthes,
		RenderTarget* out_light, RenderTarget* out_specular);

	void DeferredStepLightBlending(
		const Color& ambient, 
		const RenderTarget* in_diffuse, const RenderTarget* in_depthLightOcclusionShadow, const RenderTarget* in_light, const RenderTarget* in_specular,
		const RenderTarget* out);

private:

	HRESULT SetupQuads();
	HRESULT SetupShaders();

private:

	GraphicSystem*									m_graphicSystem = nullptr;
	CBufferManager*									m_CBufferManager = nullptr;
	InstanceBufferManager*							m_instanceBufferManager = nullptr;

	VIBuffer*										m_normalizedQuad = nullptr;		// Immutable Vertices
	VIBuffer*										m_quad = nullptr;				// Dynamic Vertices

	CompiledShaderDesc*								m_shaderScreen = nullptr;
	CompiledShaderDesc*								m_shaderLightDepthWrite = nullptr;
	CompiledShaderDesc*								m_shaderLighting = nullptr;
	CompiledShaderDesc*								m_shaderLightBlending = nullptr;

};

ENGINE_END
