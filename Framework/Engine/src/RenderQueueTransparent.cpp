#include "EnginePCH.h"
#include "RenderQueueTransparent.h"

RenderQueueTransparent::~RenderQueueTransparent()
{
	Clear();
}

bool RenderQueueTransparent::IsValidInput(const RenderRequest& input) const
{
	return input.essential.IsValid() || !input.essential.instance;
}

bool RenderQueueTransparent::AddInput(const RenderRequest& input)
{
	if (!input.essential.IsValid())
		return false;

	m_requests[input.essential.renderGroupOrder].push_back(input);
	return true;
}

void RenderQueueTransparent::Render(ICamera* camera)
{
	Com<ID3D11Device> device = m_graphicSystem->device;
	Com<ID3D11DeviceContext> deviceContext = m_graphicSystem->deviceContext;

	IMaterial* prevMaterial = nullptr;
	IMesh* prevMesh = nullptr;

	for (auto& pairByOrder : m_requests)
	{
		Vector_RenderRequests& requests = pairByOrder.second;

		// 카메라와의 거리로 정렬합니다.
		sort(requests.begin(), requests.end(),
			[&](const RenderRequest& lhs, const RenderRequest& rhs)
			{
				float lhsDist = V3::Distance(lhs.essential.worldMatrix.GetTranslation(), camera->GetPosition());
				float rhsDist = V3::Distance(rhs.essential.worldMatrix.GetTranslation(), camera->GetPosition());
				return lhsDist < rhsDist;
			});

		for (auto& request : requests)
		{
			IMaterial* material = request.essential.material;
			IMesh* mesh = request.essential.mesh;

			Com<ID3DX11Effect> effect;
			material->GetEffectDesc(effect);
			m_CBufferManager->BeginApply(effect);
			{
				// Culling
				if (!CullOp(camera, request.op.cullOp))
					continue;

				ApplyCameraBuffer(camera);
				ApplyMaterial(deviceContext, material, request.essential.techniqueIndex, request.essential.passIndex, &prevMaterial);
				ApplyMesh(deviceContext, mesh, &prevMesh);
				ApplyBoneMatrices(request.op.boneOp, request.essential.subMeshIndex);
				ApplyWorldMatrix(request.essential.worldMatrix);
				if (FAILED(mesh->DrawSubMesh(deviceContext, request.essential.subMeshIndex)))
					continue;
			}
			m_CBufferManager->EndApply();
		}
	}
}

void RenderQueueTransparent::Clear()
{
	if (!m_requests.empty())
		m_requests.clear();
}

bool RenderQueueTransparent::CullOp(ICamera* camera, IRendererCullOp* cullOp) const
{
	if (!cullOp)
		return true;

	return cullOp->CullTest(camera);
}

void RenderQueueTransparent::ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, IMaterial* material, uint techniqueIndex, uint passIndex, IMaterial** inout_prevMaterial)
{
	HRESULT hr = S_OK;

	if (!inout_prevMaterial)
		return;

	// 이미 적용된 재질이라면 적용하지 않습니다.
	if (*inout_prevMaterial == material)
		return;

	*inout_prevMaterial = material;

	material->SetMaterialValues();

	if (FAILED(hr = material->SetInputLayout(deviceContext, techniqueIndex, passIndex)))
		return;

	if (FAILED(hr = material->ApplyPass(deviceContext, techniqueIndex, passIndex)))
		return;
}

void RenderQueueTransparent::ApplyMesh(Com<ID3D11DeviceContext> deviceContext, IMesh* mesh, IMesh** inout_prevMesh)
{
	HRESULT hr = S_OK;

	if (!inout_prevMesh)
		return;

	// 이미 적용된 메시라면 적용하지 않습니다.
	if (*inout_prevMesh == mesh)
		return;

	*inout_prevMesh = mesh;

	if (FAILED(hr = mesh->ApplyVertexBuffer(deviceContext)))
		return;

	if (FAILED(hr = mesh->ApplyIndexBuffer(deviceContext)))
		return;
}

void RenderQueueTransparent::ApplyCameraBuffer(ICamera* camera)
{
	m_CBufferManager->ApplyCameraBuffer(camera->GetPosition(), camera->GetDirection(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetNear(), camera->GetFar());
}

void RenderQueueTransparent::ApplyBoneMatrices(IRendererBoneOp* boneOp, uint subMeshIndex)
{
	m_CBufferManager->ApplyBoneMatricesUsageBuffer(boneOp != nullptr);

	if (!boneOp)
		return;

	boneOp->OnSetBoneMatricesCBuffer(subMeshIndex, m_CBufferManager->GetBoneMatricesBufferData());
	m_CBufferManager->ApplyBoneMatrices();
}

void RenderQueueTransparent::ApplyWorldMatrix(const M4& worldMatrix)
{
	m_CBufferManager->ApplyWorldMatrixBuffer(worldMatrix);
}
