#include "EnginePCH.h"
#include "SkinnedMeshRenderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Transform.h"
#include "RenderRequestInput.h"
#include "System.h"
#include "GraphicSystem.h"
#include "RenderQueue.h"
#include "NodeTransform.h"
#include "NodeSet.h"
#include "ResourceManagement.h"
#include "BuiltInResources.h"
#include "ResourceFactory.h"
#include "Texture2D.h"

void SkinnedMeshRenderer::Render()
{
	if (!isValid)
		return;

	//if (m_rootNodeTransform)
	//	m_rootNodeTransform->UpdateCombinedMatricesFromRoot();

	for (auto& nodeTransform : m_nodeTransformsByIndex)
		nodeTransform->UpdateCombinedMatrixSelf();

	for (uint i = 0; i < m_mesh->GetSubMeshCount() && i < m_materials.size(); ++i)
	{
		if (!m_mesh || !m_mesh->isValid)
			continue;

		ResourceRef<Material>& currentMaterial = m_materials[i];

		if (!currentMaterial || !currentMaterial->isValid)
			continue;

		uint passCount = currentMaterial->GetPassCountOfAppliedTechnique();

		for (uint j = 0; j < passCount; ++j)
		{
			RenderGroup renderGroup;
			bool cullingFlag;
			int renderGroupOrder;
			bool drawShadowFlag;
			bool shadowPassFlag;

			if (FAILED(currentMaterial->GetRenderGroupOfAppliedTechnique(j, renderGroup))) continue;
			if (FAILED(currentMaterial->GetRenderGroupOrderOfAppliedTechnique(j, renderGroupOrder))) continue;
			if (FAILED(currentMaterial->GetCullingFlagOfAppliedTechnique(j, cullingFlag))) continue;
			if (FAILED(currentMaterial->GetDrawShadowFlagOfAppliedTechnique(j, drawShadowFlag))) continue;
			if (FAILED(currentMaterial->GetShadowPassFlagOfAppliedTechnique(j, shadowPassFlag))) continue;

			RenderRequest input = {};
			input.essential.worldMatrix = transform->localToWorldMatrix;
			input.essential.renderGroup = renderGroup;
			input.essential.renderGroupOrder = renderGroupOrder;
			input.essential.layerIndex = m_layerIndex;
			input.essential.material = currentMaterial;
			input.essential.techniqueIndex = currentMaterial->techniqueIndex;
			input.essential.passIndex = j;
			input.essential.mesh = m_mesh;
			input.essential.subMeshIndex = i;
			input.essential.cull = cullingFlag;
			input.essential.instance = false;
			memcpy(input.essential.instanceData, GetInstanceDataArray(), sizeof(Real4) * 4);

			RenderRequestShadow shadow = {};
			shadow.draw = drawShadowFlag;
			shadow.shadowPass = shadowPassFlag;
			input.shadow = shadow;

			input.op.boneOp = this;
			input.op.cullOp = this;
			input.op.boundsOp = this;

			system->graphic->renderQueue->Add(input);
		}
	}
}

bool SkinnedMeshRenderer::CullTest(ICamera* camera) const
{
	return camera->Intersects(GetBounds());
}

Bounds SkinnedMeshRenderer::GetBounds() const
{
	if (!m_mesh)
		return Bounds(transform->position, V3::zero());

	Bounds bounds = m_mesh->GetBounds();
	float maxElement = V3::MaxAbsElement(bounds.extents) * 2.0f;
	bounds.extents = V3::one() * maxElement;

	bounds.Transform(transform->localToWorldMatrix);
	return bounds;
}

void SkinnedMeshRenderer::OnDestroyed()
{
	DeleteNodeTransforms();
}

bool SkinnedMeshRenderer::IsValid() const
{
	if (!Renderer::IsValid())
		return false;

	return true;
}

void SkinnedMeshRenderer::SetMesh(const ResourceRef<Mesh>& mesh)
{
	if (m_mesh == mesh)
		return;

	ResourceRef<Mesh> before = m_mesh;
	m_mesh = mesh;
	OnMeshChanged(before, mesh);

	SetupDefaultMaterials();

	SetupNodeTransforms();
}

uint SkinnedMeshRenderer::GetNodeTransformCount() const
{
	return uint(m_nodeTransformsByIndex.size());
}

Ref<NodeTransform> SkinnedMeshRenderer::GetNodeTransformByIndex(uint index) const
{
	if (index >= m_nodeTransformsByIndex.size())
		return nullptr;

	return m_nodeTransformsByIndex[index];
}

Ref<NodeTransform> SkinnedMeshRenderer::GetNodeTransformByName(const tstring& name) const
{
	auto find_it = m_nodeTransformsByName.find(name);
	if (find_it == m_nodeTransformsByName.end())
		return nullptr;

	return find_it->second;
}

void SkinnedMeshRenderer::OnSetBoneMatricesCBuffer(uint subMeshIndex, BoneMatricesCBuffer* pCBuffer)
{
	if (!m_mesh || subMeshIndex >= m_mesh->subMeshCount)
		return;

	const vector<uint>& subMeshNodeIndices = m_mesh->GetSubMeshNodeIndices(subMeshIndex);
	uint count = (uint)subMeshNodeIndices.size();

	for (uint i = 0; i < count; ++i)
	{
		pCBuffer->_BoneMatrices[i] = m_nodeTransformsByIndex[subMeshNodeIndices[i]]->boneMatrix.transposed;
	}
}

void SkinnedMeshRenderer::DeleteNodeTransforms()
{
	for (auto& nodeTransform : m_nodeTransformsByIndex)
	{
		SafeDelete(nodeTransform);
	}
	m_rootNodeTransform = nullptr;
	m_nodeTransformsByIndex.clear();
	m_nodeTransformsByName.clear();

	vector<NodeTransform*> temp0;
	unordered_map<tstring, NodeTransform*> temp1;
	m_nodeTransformsByIndex.swap(temp0);
	m_nodeTransformsByName.swap(temp1);
}


void SkinnedMeshRenderer::SetupNodeTransforms()
{
	DeleteNodeTransforms();

	if (!m_mesh || !m_mesh->nodeSet || m_mesh->nodeSet->nodeCount == 0)
		return;

	const NodeSet* nodeSet = m_mesh->nodeSet;
	uint nodeCount = (uint)nodeSet->nodeCount;

	NodeTransform* rootNodeTransform = new NodeTransform(transform, nullptr, nullptr, nodeSet->rootNode);
	m_rootNodeTransform = rootNodeTransform;
	m_nodeTransformsByIndex.push_back(rootNodeTransform);
	m_nodeTransformsByName.emplace(rootNodeTransform->name, rootNodeTransform);

	uint parentIndex = 0;
	uint childIndex = 1;

	while (childIndex < nodeCount)
	{
		NodeTransform* parentNodeTransform = m_nodeTransformsByIndex[parentIndex];

		const Node* childNode = nodeSet->GetNodeByIndex(childIndex);

		// i번쨰 NodeTranform이 Parent NodeTranform이 아니라면 다음 부모를 선택합니다.
		if (childNode->parent->index != parentNodeTransform->node->index)
		{
			++parentIndex;
			continue;
		}

		NodeTransform* nodeTransform = new NodeTransform(transform, rootNodeTransform, parentNodeTransform, childNode);
		m_nodeTransformsByIndex.push_back(nodeTransform);
		bool result = m_nodeTransformsByName.emplace(nodeTransform->name, nodeTransform).second;
		assert(result); // m_nodeTransformsByName 에 같은 노드 이름을 가진 NodeTransform이 삽입되려고 하니다.

		++childIndex;
	}

	//for (uint i = 0; i < nodeCount - 1; ++i)
	//{
	//	// i: parent childNode index
	//	// 마지막 NodeTransform은 부모일 수 없으므로 nodeCount - 1 범위를 사용합니다.

	//	NodeTransform* parentNodeTransform = m_nodeTransformsByIndex[i];

	//	for (uint j = i + 1; j < nodeCount; ++j)
	//	{
	//		// j: current childNode index

	//		const Node* node = nodeSet->GetAnimationByIndex(j);

	//		// i번쨰 NodeTranform이 Parent NodeTranform이 아니라면 다음 부모를 선택합니다.
	//		if (node->parent->index != parentNodeTransform->node->index)
	//			break;

	//		NodeTransform* nodeTransform = new NodeTransform(rootNodeTransform, parentNodeTransform, node);
	//		m_nodeTransformsByIndex.push_back(nodeTransform);
	//		bool result = m_nodeTransformsByName.emplace(nodeTransform->name, nodeTransform).second;
	//		assert(result); // m_nodeTransformsByName 에 같은 노드 이름을 가진 NodeTransform이 삽입되려고 하니다.
	//	}
	//}
}
