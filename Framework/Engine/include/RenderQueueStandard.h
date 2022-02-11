#pragma once

#include "RenderQueueBase.h"

ENGINE_BEGIN

class ENGINE_API RenderQueueStandard : public RenderQueueBase
{
private:

	// 렌더 요청들이 저장되는 벡터입니다.
	using Vector_RenderRequests = vector<RenderRequest>;

	// 렌더 요청의 메쉬에 따라 분류되는 비정렬 맵입니다.
	using Map_RenderRequestsByMesh = unordered_map<IMesh*, Vector_RenderRequests>;

	// 렌더 요청의 재질에 따라 분류되는 비정렬 맵입니다.
	using Map_RenderRequestsByMaterial = unordered_map<IMaterial*, Map_RenderRequestsByMesh>;

	// 렌더 요청의 순서에 따라 분류되는 정렬 맵입니다.
	using Map_RenderRequestsByOrder = map<int, Map_RenderRequestsByMaterial>;

public:

	RenderQueueStandard() = default;

	virtual ~RenderQueueStandard();

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

	Map_RenderRequestsByOrder					m_requests;
};

ENGINE_END
