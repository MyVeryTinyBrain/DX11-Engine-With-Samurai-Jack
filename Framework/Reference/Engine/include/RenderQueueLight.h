#pragma once

#include "RenderQueueBase.h"
#include "ILight.h"

ENGINE_BEGIN
class ILight;
class DepthStencil;
class VIBuffer;
class CompiledShaderDesc;
class ENGINE_API RenderQueueLight : public RenderQueueBase
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

	RenderQueueLight() = default;

	virtual ~RenderQueueLight();

public:

	HRESULT Initialize(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferManager) override;

	virtual bool IsValidInput(const RenderRequest& input) const override;

	virtual bool AddInput(const RenderRequest& input) override;

	// 그림자를 그리는 조명의 시점에서 모델들을 렌더링합니다.
	// 결과를 깊이 버퍼에만 작성합니다.
	void RenderDepthes(ICamera* camera);

	virtual void Render(ICamera* camera) override;

	virtual void Clear() override;

private:

	bool IsValidLight(ICamera* camera, ILight* light) const;
	bool IsValidShadowRequest(ICamera* camera, const RenderRequest& request, const BoundingHolder& boundingHolder) const;
	void ApplyCBufferForLight(const RenderRequest& request, const LightDesc& lightDesc, uint projectionIndex);

private:

	void Render_DepthOfLight(ICamera* camera, ILight* light, const LightDesc& lightDesc);
	void Render_DepthOfLight_Instance_NonCutoff(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Instance_Cutoff(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Instance(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex, bool cutoff, const InstanceRequets& requests);
	void Render_DepthOfLight_Skinned_NonCutoff(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Skinned_Cutoff(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Skinned(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex, bool cutoff);

	void Render_LightAccumulate(ICamera* camera, ILight* light, LightDesc lightDesc);

	void Render_LightBlend(ICamera* camera);

private:

	HRESULT SetupQuad();
	HRESULT SetupShaders();

private:

	VIBuffer*									m_normalizedQuad = nullptr;		// Immutable Vertices

	CompiledShaderDesc*							m_shaderLightDepthWrite = nullptr;
	CompiledShaderDesc*							m_shaderLighting = nullptr;
	CompiledShaderDesc*							m_shaderLightBlending = nullptr;

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
