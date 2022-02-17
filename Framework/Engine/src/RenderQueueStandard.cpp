#include "EnginePCH.h"
#include "RenderQueueStandard.h"

RenderQueueStandard::~RenderQueueStandard()
{
	Clear();
}

bool RenderQueueStandard::IsValidInput(const RenderRequest& input) const
{
	return input.essential.IsValid() || !input.essential.instance;
}

bool RenderQueueStandard::AddInput(const RenderRequest& input)
{
	if (!input.essential.IsValid())
		return false;

	// ���� ������ ���� �����ϴ�.
	Map_RenderRequestsByMaterial& materialMap = m_requests[input.essential.renderGroupOrder];

	// ������ ���� �����ϴ�.
	IMaterial* material = input.essential.material;
	Map_RenderRequestsByMesh& meshMap = materialMap[material];

	// �޽��� ���� �����ϴ�.
	IMesh* mesh = input.essential.mesh;
	Vector_RenderRequests& requests = meshMap[mesh];

	// ��û�� �����մϴ�.
	requests.push_back(input);
	return true;
}

void RenderQueueStandard::Render(ICamera* camera)
{
	Com<ID3D11Device> device = m_graphicSystem->device;
	Com<ID3D11DeviceContext> deviceContext = m_graphicSystem->deviceContext;

	IMaterial* prevMaterial = nullptr;
	IMesh* prevMesh = nullptr;

	for (auto& pairByOrder : m_requests)
	{
		Map_RenderRequestsByMaterial& mapByMaterial = pairByOrder.second;

		for (auto& pairByMaterial : mapByMaterial)
		{
			Map_RenderRequestsByMesh& mapByMesh = pairByMaterial.second;
			IMaterial* material = pairByMaterial.first;

			Com<ID3DX11Effect> effect;
			material->GetEffectDesc(effect);
			m_CBufferManager->BeginApply(effect);
			{
				ApplyCameraBuffer(camera);

				for (auto& pairByMesh : mapByMesh)
				{
					Vector_RenderRequests& requests = pairByMesh.second;
					IMesh* mesh = pairByMesh.first;

					for (auto& request : requests)
					{
						if ((camera->GetAllowedLayers() & (1 << request.essential.layerIndex)) == 0)
							continue;

						if (!CullOp(camera, request.op.cullOp))
							continue;

						ApplyMaterial(deviceContext, material, request.essential.techniqueIndex, request.essential.passIndex, &prevMaterial);
						ApplyMesh(deviceContext, mesh, &prevMesh);
						ApplyBoneMatrices(request.op.boneOp, request.essential.subMeshIndex);
						ApplyWorldMatrix(request.essential.worldMatrix);
						if (FAILED(mesh->DrawSubMesh(deviceContext, request.essential.subMeshIndex)))
							continue;
					}
				}
			}
			m_CBufferManager->EndApply();
		}
	}
}

void RenderQueueStandard::Clear()
{
	if (!m_requests.empty())
		m_requests.clear();
}

bool RenderQueueStandard::CullOp(ICamera* camera, IRendererCullOp* cullOp) const
{
	if (!cullOp)
		return true;

	return cullOp->CullTest(camera);
}

void RenderQueueStandard::ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, IMaterial* material, uint techniqueIndex, uint passIndex, IMaterial** inout_prevMaterial)
{
	HRESULT hr = S_OK;

	if (!inout_prevMaterial)
		return;

	// �̹� ����� �����̶�� �������� �ʽ��ϴ�.
	if (*inout_prevMaterial == material)
		return;

	*inout_prevMaterial = material;

	material->SetMaterialValues();

	if (FAILED(hr = material->SetInputLayout(deviceContext, techniqueIndex, passIndex)))
		return;

	if (FAILED(hr = material->ApplyPass(deviceContext, techniqueIndex, passIndex)))
		return;
}

void RenderQueueStandard::ApplyMesh(Com<ID3D11DeviceContext> deviceContext, IMesh* mesh, IMesh** inout_prevMesh)
{
	HRESULT hr = S_OK;

	if (!inout_prevMesh)
		return;

	// �̹� ����� �޽ö�� �������� �ʽ��ϴ�.
	if (*inout_prevMesh == mesh)
		return;

	*inout_prevMesh = mesh;

	if (FAILED(hr = mesh->ApplyVertexBuffer(deviceContext)))
		return;

	if (FAILED(hr = mesh->ApplyIndexBuffer(deviceContext)))
		return;
}

void RenderQueueStandard::ApplyCameraBuffer(ICamera* camera)
{
	m_CBufferManager->ApplyCameraBuffer(camera->GetPosition(), camera->GetDirection(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetNear(), camera->GetFar());
}

void RenderQueueStandard::ApplyBoneMatrices(IRendererBoneOp* boneOp, uint subMeshIndex)
{
	m_CBufferManager->ApplyBoneMatricesUsageBuffer(boneOp != nullptr);

	if (!boneOp)
		return;

	boneOp->OnSetBoneMatricesCBuffer(subMeshIndex, m_CBufferManager->GetBoneMatricesBufferData());
	m_CBufferManager->ApplyBoneMatrices();
}

void RenderQueueStandard::ApplyWorldMatrix(const M4& worldMatrix)
{
	m_CBufferManager->ApplyWorldMatrixBuffer(worldMatrix);
}
