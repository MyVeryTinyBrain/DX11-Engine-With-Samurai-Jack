#include "EnginePCH.h"
#include "AnimatorLayer.h"
#include "SkinnedMeshRenderer.h"
#include "NodeTransform.h"
#include "AnimatorNode.h"
#include "System.h"
#include "Times.h"
#include "Node.h"
#include "Transform.h"

AnimatorLayer::AnimatorLayer() :
	Object()
{
}

AnimatorLayer::AnimatorLayer(const tstring& name) :
	Object(name)
{
}

AnimatorLayer::~AnimatorLayer()
{
	for (auto& node : m_nodes)
		SafeDelete(node);
	m_nodes.clear();

	for (auto& prop : m_properties)
		SafeDelete(prop.second);
	m_properties.clear();

	for (auto& transitions : m_transitions)
	{
		for (auto& transition : transitions.second)
			SafeDelete(transition);
		transitions.second.clear();
	}
	m_transitions.clear();
}

void AnimatorLayer::Accumulate(float deltaTime, float speed)
{
	float dt = deltaTime * speed;

	AnimatorTransition* transition = nullptr;

	// 모든 노드에서 전환 가능한 트랜지션을 먼저 검사합니다.
	auto any_find_it = m_transitions.find(nullptr);
	if (!transition && any_find_it != m_transitions.end())
	{
		const vector<AnimatorTransition*>& anyTransitions = any_find_it->second;

		for (auto& currentTransition : anyTransitions)
		{
			if (currentTransition->IsTransferable())
			{
				transition = currentTransition;
				break;
			}
		}
	}

	// 현재 노드에서의 트랜지션을 검사합니다.
	auto find_it = m_transitions.find(m_currentNode);
	if (!transition && find_it != m_transitions.end())
	{
		const vector<AnimatorTransition*>& transitionsOfNode = find_it->second;

		for (auto& currentTransition : transitionsOfNode)
		{
			if (currentTransition->IsTransferable())
			{
				transition = currentTransition;
				break;
			}
		}
	}

	if (/*!m_currentTransition && */transition && m_currentTransition != transition)
	{
		m_currentTransition = transition;

		// nextNode == nullptr 이면 다음 노드는 default 노드가 됩니다.
		// 유니티의 Exit 노드와 같습니다.
		AnimatorNode* nextNode = m_currentTransition->nextNode;
		if (!nextNode)
			nextNode = m_defaultNode;

		if (m_blendNode != nextNode)
			nextNode->normalizedTime = m_currentTransition->offset;

		m_blendNode = nextNode;
	}
	else if(m_currentTransition)
	{
		m_blendNode->Accumulate(dt);
	}

	// 유효한 트랜지션이 있다면 전환을 시작합니다.
	if (m_currentTransition)
	{
		// 블렌딩 퍼센트를 설정합니다.
		if (m_currentTransition->duration <= 0)
			m_blendPercent = 1.0f;
		else
			m_blendPercent = (m_blendNode->normalizedTime - m_currentTransition->offset) / m_currentTransition->duration;

		if (m_blendPercent >= 1.0f)
		{
			// 블렌딩을 종료하고 블렌딩 대상을 현재 노드로 설정합니다.
			m_currentTransition->Used();
			m_currentNode = m_blendNode;
			m_blendNode = nullptr;
			m_blendPercent = 0.0f;
			m_currentTransition = nullptr;
			return;
		}
	}

	m_currentNode->Accumulate(dt);
}

void AnimatorLayer::Animate(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer)
{
	if (!m_currentTransition)
		AnimateSingleNode(skinnedMeshRenderer);
	else
		AnimateDoubleNode(skinnedMeshRenderer);
}

bool AnimatorLayer::AddNode(AnimatorNode* node)
{
	if (!node)
		return false;

	if (std::find(m_nodes.begin(), m_nodes.end(), node) != m_nodes.end())
		return false;

	if (m_rootNode)
	{
		IAnimatorNode* iAnimatorNode = node;
		iAnimatorNode->SetRootNode(m_rootNode);
	}

	if (!m_defaultNode)
		m_defaultNode = node;

	if (!m_currentNode)
		m_currentNode = node;

	m_nodes.push_back(node);
	return true;
}

bool AnimatorLayer::AddProperty(AnimatorProperty* property)
{
	if (!property)
		return false;

	auto find_it = m_properties.find(property->name);
	if (find_it != m_properties.end())
		return false;

	m_properties.emplace(property->name, property);
	return true;
}

bool AnimatorLayer::AddTransition(AnimatorTransition* transition)
{
	if (!transition)
		return false;

	vector<AnimatorTransition*>& transitionsOfNode = m_transitions[transition->startNode];

	auto find_it = std::find(transitionsOfNode.begin(), transitionsOfNode.end(), transition);
	if (find_it != transitionsOfNode.end())
		return false;

	transitionsOfNode.push_back(transition);

	return true;
}

void AnimatorLayer::SetRootNodeByName(const tstring& rootNodeName)
{
	if (!m_skinnedMeshRenderer)
		return;

	m_rootNode = m_skinnedMeshRenderer->GetNodeTransformByName(rootNodeName);

	for (auto& animatorNode : m_nodes)
	{
		IAnimatorNode* iAnimatorNode = animatorNode;
		iAnimatorNode->SetRootNode(m_rootNode);
	}
}

V3 AnimatorLayer::GetDeltaPosition() const
{
	return m_skinnedMeshRenderer->transform->localToWorldMatrix.MultiplyVector(m_deltaPosition);
}

Q AnimatorLayer::GetDeltaRotation() const
{
	return m_skinnedMeshRenderer->transform->rotation * m_deltaRotation;
}

void AnimatorLayer::AnimateSingleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer)
{
	uint channelIndex = 0;

	m_deltaPosition = V3::zero();
	m_deltaRotation = Q::identity();
	while (1)
	{
		uint nodeIndex;
		V3 position;
		Q  rotation;
		V3 scale;
		V3 deltaPosition;
		Q  deltaRotation;
		if (!m_currentNode->Animate(channelIndex, nodeIndex, position, rotation, scale, deltaPosition, deltaRotation))
			break;

		const Ref<NodeTransform>& nodeTransform = skinnedMeshRenderer->GetNodeTransformByIndex(nodeIndex);
		if (!nodeTransform)
			break;

		if (m_rootNode && nodeTransform->node->index == m_rootNode->node->index)
		{
			m_deltaPosition = deltaPosition;
			m_deltaRotation = deltaRotation;

			position = m_rootNode->defaultLocalPosition;
			rotation = m_rootNode->defaultLocalRotation;
		}

		nodeTransform->SetLocalTransformation(position, rotation, scale);

		++channelIndex;
	}
}

void AnimatorLayer::AnimateDoubleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer)
{
	uint channelIndex = 0;

	m_deltaPosition = V3::zero();
	m_deltaRotation = Q::identity();
	while (1)
	{
		uint nodeIndex;
		V3 position[2];
		Q  rotation[2];
		V3 scale[2];
		V3 deltaPosition[2];
		Q  deltaRotation[2];

		if (!m_currentNode->Animate(channelIndex, nodeIndex, position[0], rotation[0], scale[0], deltaPosition[0], deltaRotation[0]))
			break;
		if (!m_blendNode->Animate(channelIndex, nodeIndex, position[1], rotation[1], scale[1], deltaPosition[1], deltaRotation[1]))
			break;

		const Ref<NodeTransform>& nodeTransform = skinnedMeshRenderer->GetNodeTransformByIndex(nodeIndex);
		if (!nodeTransform)
			break;

		V3 lerpPosition = V3::Lerp(position[0], position[1], m_blendPercent);
		Q  lerpRotation = Q::SLerp(rotation[0], rotation[1], m_blendPercent);
		V3 lerpScale = V3::Lerp(scale[0], scale[1], m_blendPercent);

		if (m_rootNode && nodeTransform->node->index == m_rootNode->node->index)
		{
			V3 lerpDeltaPosition = V3::Lerp(deltaPosition[0], deltaPosition[1], m_blendPercent);
			Q  lerpDeltaRotation = Q::SLerp(deltaRotation[0], deltaRotation[1], m_blendPercent);

			m_deltaPosition = lerpDeltaPosition;
			m_deltaRotation = lerpDeltaRotation;

			lerpPosition = m_rootNode->defaultLocalPosition;
			lerpRotation = m_rootNode->defaultLocalRotation;
		}

		nodeTransform->SetLocalTransformation(lerpPosition, lerpRotation, lerpScale);

		++channelIndex;
	}
}

void AnimatorLayer::SetSkinnedMeshRenderer(Ref<SkinnedMeshRenderer> skinnedMeshRenderer)
{
	m_skinnedMeshRenderer = skinnedMeshRenderer;
}
