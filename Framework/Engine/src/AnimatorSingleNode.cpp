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
	assert(animationClip != nullptr); // 애니메이션 클립이 nullptr입니다.
	return new AnimatorSingleNode(animationClip, loop);
}

bool AnimatorSingleNode::AnimateNodeImpl(
	uint channelIndex,
	uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale,
	V3& out_deltaPosition, Q& out_deltaRotation)
{
	if (!m_animationClip)
		return false;

	const AnimationChannel* channel = m_animationClip->GetChannel(channelIndex);
	if (!channel)
		return false;

	// 애니메이션의 길이입니다.
	float duration = GetDurationImpl();
	// 현재 재생중인 애니메이션의 정규화된 시간입니다.
	float newNormalizedTime = m_normalizedTime;

	// 애니메이션 루프 처리
	if (m_isLoop)
		newNormalizedTime = Mod(newNormalizedTime, 1.0f);
	else
		newNormalizedTime = Clamp01(newNormalizedTime);

	// 정규화된 애니메이션 재생시간과 애니메이션의 길이를 곱해,
	// 키프레임 시간을 구합니다.
	float time = newNormalizedTime * duration;
	float keyframeTime = time * m_animationClip->tickPerSecond;

	// 애니메이션의 키프레임에 해당하는 트랜스폼을 구합니다.
	channel->GetKeyframeTransform(keyframeTime, out_position, out_rotation, out_scale);

	out_nodeIndex = channel->boneIndex;

	if (m_rootNode && m_rootNode->node->index == out_nodeIndex)
	{
		float duration = GetDurationImpl();
		float prevNormalizedTime = Max(0.0f, m_normalizedTime - (m_dt / duration));

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
			V3 prevPosition; Q  prevRotation; V3 prevScale;
			channel->GetKeyframeTransform(prevTime, prevPosition, prevRotation, prevScale);

			float endTime = channel->maxTime;
			V3 endPosition; Q  endRotation; V3 endScale;
			channel->GetKeyframeTransform(endTime, endPosition, endRotation, endScale);

			float startTime = 0;
			V3 startPosition; Q  startRotation; V3 startScale;
			channel->GetKeyframeTransform(startTime, startPosition, startRotation, startScale);

			V3 prevToEndDeltaPosition = endPosition - prevPosition;
			Q  prevToEndDeltaRotation = endRotation * prevRotation.inversed;
			V3 startToCurrentDeltaPosition = out_position - startPosition;
			Q  startToCurrentDeltaRotation = out_rotation * startRotation.inversed;

			// 루트 모션에 사용하기 위해, 이전 본 트랜스폼과의 차이를 저장합니다.
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
