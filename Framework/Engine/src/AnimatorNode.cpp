#include "EnginePCH.h"
#include "AnimatorNode.h"
#include "NodeTransform.h"

AnimatorNode::AnimatorNode(const tstring& name, bool loop, bool isEmpty) :
	Object(name),
	m_isLoop(loop),
	m_isEmpty(isEmpty)
{
}

void AnimatorNode::AddEvent(const AnimationEventDesc& eventDesc)
{
	AnimationEvent animationEvent;
	animationEvent.Desc = eventDesc;
	animationEvent.used = false;
	m_events.push_back(animationEvent);

	SortEvents();
}

void AnimatorNode::Accumulate(float deltaTime, vector<AnimationEventDesc>& out_eventNotifications)
{
	uint prevLoop = uint(m_normalizedTime);

	deltaTime *= m_speed;

	float duration = GetDurationImpl();
	float normalizedDeltaTime = deltaTime / duration;
	m_normalizedTime = Max(0.0f, m_normalizedTime + normalizedDeltaTime);
	m_dt = deltaTime;

	uint curLoop = uint(m_normalizedTime);

	if (m_isLoop && prevLoop < curLoop)
	{
		OnLooped();
	}

	EventProcess(out_eventNotifications);
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
	m_normalizedTime = Max(0.0f, value);
}

void AnimatorNode::OnTransition()
{
	ClearEventUsed();
}

void AnimatorNode::OnLooped()
{
	ClearEventUsed();
}

void AnimatorNode::SortEvents()
{
	std::sort(m_events.begin(), m_events.end(), [](const AnimationEvent& lhs, const AnimationEvent& rhs)
		{
			return lhs.Desc.NormalizedTime < rhs.Desc.NormalizedTime;
		});
}

void AnimatorNode::ClearEventUsed()
{
	for (auto& animationEvent : m_events)
	{
		animationEvent.used = false;
	}
}

void AnimatorNode::EventProcess(vector<AnimationEventDesc>& out_notifications)
{
	float loopedNormalizedTime = Repeat(m_normalizedTime, 1.0f);

	uint numEvents = uint(m_events.size());
	for (uint i = 0; i < numEvents; ++i)
	{
		AnimationEvent& animationEvent = m_events[i];

		if (animationEvent.Desc.NormalizedTime > loopedNormalizedTime)
			return;

		if (!animationEvent.used && animationEvent.Desc.NormalizedTime < loopedNormalizedTime)
		{
			animationEvent.used = true;
			out_notifications.push_back(animationEvent.Desc);
		}
	}
}
