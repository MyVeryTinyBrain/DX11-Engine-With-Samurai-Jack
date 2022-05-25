#include "EnginePCH.h"
#include "AnimatorLayer.h"
#include "SkinnedMeshRenderer.h"
#include "NodeTransform.h"
#include "AnimatorNode.h"
#include "System.h"
#include "Times.h"
#include "Node.h"
#include "Transform.h"
#include "Mesh.h"
#include "AnimatorBlendNodeElement.h"

AnimatorLayer::AnimatorLayer() :
	Object()
{
}

AnimatorLayer::AnimatorLayer(const tstring& name, AnimatorLayer::AnimateType type) :
	Object(name),
	m_type(type)
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
	AnimatorTransition::StartNode startNode;

	// 모든 노드에서 전환 가능한 트랜지션을 먼저 검사합니다.
	{
		auto any_find_it = m_transitions.find(nullptr);
		{
			if (!transition && any_find_it != m_transitions.end())
			{
				const vector<AnimatorTransition*>& anyTransitions = any_find_it->second;

				for (auto& currentTransition : anyTransitions)
				{
					if (currentTransition->IsTransferable(m_animator, this, m_currentNode, m_blendNode, m_currentTransition, startNode))
					{
						transition = currentTransition;
						break;
					}
				}
			}
		}
	}

	// 현재 노드에서의 트랜지션을 검사합니다.
	{
		auto find_it = m_transitions.find(m_currentNode);
		if (!transition && find_it != m_transitions.end())
		{
			const vector<AnimatorTransition*>& transitionsOfNode = find_it->second;

			for (auto& currentTransition : transitionsOfNode)
			{
				if (currentTransition->IsTransferable(m_animator, this, m_currentNode, m_blendNode, m_currentTransition, startNode))
				{
					transition = currentTransition;
					break;
				}
			}
		}
	}

	// 블렌딩중인 노드에서의 트랜지션을 검사합니다.
	if (m_blendNode)
	{
		auto find_it = m_transitions.find(m_blendNode);
		if (!transition && find_it != m_transitions.end())
		{
			const vector<AnimatorTransition*>& transitionsOfNode = find_it->second;

			for (auto& currentTransition : transitionsOfNode)
			{
				if (currentTransition->IsTransferable(m_animator, this, m_currentNode, m_blendNode, m_currentTransition, startNode))
				{
					transition = currentTransition;
					break;
				}
			}
		}
	}

	if (transition && m_currentTransition != transition)
	{
		if (startNode == AnimatorTransition::StartNode::Next)
		{
			m_currentNode = m_blendNode;
		}

		m_currentTransition = transition;
		m_currentTransition->Used();

		// nextNode == nullptr 이면 다음 노드는 default 노드가 됩니다.
		// 유니티의 Exit 노드와 같습니다.
		AnimatorNode* nextNode = m_currentTransition->nextNode;
		if (!nextNode)
			nextNode = m_defaultNode;

		if (m_blendNode != nextNode)
			nextNode->normalizedTime = m_currentTransition->offset;

		m_blendNode = nextNode;

		// 이 노드로 변경되려고 하는것을 이 노드에 알립니다.
		IAnimatorNode* iBlendNode = m_blendNode;
		iBlendNode->OnTransition();

		// For event notification
		m_beginChangingNode = m_blendNode;
	}
	else if(m_currentTransition)
	{
		m_blendNode->Accumulate(dt, m_eventDescs);
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
			// For event notification
			m_endChangedNode = m_blendNode;
			m_prevNode = m_currentNode;

			// 블렌딩을 종료하고 블렌딩 대상을 현재 노드로 설정합니다.
			m_currentNode = m_blendNode;
			m_blendNode = nullptr;
			m_blendPercent = 0.0f;
			m_currentTransition = nullptr;

			return;
		}
	}

	if (m_currentNode) 
	{
		m_currentNode->Accumulate(dt, m_eventDescs);
	}
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

AnimatorProperty* AnimatorLayer::AddProperty(const tstring& name, AnimatorProperty::Type type)
{
	auto find_it = m_properties.find(name);
	if (find_it != m_properties.end())
	{
		assert(0); // 중복된 이름을 사용했습니다.
		return nullptr;
	}

	AnimatorProperty* property = new AnimatorProperty(name, type);

	m_properties.emplace(property->name, property);

	return property;
}

AnimatorTransition* AnimatorLayer::AddTransition(
	AnimatorNode* startNode, AnimatorNode* nextNode, 
	const vector<AnimatorTransition::PropertyValue>& propertyValues, 
	float exitTime, float duration, float offset,
	AnimatorTransition::Interrupt interrupt, bool noRecursive)
{
	AnimatorTransition* transition = new AnimatorTransition(startNode, nextNode, propertyValues, exitTime, duration, offset, interrupt, noRecursive);

	vector<AnimatorTransition*>& transitionsOfNode = m_transitions[startNode];
	transitionsOfNode.push_back(transition);

	return transition;
}

void AnimatorLayer::Play(Ref<AnimatorNode> node, float normalizedTime)
{
	m_prevNode = m_currentNode;
	m_currentNode = node;
	m_blendNode = nullptr;
	m_currentTransition = nullptr;
	m_blendPercent = 0.0f;
	m_deltaPosition = V3::zero();
	m_deltaRotation = Q::identity();
	m_beginChangingNode = nullptr;
	m_endChangedNode = node;

	m_currentNode->normalizedTime = normalizedTime;

	// 이 노드로 변경되려고 하는것을 이 노드에 알립니다.
	IAnimatorNode* targetNode = m_currentNode;
	targetNode->OnTransition();
}

bool AnimatorLayer::IsPlaying(const tstring& nodeName) const
{
	if (m_currentNode)
	{
		if (m_currentNode->name == nodeName)
			return true;
	}

	if (m_blendNode)
	{
		if (m_blendNode->name == nodeName)
			return true;
	}

	return false;
}

bool AnimatorLayer::IsPlaying(Ref<AnimatorNode> node) const
{
	if (node.GetPointer() == m_currentNode)
		return true;

	if (node.GetPointer() == m_blendNode)
		return true;

	return false;
}

bool AnimatorLayer::IsContains(const tstring& str) const
{
	if (m_currentNode && m_currentNode->name.find(str) != tstring::npos)
		return true;

	if (m_blendNode && m_blendNode->name.find(str) != tstring::npos)
		return true;

	return false;
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

Ref<AnimatorNode> AnimatorLayer::GetBeginChangingNode() const
{
	return m_beginChangingNode;
}

Ref<AnimatorNode> AnimatorLayer::GetEndChangedNode() const
{
	return m_endChangedNode;
}

Ref<AnimatorNode> AnimatorLayer::GetPreviousNode() const
{
	return m_prevNode;
}

void AnimatorLayer::AnimateSingleNode(AnimatorNode* node, const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer)
{
	m_deltaPosition = V3::zero();
	m_deltaRotation = Q::identity();

	if (!node)
		return;

	if (node->IsEmpty())
		return;

	for (uint channelIndex = 0; channelIndex < skinnedMeshRenderer->nodeTransformCount; ++channelIndex)
	{
		uint nodeIndex;
		V3 position;
		Q  rotation;
		V3 scale;
		V3 deltaPosition;
		Q  deltaRotation;
		if (!node->Animate(channelIndex, nodeIndex, position, rotation, scale, deltaPosition, deltaRotation))
			continue;

		const Ref<NodeTransform>& nodeTransform = skinnedMeshRenderer->GetNodeTransformByIndex(nodeIndex);
		if (!nodeTransform)
			continue;

		if (m_rootNode && nodeTransform->node->index == m_rootNode->node->index)
		{
			m_deltaPosition = deltaPosition;
			m_deltaRotation = deltaRotation;

			position = m_rootNode->defaultLocalPosition;
			rotation = m_rootNode->defaultLocalRotation;
		}

		AnimateNodeTransform(nodeTransform, position, rotation, scale);
	}
}

void AnimatorLayer::AnimateSingleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer)
{
	AnimateSingleNode(m_currentNode, skinnedMeshRenderer);
}

void AnimatorLayer::AnimateDoubleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer)
{
	m_deltaPosition = V3::zero();
	m_deltaRotation = Q::identity();

	if (!m_currentNode)
		return;
	if (!m_blendNode)
		return;

	if (m_currentNode->IsEmpty() && m_blendNode->IsEmpty())
		return;
	else if (m_currentNode->IsEmpty())
		AnimateSingleNode(m_blendNode, skinnedMeshRenderer);
	else if(m_blendNode->IsEmpty())
		AnimateSingleNode(m_currentNode, skinnedMeshRenderer);

	for (uint channelIndex = 0; channelIndex < skinnedMeshRenderer->nodeTransformCount; ++channelIndex)
	{
		uint nodeIndex;
		V3 position[2];
		Q  rotation[2];
		V3 scale[2];
		V3 deltaPosition[2];
		Q  deltaRotation[2];

		if (!m_currentNode->Animate(channelIndex, nodeIndex, position[0], rotation[0], scale[0], deltaPosition[0], deltaRotation[0]))
			continue;
		if (!m_blendNode->Animate(channelIndex, nodeIndex, position[1], rotation[1], scale[1], deltaPosition[1], deltaRotation[1]))
			continue;

		const Ref<NodeTransform>& nodeTransform = skinnedMeshRenderer->GetNodeTransformByIndex(nodeIndex);
		if (!nodeTransform)
			continue;

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

		AnimateNodeTransform(nodeTransform, lerpPosition, lerpRotation, lerpScale);
	}
}

void AnimatorLayer::AnimateNodeTransform(const Ref<NodeTransform>& nodeTransform, const V3& t, const Q& r, const V3& s)
{
	switch (m_type)
	{
		case AnimatorLayer::AnimateType::Override:
		{
			nodeTransform->SetLocalTransformation(t, r, s);
		}
		break;
		case AnimatorLayer::AnimateType::Additive:
		{
			V3 deltaPos = t - nodeTransform->defaultLocalPosition;
			Q deltaRot = r * nodeTransform->defaultLocalRotation.inversed;
			V3 deltaScale = s - nodeTransform->defaultLocalScale;

			nodeTransform->SetLocalTransformation(
				nodeTransform->localPosition + deltaPos,
				/*r **/ nodeTransform->localRotation * deltaRot,
				nodeTransform->localScale + deltaScale
			);
		}
		break;
	}
}

void AnimatorLayer::SetSkinnedMeshRenderer(Ref<SkinnedMeshRenderer> skinnedMeshRenderer)
{
	m_skinnedMeshRenderer = skinnedMeshRenderer;
}

void AnimatorLayer::ClearTransitionEvents()
{
	m_beginChangingNode = nullptr;
	m_endChangedNode = nullptr;
	m_prevNode = nullptr;
}

void AnimatorLayer::ClearAnimationEvents()
{
	if (!m_eventDescs.empty())
	{
		m_eventDescs.clear();
	}
}

void AnimatorLayer::SetAnimator(Animator* animator)
{
	m_animator = animator;
}
