#pragma once

#include "Channel.h"

ENGINE_BEGIN

class ENGINE_API Animation
{
public:

	Animation(const tstring& name, float duration, float tickPerSecond, vector<Channel*>&& channels);

	~Animation();

public:

	inline const tstring& GetName() const { return m_name; }

	inline float GetDuration() const { return m_duration; }

	inline float GetTickPerSecond() const { return m_tickPerSecond; }

	inline float GetRealtimeDuration() const { return m_realtimeDuration; }

	inline size_t GetChannelCount() const { return m_channels.size(); }

	const Channel* GetChannel(uint index) const;

	_declspec(property(get = GetName)) const tstring& name;
	_declspec(property(get = GetDuration)) float duration;
	_declspec(property(get = GetTickPerSecond)) float tickPerSecond;
	_declspec(property(get = GetRealtimeDuration)) float realtimeDuration;
	_declspec(property(get = GetChannelCount)) size_t channelCount;

private:

	tstring m_name;

	float m_duration;

	float m_tickPerSecond;

	float m_realtimeDuration;

	vector<Channel*> m_channels;

};

ENGINE_END
