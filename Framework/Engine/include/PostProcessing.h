#pragma once

#include "PostProcessingTypes.h"

ENGINE_BEGIN
class GraphicSystem;
class CBufferManager;
class InstanceBufferManager;
class VIBuffer;
class CompiledShaderDesc;
class ICamera;
class RenderTarget;
class ENGINE_API PostProcessing
{
	enum class Type
	{
		SSAO_WirteOcclusion,
		SSAO_ApplyOcclusion,
		HorizontalBlur,
		VerticalBlur,
		HorizontalDepthBlur,
		VerticalDepthBlur,
	};

public:

	enum class Step 
	{
		DEFERRED,
	};

public:

	PostProcessing(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferMaanger);

	~PostProcessing();

	HRESULT Initialize();

public:

	void PostProcess(ICamera* camera, PostProcessing::Step step);

private:

	void GetTehcniqueAndPass(Type type, uint& out_technique, uint& out_pass) const;
	void Render_SSAO_WriteOcclusion(ICamera* camera, const SSAODesc& ssaoDesc);
	void Render_SSAO_ApplyOcclusion(ICamera* camera, const SSAODesc& ssaoDesc);
	void Render_Blur(const BlurDesc& desc, RenderTarget* in, RenderTarget* bridge, RenderTarget* out);

private:

	HRESULT SetupQuads();
	HRESULT SetupShaders();

private:

	GraphicSystem* m_graphicSystem = nullptr;
	CBufferManager* m_CBufferManager = nullptr;
	InstanceBufferManager* m_instanceBufferManager = nullptr;

	VIBuffer* m_normalizedQuad = nullptr;		// Immutable Vertices

	CompiledShaderDesc* m_shaderPostProcessing = nullptr;
};

ENGINE_END
