#pragma once

#include "RenderQueueBase.h"
#include "ILight.h"

ENGINE_BEGIN
class ILight;
class DepthStencil;
class VIBuffer;
class CompiledShaderDesc;
class ENGINE_API LightRenderQueue : public RenderQueueBase
{
	using InstanceRequets = unordered_map<IMesh*, vector<RenderRequest>>;
	using InstanceCutoffRequests = unordered_map<float, InstanceRequets>;

	enum class Shader_AccumulateLight_Type
	{ 
		Directional,
		Point,
		Spot,
	};

public:

	LightRenderQueue() = default;

	virtual ~LightRenderQueue();

public:

	HRESULT Initialize(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferManager) override;

	virtual bool IsValidInput(const RenderRequest& input) const override;

	virtual bool AddInput(const RenderRequest& input) override;

	virtual void Render(ICamera* camera) override;

	virtual void RenderOnce(ICamera* camera, const RenderRequest& request);

	virtual void Clear() override;

private:

	bool IsValidLight(ICamera* camera, ILight* light) const;
	bool IsValidShadowRequest(const RenderRequest& request, const BoundingHolder& boundingHolder) const;
	void ApplyCBufferForLight(const RenderRequest& request, const LightDesc& lightDesc, uint projectionIndex);

private:

	void Render_DepthOfLight(ICamera* camera, ILight* light, const LightDesc& lightDesc);
	void Render_DepthOfLight_Instance_NonCutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Instance_Cutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Instance(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex, bool cutoff, const InstanceRequets& requests);
	void Render_DepthOfLight_Skinned_NonCutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Skinned_Cutoff(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Skinned(const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex, bool cutoff);

	void Render_LightAccumulate(ICamera* camera, ILight* light, LightDesc lightDesc);

	void Render_LightBlend(ICamera* camera);

private:

	HRESULT SetupQuad();
	HRESULT SetupShaders();
	HRESULT SetupDepthes();

private:

	VIBuffer*									m_normalizedQuad = nullptr;		// Immutable Vertices

	CompiledShaderDesc*							m_shaderLightDepthWrite = nullptr;
	CompiledShaderDesc*							m_shaderLighting = nullptr;
	CompiledShaderDesc*							m_shaderLightBlending = nullptr;

	uint										m_lightDepthMapWidth = 2048;
	uint										m_lightDepthMapHeight = 2048;
	DepthStencil*								m_lightDepth[6] = {};

private:

	// For static meshes
	InstanceRequets								m_instanceRequets;
	InstanceCutoffRequests						m_instanceCutoffRequets;

	vector<RenderRequest>						m_skinnedRequests;
	vector<RenderRequest>						m_skinnedCutoffRequests;

private:

	enum
	{
		LIGHT_DEPTH_SHADER_TECHNIQUE_INSTANCE = 0,
		LIGHT_DEPTH_SHADER_TECHNIQUE_SKINNED = 1,
		LIGHT_DEPTH_SHADER_PASS_NONCUTOFF = 0,
		LIGHT_DEPTH_SHADER_PASS_CUTOFF = 1,
	};
};

ENGINE_END
