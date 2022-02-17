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

};

ENGINE_END
