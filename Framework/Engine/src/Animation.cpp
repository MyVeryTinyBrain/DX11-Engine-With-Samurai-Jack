#include "EnginePCH.h"
#include "Animation.h"

Animation::Animation(const tstring& name, float duration, float tickPerSecond, vector<AnimationChannel*>&& channels) :
	m_name(name),
	m_duration(duration),
	m_tickPerSecond(tickPerSecond),
	m_channels(channels)
{
	if (m_tickPerSecond == 0)
		m_realtimeDuration = 0;
	else
		m_realtimeDuration = m_duration / m_tickPerSecond;
}

Animation::~Animation()
{
	for (auto& channel : m_channels)
	{
		SafeDelete(channel);
	}
	m_channels.clear();
}

const AnimationChannel* Animation::GetChannel(uint index) const
{
	if (index >= m_channels.size())
		return nullptr;

	return m_channels[index];
}
