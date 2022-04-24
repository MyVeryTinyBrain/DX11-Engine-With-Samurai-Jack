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
	// 렌더 요청들이 저장되는 벡터입니다.
	using Vector_RenderRequests = vector<RenderRequest>;

	// 렌더 요청의 메쉬에 따라 분류되는 비정렬 맵입니다.
	using Map_RenderRequestsByMesh = unordered_map<IMesh*, Vector_RenderRequests>;

	// 렌더 요청의 재질에 따라 분류되는 비정렬 맵입니다.
	using Map_RenderRequestsByMaterial = unordered_map<IMaterial*, Map_RenderRequestsByMesh>;

	// 인스턴스 렌더 요청의 서브메쉬 인덱스에 따라 분류되는 비정렬 맵입니다.
	using IMap_RenderRequestsBySubMeshIndex = unordered_map<uint, Vector_RenderRequests>;

	// 인스턴스 렌더 요청의 메쉬에 따라 분류되는 비정렬 맵입니다.
	using IMap_RenderRequestsByMesh = unordered_map<IMesh*, IMap_RenderRequestsBySubMeshIndex>;

	// 인스턴스 렌더 요청의 재질에 따라 분류되는 비정렬 맵입니다.
	using IMap_RenderRequestsByMaterial = unordered_map<IMaterial*, IMap_RenderRequestsByMesh>;

	enum class Shader_AccumulateLight_Type
	{ 
		Directional,
		Point,
		Spot,
	};

	enum
	{
		LIGHT_DEPTH_SHADER_TECHNIQUE = 0,
		LIGHT_DEPTH_SHADER_PASS_STATIC = 0,
		LIGHT_DEPTH_SHADER_PASS_SKINNED = 1,
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
	virtual void PostProcessing(ICamera* camera);

	virtual void Clear() override;

private:

	bool IsValidLight(ICamera* camera, ILight* light) const;
	bool IsValidShadowRequest(ICamera* camera, const RenderRequest& request, const BoundingHolder& boundingHolder) const;
	void ApplyCBufferForLight(const RenderRequest& request, const LightDesc& lightDesc, uint projectionIndex);

	bool CullOp(const RenderRequest& request, const BoundingHolder& boundingHolder) const;

	void ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, ICamera* camera, IMaterial* material, uint techniqueIndex, uint passIndex, IMaterial** inout_prevMaterial);
	void ApplyMesh(Com<ID3D11DeviceContext> deviceContext, IMesh* mesh, IMesh** inout_prevMesh);
	void ApplyCameraBuffer(const LightDesc& lightDesc, uint projectionIndex);
	void ApplyBoneMatrices(IRendererBoneOp* boneOp, uint subMeshIndex);
	void ApplyWorldMatrix(const M4& worldMatrix);

	void IApplyMesh(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceDataBuffer, IMesh* mesh, IMesh** inout_prevMesh);
	void IApplyBoneMatricesUsage();
	 
private:

	void Render_DepthOfLight(ICamera* camera, ILight* light, const LightDesc& lightDesc);
	void Render_DepthOfLight_Static(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_Skinned(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_ShadowPass(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);
	void Render_DepthOfLight_ShadowPassInstance(ICamera* camera, const LightDesc& lightDesc, BoundingHolder* boundings, uint projectionIndex);

	void Render_LightAccumulate(ICamera* camera, ILight* light, LightDesc lightDesc, const VolumetricDesc& volumetricDesc);

	void Render_LightBlend(ICamera* camera);
	void Render_Volumetric(ICamera* camera);

private:

	HRESULT SetupQuad();
	HRESULT SetupShaders();

private:

	VIBuffer*						m_normalizedQuad = nullptr;		// Immutable Vertices

	CompiledShaderDesc*				m_shaderLightDepthWrite = nullptr;
	CompiledShaderDesc*				m_shaderLighting = nullptr;
	CompiledShaderDesc*				m_shaderLightBlending = nullptr;

private:

	Map_RenderRequestsByMesh		m_staticRequets;
	Map_RenderRequestsByMesh		m_skinnedRequests;
	Map_RenderRequestsByMaterial	m_shadowPassRequests;
	IMap_RenderRequestsByMaterial	m_shadowPassInstanceRequests;

	Com<ID3D11DeviceContext>		m_deferredDeviceContext;
};

ENGINE_END
