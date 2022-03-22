#include "EnginePCH.h"
#include "AnimatorNode.h"
#include "NodeTransform.h"

AnimatorNode::AnimatorNode(const tstring& name, bool loop) :
	Object(name),
	m_isLoop(loop)
{
}

void AnimatorNode::Accumulate(float deltaTime)
{
	float duration = GetDurationImpl();
	float normalizedDeltaTime = deltaTime / duration;
	m_normalizedTime = Saturate(m_normalizedTime + normalizedDeltaTime);
	m_dt = deltaTime;
}

bool AnimatorNode::Animate(
	uint channelIndex, uint& out_nodeIndex, 
	V3& out_position, Q& out_rotation, V3& out_scale,
	V3& out_deltaPosition, Q& out_deltaRotation)
{
	return AnimateNodeImpl(channelIndex, out_nodeIndex, out_position, out_rotation, out_scale, out_deltaPosition, out_deltaRotation);
}

void AnimatorNode::SetNormalizedTime(float value)
{
	m_normalizedTime = Saturate(value);
}
