#include "EnginePCH.h"
#include "Channel.h"

Channel::Channel(const TCHAR* boneName, uint boneIndex, vector<KeyframeDesc>&& keyframes)
{
	_tcscpy_s(m_boneName, boneName);

	m_boneIndex = boneIndex;

	m_keyframes = keyframes;

	m_minTime = 0;
	if (m_keyframes.front().time < 0.0f)
	{
		m_minTime = m_keyframes.front().time;
	}

	m_maxTime = m_keyframes.back().time;
}

Channel::~Channel()
{
}

void Channel::GetKeyframeTransform(float time, V3& out_position, Q& out_rotation, V3& out_scale) const
{
	uint maxKeyframes = (uint)m_keyframes.size();

	uint nextIndex = ClosetIndex(time);
	nextIndex = Clamp(nextIndex, 0u, maxKeyframes - 1);

	uint index = nextIndex == 0 ? 0 : nextIndex - 1;

	float deltaTime = m_keyframes[nextIndex].time - m_keyframes[index].time;
	float ratio = (time - m_keyframes[index].time) / deltaTime;

	ratio = Clamp(ratio, 0.0f, 1.0f);

	if (deltaTime == 0)
		ratio = 0;

	out_position = V3::Lerp(m_keyframes[index].position, m_keyframes[nextIndex].position, ratio);
	out_rotation = Q::SLerp(m_keyframes[index].rotation, m_keyframes[nextIndex].rotation, ratio);
	out_scale = V3::Lerp(m_keyframes[index].scale, m_keyframes[nextIndex].scale, ratio);
}

uint Channel::ClosetIndex(float time) const
{
	auto it = std::lower_bound(m_keyframes.begin(), m_keyframes.end(), time,
		[](KeyframeDesc const& x, float d)
		{
			return x.time < d;
		});

	return (uint)std::distance(m_keyframes.begin(), it);
}
