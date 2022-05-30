#pragma once

#include "RenderQueueBase.h"

ENGINE_BEGIN
class RenderQueueLight;
class ENGINE_API RenderQueueInstance : public RenderQueueBase
{
private:

	// ���� ��û���� ����Ǵ� �����Դϴ�.
	using Vector_RenderRequests = vector<RenderRequest>;

	// ���� ��û�� ����޽� �ε����� ���� �з��Ǵ� ������ ���Դϴ�.
	using Map_RenderRequestsBySubMeshIndex = unordered_map<uint, Vector_RenderRequests>;

	// ���� ��û�� �޽��� ���� �з��Ǵ� ������ ���Դϴ�.
	using Map_RenderRequestsByMesh = unordered_map<IMesh*, Map_RenderRequestsBySubMeshIndex>;

	// ���� ��û�� ������ ���� �з��Ǵ� ������ ���Դϴ�.
	using Map_RenderRequestsByMaterial = unordered_map<IMaterial*, Map_RenderRequestsByMesh>;

	// ���� ��û�� ������ ���� �з��Ǵ� ���� ���Դϴ�.
	using Map_RenderRequestsByOrder = map<int, Map_RenderRequestsByMaterial>;

public:

	RenderQueueInstance(RenderQueueLight* renderQueueLight, bool transparentQueue);

	virtual ~RenderQueueInstance();

public:

	virtual bool IsValidInput(const RenderRequest & input) const override;

	virtual bool AddInput(const RenderRequest & input) override;

	virtual void Render(ICamera * camera) override;

	virtual void Clear() override;

private:

	bool CullOp(ICamera* camera, IRendererCullOp* cullOp) const;

	void ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, ICamera* camera, IMaterial * material, uint techniqueIndex, uint passIndex, uint& inout_prevPassIndex, IMaterial * *inout_prevMaterial);
	void IApplyMesh(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceDataBuffer, IMesh * mesh, IMesh * *inout_prevMesh);
	void ApplyCameraBuffer(ICamera * camera);
	void ApplyWorldMatrix(const M4 & worldMatrix);
	void IApplyBoneMatricesUsage();

private:

	RenderQueueLight*				m_light = nullptr;

	Map_RenderRequestsByOrder		m_requests;

	bool							m_transparentQueue;
};

ENGINE_END
