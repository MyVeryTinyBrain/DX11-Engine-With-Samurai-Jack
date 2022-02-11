#pragma once

#include "RenderQueueBase.h"

ENGINE_BEGIN

class ENGINE_API RenderQueueStandard : public RenderQueueBase
{
private:

	// ���� ��û���� ����Ǵ� �����Դϴ�.
	using Vector_RenderRequests = vector<RenderRequest>;

	// ���� ��û�� �޽��� ���� �з��Ǵ� ������ ���Դϴ�.
	using Map_RenderRequestsByMesh = unordered_map<IMesh*, Vector_RenderRequests>;

	// ���� ��û�� ������ ���� �з��Ǵ� ������ ���Դϴ�.
	using Map_RenderRequestsByMaterial = unordered_map<IMaterial*, Map_RenderRequestsByMesh>;

	// ���� ��û�� ������ ���� �з��Ǵ� ���� ���Դϴ�.
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
