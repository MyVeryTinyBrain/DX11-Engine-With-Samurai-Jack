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
		SSR_Write,
		SSR_Apply,
		DOF_WritePass0,
		DOF_WritePass1,
		DOF_Apply,
		Fog_Apply_Z,
		Fog_Apply_Distance,
		Bloom_Extract,
		Bloom_Apply_Add,
		Bloom_Apply_Mix,
		ChromaticAberration_Write,
		ChromaticAberration_Apply,
		HorizontalBlur,
		VerticalBlur,
		HorizontalInvDepthBlur,
		VerticalInvDepthBlur,
		HorizontalDepthBlur,
		VerticalDepthBlur,
		DefaultScreen,
		AlphatestScreen,
		AlphablendScreen,
		AlphaToDarkScreen,
		LinearDepthScreen,
	};

public:

	enum class CopyType
	{
		Default,
		Alphatest,
		Alphablend,
		AlphaToDark,
		LinearDepth,
	};

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
	void Blur(const BlurDesc& desc, RenderTarget* in, RenderTarget* bridge, RenderTarget* out);
	void DrawToScreen(Com<ID3D11ShaderResourceView> src, Com<ID3D11RenderTargetView> dest, uint2 destSize, uint2 pos, uint2 size, CopyType type);
	void DrawToTextrue(Com<ID3D11ShaderResourceView> src, Com<ID3D11RenderTargetView> dest, uint2 destSize, CopyType type);

private:

	void PostProcess_Deferred(ICamera* camera);
	void PostProcess_After(ICamera* camera);

private:

	void GetTehcniqueAndPass(Type type, uint& out_technique, uint& out_pass) const;
	void GetBlurTechniqueAndPass(BlurType type, uint out_technique[2], uint out_pass[2]);
	void GetCopyTechniqueAndPass(CopyType type, uint& out_technique, uint& out_pass);

	void Render_SSAO_WriteOcclusion(ICamera* camera, const SSAODesc& ssaoDesc);
	void Render_SSAO_ApplyOcclusion(ICamera* camera, const SSAODesc& ssaoDesc);
	void Render_SSR_Write(ICamera* camera, const SSRDesc& ssrDesc);
	void Render_SSR_Apply(ICamera* camera, const SSRDesc& ssrDesc);
	void Render_DOF_Write(ICamera* camera, const DOFDesc& dofDesc);
	void Render_DOF_Apply(ICamera* camera, const DOFDesc& dofDesc);
	void Render_Fog_Apply(ICamera* camera, const FogDesc& fogDesc);
	void Render_Fog_Apply_Distance(ICamera* camera, const FogDesc& fogDesc);
	void Render_Fog_Apply_Z(ICamera* camera, const FogDesc& fogDesc);
	void Render_Bloom_Extract(ICamera* camera, const BloomDesc& bloomDesc);
	void Render_Bloom_Apply(ICamera* camera, const BloomDesc& bloomDesc);
	void Render_ChromaticAberration_Write(ICamera* camera, const ChromaticAberrationDesc& chromaticAberrationDesc);
	void Render_ChromaticAberration_Apply(ICamera* camera, const ChromaticAberrationDesc& chromaticAberrationDesc);
	void Render_Blur(const BlurDesc& desc, RenderTarget* in, RenderTarget* bridge, RenderTarget* out);

private:

	HRESULT SetupQuads();
	HRESULT SetupShaders();

private:

	GraphicSystem*				m_graphicSystem = nullptr;
	CBufferManager*				m_CBufferManager = nullptr;
	InstanceBufferManager*		m_instanceBufferManager = nullptr;

	VIBuffer*					m_normalizedQuad = nullptr;			// Immutable Vertices
	VIBuffer*					m_screenQuad = nullptr;				// Dynamic Vertices

	CompiledShaderDesc*			m_shaderPostProcessing = nullptr;
};

ENGINE_END
