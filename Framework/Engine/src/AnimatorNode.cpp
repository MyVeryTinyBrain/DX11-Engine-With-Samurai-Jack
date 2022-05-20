#include "EnginePCH.h"
#include "AnimatorNode.h"
#include "NodeTransform.h"

AnimatorNode::AnimatorNode(const tstring& name, bool loop) :
	Object(name),
	m_isLoop(loop)
{
}

void AnimatorNode::AddEvent(float noramlizedTime, const string& context)
{
	AnimationEvent animationEvent = {};
	animationEvent.Desc.NormalizedTime = noramlizedTime;
	strcpy_s(animationEvent.Desc.Context, context.c_str());
	animationEvent.used = false;
	m_events.push_back(animationEvent);

	SortEvents();
}

void AnimatorNode::Accumulate(float deltaTime, vector<string>& out_eventNotifications)
{
	deltaTime *= m_speed;

	uint prevLoop = uint(m_normalizedTime);

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

void AnimatorNode::EventProcess(vector<string>& out_notifications)
{
	uint numEvents = uint(m_events.size());
	for (uint i = 0; i < numEvents; ++i)
	{
		AnimationEvent& animationEvent = m_events[i];

		if (animationEvent.Desc.NormalizedTime > m_normalizedTime)
			return;

		if (!animationEvent.used && animationEvent.Desc.NormalizedTime < m_normalizedTime)
		{
			animationEvent.used = true;
			out_notifications.push_back(animationEvent.Desc.Context);
		}
	}
}
