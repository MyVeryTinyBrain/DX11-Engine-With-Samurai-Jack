#pragma once

#include "RenderQueueBase.h"

ENGINE_BEGIN
class RenderQueueLight;
class ENGINE_API RenderQueueTransparent : public RenderQueueBase
{
private:

	// 렌더 요청들이 저장되는 벡터입니다.
	using Vector_RenderRequests = vector<RenderRequest>;

	// 정렬 가능한 렌더 요청들의 순서에 따라 분류되는 정렬 맵입니다.
	using Map_SortableRenderRequestsByOrder = map<int, Vector_RenderRequests>;

public:

	RenderQueueTransparent(RenderQueueLight* renderQueueLight) : RenderQueueBase(), m_light(renderQueueLight) {};

	virtual ~RenderQueueTransparent();

public:

	virtual bool IsValidInput(const RenderRequest& input) const override;

	virtual bool AddInput(const RenderRequest& input) override;

	virtual void Render(ICamera* camera) override;

	virtual void Clear() override;

private:

	bool CullOp(ICamera* camera, IRendererCullOp* cullOp) const;

	void ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, IMaterial* material, uint techniqueIndex, uint passIndex, IMaterial** inout_prevMaterial);

	void ApplyMesh(Com<ID3D11DeviceContext> deviceContext, IMesh* mesh, IMesh** inout_prevMesh);

	void ApplyCameraBuffer(ICamera* camera);

	void ApplyBoneMatrices(IRendererBoneOp* boneOp, uint subMeshIndex);

	void ApplyWorldMatrix(const M4& worldMatrix);

private:

	void BeginForwardLightRender(ICamera* camera);
	void EndForwardLightRender(ICamera* camera);

private:

	RenderQueueLight*						m_light = nullptr;

	Map_SortableRenderRequestsByOrder		m_requests;
};

ENGINE_END
