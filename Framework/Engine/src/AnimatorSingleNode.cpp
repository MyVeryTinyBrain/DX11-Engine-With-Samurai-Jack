#include "EnginePCH.h"
#include "AnimatorSingleNode.h"
#include "NodeTransform.h"
#include "Node.h"

AnimatorSingleNode::AnimatorSingleNode(const tstring& name, const SubResourceRef<AnimationClip>& animationClip, bool loop) :
	AnimatorNode(name, loop),
	m_animationClip(animationClip)
{
}

AnimatorSingleNode::AnimatorSingleNode(const SubResourceRef<AnimationClip>& animationClip, bool loop) :
	AnimatorNode(animationClip->name, loop),
	m_animationClip(animationClip)
{
}

AnimatorSingleNode* AnimatorSingleNode::Create(const tstring& name, const SubResourceRef<AnimationClip>& animationClip, bool loop)
{
	return new AnimatorSingleNode(name, animationClip, loop);
}

AnimatorSingleNode* AnimatorSingleNode::Create(const SubResourceRef<AnimationClip>& animationClip, bool loop)
{
	return new AnimatorSingleNode(animationClip, loop);
}

bool AnimatorSingleNode::AnimateNodeImpl(
	uint channelIndex,
	uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale,
	V3& out_deltaPosition, Q& out_deltaRotation)
{
	if (!m_animationClip)
		return false;

	const Channel* channel = m_animationClip->GetChannel(channelIndex);
	if (!channel)
		return false;

	float duration = GetDurationImpl();
	float newNormalizedTime = m_normalizedTime;
	if (m_isLoop)
		newNormalizedTime = Mod(newNormalizedTime, 1.0f);
	else
		newNormalizedTime = Clamp01(newNormalizedTime);
	float time = newNormalizedTime * duration;

	float keyframeTime = time * m_animationClip->tickPerSecond;

	channel->GetKeyframeTransform(keyframeTime, out_position, out_rotation, out_scale);
	out_nodeIndex = channel->boneIndex;

	if (m_rootNode && m_rootNode->node->index == out_nodeIndex)
	{
		float duration = GetDurationImpl();
		float prevNormalizedTime = Saturate(m_normalizedTime - (m_dt / duration));

		uint beforeLoopCount = uint(Abs(prevNormalizedTime));
		uint currentLoopCount = uint(Abs(m_normalizedTime));

		if (beforeLoopCount == currentLoopCount || !m_isLoop)
		{
			if (m_isLoop)
				prevNormalizedTime = Mod(prevNormalizedTime, 1.0f);
			else
				prevNormalizedTime = Clamp01(prevNormalizedTime);
			float prevTime = prevNormalizedTime * duration * m_animationClip->tickPerSecond;

			V3 prevPosition;
			Q  prevRotation;
			V3 prevScale;
			channel->GetKeyframeTransform(prevTime, prevPosition, prevRotation, prevScale);
			out_deltaPosition = out_position - prevPosition;
			out_deltaRotation = out_rotation * prevRotation.inversed;
		}
		else
		{
			float prevTime = Mod(prevNormalizedTime, 1.0f) * duration * m_animationClip->tickPerSecond;
			V3 prevPosition;
			Q  prevRotation;
			V3 prevScale;
			channel->GetKeyframeTransform(prevTime, prevPosition, prevRotation, prevScale);

			float endTime = channel->maxTime;
			V3 endPosition;
			Q  endRotation;
			V3 endScale;
			channel->GetKeyframeTransform(endTime, endPosition, endRotation, endScale);

			float startTime = 0;
			V3 startPosition;
			Q  startRotation;
			V3 startScale;
			channel->GetKeyframeTransform(startTime, startPosition, startRotation, startScale);

			V3 prevToEndDeltaPosition = endPosition - prevPosition;
			Q  prevToEndDeltaRotation = endRotation * prevRotation.inversed;
			V3 startToCurrentDeltaPosition = out_position - startPosition;
			Q  startToCurrentDeltaRotation = out_rotation * startRotation.inversed;
			out_deltaPosition = prevToEndDeltaPosition + startToCurrentDeltaPosition;
			out_deltaRotation = prevToEndDeltaRotation * startToCurrentDeltaRotation;
		}
	}

	return true;
}

float AnimatorSingleNode::GetDurationImpl() const
{
	if (!m_animationClip)
		return 0;

	return m_animationClip->realtimeDuration;
}

void AnimatorSingleNode::SetRootNode(const Ref<NodeTransform>& rootNode)
{
	m_rootNode = rootNode;
}
