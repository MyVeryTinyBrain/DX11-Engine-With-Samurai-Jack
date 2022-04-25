#include "EnginePCH.h"
#include "RenderQueueInstance.h"

#include "RenderQueueLight.h"
#include "DeferredRenderTarget.h"
#include "RenderTarget.h"

RenderQueueInstance::RenderQueueInstance(RenderQueueLight* renderQueueLight, bool transparentQueue) :
	RenderQueueBase(),
	m_light(renderQueueLight),
	m_transparentQueue(transparentQueue)
{
}

RenderQueueInstance::~RenderQueueInstance()
{
	Clear();
}

bool RenderQueueInstance::IsValidInput(const RenderRequest& input) const
{
	return input.essential.IsValid() || input.essential.instance || input.shadow.shadowPass;
}

bool RenderQueueInstance::AddInput(const RenderRequest& input)
{
	if (!input.essential.IsValid())
		return false;

	// 렌더 순서에 따라 나눕니다.
	Map_RenderRequestsByMaterial& materialMap = m_requests[input.essential.renderGroupOrder];

	// 재질에 따라 나눕니다.
	IMaterial* material = input.essential.material;
	Map_RenderRequestsByMesh& meshMap = materialMap[material];

	// 메쉬에 따라 나눕니다.
	IMesh* mesh = input.essential.mesh;
	Map_RenderRequestsBySubMeshIndex& subMeshIndexMap = meshMap[mesh];

	// 서브메쉬 인덱스에 따라 나눕니다.
	uint subMeshIndex = input.essential.subMeshIndex;
	Vector_RenderRequests& requests = subMeshIndexMap[subMeshIndex];

	// 요청을 저장합니다.
	requests.push_back(input);
	return true;
}

void RenderQueueInstance::Render(ICamera* camera)
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

			for (auto& pairByMesh : mapByMesh)
			{
				Map_RenderRequestsBySubMeshIndex& mapByMesh = pairByMesh.second;
				IMesh* mesh = pairByMesh.first;

				for (auto& pairBySubMeshIndex : mapByMesh)
				{
					Vector_RenderRequests& requests = pairBySubMeshIndex.second;
					uint instanceRequestCount = uint(requests.size());
					if (instanceRequestCount == 0)
						continue;

					if (m_transparentQueue)
					{
						// 카메라와의 거리로 정렬합니다.
						sort(requests.begin(), requests.end(),
							[&](const RenderRequest& lhs, const RenderRequest& rhs)
							{
								float lhsDist = V3::Distance(lhs.essential.worldMatrix.GetTranslation(), camera->GetPosition());
								float rhsDist = V3::Distance(rhs.essential.worldMatrix.GetTranslation(), camera->GetPosition());
								return lhsDist > rhsDist;
							});
					}

					const RenderRequest& front = requests.front();

					uint drawCount = 0;
					m_instanceBufferManager->BeginSetDatas(m_graphicSystem->deviceContext, instanceRequestCount);
					{
						for (uint i = 0; i < instanceRequestCount; ++i)
						{
							const RenderRequest& request = requests[i];

							if ((camera->GetAllowedLayers() & (1 << request.essential.layerIndex)) == 0)
								continue;

							if (request.essential.cull && !CullOp(camera, request.op.cullOp))
								continue;

							InstanceData data;
							data.right = request.essential.worldMatrix.row[0];
							data.up = request.essential.worldMatrix.row[1];
							data.forward = request.essential.worldMatrix.row[2];
							data.position = request.essential.worldMatrix.row[3];

							m_instanceBufferManager->SetData(uint(drawCount), &data);
							++drawCount;
						}
					}
					m_instanceBufferManager->EndSetDatas(m_graphicSystem->deviceContext);

					if (drawCount == 0)
						continue;

					Com<ID3DX11Effect> effect;
					material->GetEffectDesc(effect);
					m_CBufferManager->BeginApply(effect);
					{
						ApplyCameraBuffer(camera);
						ApplyMaterial(deviceContext, camera, material, front.essential.techniqueIndex, front.essential.passIndex, &prevMaterial);
						IApplyMesh(deviceContext, m_instanceBufferManager->GetBuffer(), mesh, &prevMesh);
						ApplyWorldMatrix(front.essential.worldMatrix); // There's no meaning in this render queue
						IApplyBoneMatricesUsage(); // It always set to false(0)
						
						if (front.customPrimitiveCount.usePrimitiveCount)
						{
							if (FAILED(mesh->DrawInstanceSubMesh(deviceContext, front.essential.subMeshIndex, uint(drawCount), front.customPrimitiveCount.primitiveCount))) continue;
						}
						else
						{
							if (FAILED(mesh->DrawInstanceSubMesh(deviceContext, front.essential.subMeshIndex, uint(drawCount)))) continue;
						}
					}
					m_CBufferManager->EndApply();
				}
			}

			m_CBufferManager->EndApply();
		}
	}
}

void RenderQueueInstance::Clear()
{
	if (!m_requests.empty())
		m_requests.clear();
}

bool RenderQueueInstance::CullOp(ICamera* camera, IRendererCullOp* cullOp) const
{
	if (!cullOp)
		return true;

	return cullOp->CullTest(camera);
}

void RenderQueueInstance::ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, ICamera* camera, IMaterial* material, uint techniqueIndex, uint passIndex, IMaterial** inout_prevMaterial)
{
	HRESULT hr = S_OK;

	if (!inout_prevMaterial)
		return;

	// 이미 적용된 재질이라면 적용하지 않습니다.
	if (*inout_prevMaterial == material)
		return;

	*inout_prevMaterial = material;

	material->ApplyMaterial(deviceContext, camera);

	if (FAILED(hr = material->SetInputLayout(deviceContext, techniqueIndex, passIndex)))
		return;

	if (FAILED(hr = material->ApplyPass(deviceContext, techniqueIndex, passIndex)))
		return;
}

void RenderQueueInstance::IApplyMesh(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceDataBuffer, IMesh* mesh, IMesh** inout_prevMesh)
{
	HRESULT hr = S_OK;

	if (!inout_prevMesh)
		return;

	// 이미 적용된 메시라면 적용하지 않습니다.
	if (*inout_prevMesh == mesh)
		return;

	*inout_prevMesh = mesh;

	if (FAILED(hr = mesh->ApplyVertexAndInstanceBuffer(deviceContext, instanceDataBuffer)))
		return;

	if (FAILED(hr = mesh->ApplyIndexBuffer(deviceContext)))
		return;
}

void RenderQueueInstance::ApplyCameraBuffer(ICamera* camera)
{
	m_CBufferManager->ApplyCameraBuffer(m_graphicSystem->deviceContext, camera->GetPosition(), camera->GetDirection(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetSize(), camera->GetNear(), camera->GetFar());
}

void RenderQueueInstance::ApplyWorldMatrix(const M4& worldMatrix)
{
	// There's no meaning in this render queue
	m_CBufferManager->ApplyWorldMatrixBuffer(m_graphicSystem->deviceContext, worldMatrix);
}

void RenderQueueInstance::IApplyBoneMatricesUsage()
{
	m_CBufferManager->ApplyBoneMatricesUsageBuffer(m_graphicSystem->deviceContext, false);
}
