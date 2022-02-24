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
		Fog_Apply,
		Bloom_Extract,
		Bloom_Apply_Add,
		Bloom_Apply_Mix,
		LinearDOF_WritePass0,
		LinearDOF_WritePass1,
		LinearDOF_Apply,
		HorizontalBlur,
		VerticalBlur,
		HorizontalInvDepthBlur,
		VerticalInvDepthBlur,
		HorizontalDepthBlur,
		VerticalDepthBlur,
	};

public:

	enum class Step 
	{
		Deferred,
		After,
	};

public:

	PostProcessing(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferMaanger);

	~PostProcessing();

	HRESULT Initialize();

public:

	void PostProcess(ICamera* camera, PostProcessing::Step step);

private:

	void PostProcess_Deferred(ICamera* camera);
	void PostProcess_After(ICamera* camera);

private:

	void GetTehcniqueAndPass(Type type, uint& out_technique, uint& out_pass) const;

	void Render_SSAO_WriteOcclusion(ICamera* camera, const SSAODesc& ssaoDesc);
	void Render_SSAO_ApplyOcclusion(ICamera* camera, const SSAODesc& ssaoDesc);
	void Render_Fog_Apply(ICamera* camera, const FogDesc& fogDesc);
	void Render_Bloom_Extract(ICamera* camera, const BloomDesc& bloomDesc);
	void Render_Bloom_Apply(ICamera* camera, const BloomDesc& bloomDesc);
	void Render_Blur(const BlurDesc& desc, RenderTarget* in, RenderTarget* bridge, RenderTarget* out);
	void Render_LinearDOF_Write(ICamera* camera, const LinearDOFDesc& dofDesc);
	void Render_LinearDOF_Apply(ICamera* camera, const LinearDOFDesc& dofDesc);

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
