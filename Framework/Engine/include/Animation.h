#pragma once

#include "AnimationChannel.h"

ENGINE_BEGIN

class ENGINE_API Animation
{
public:

	Animation(const tstring& name, float duration, float tickPerSecond, vector<AnimationChannel*>&& channels);

	~Animation();

public:

	inline const tstring& GetName() const { return m_name; }

	inline float GetDuration() const { return m_duration; }

	inline float GetTickPerSecond() const { return m_tickPerSecond; }

	inline float GetRealtimeDuration() const { return m_realtimeDuration; }

	inline uint GetChannelCount() const { return uint(m_channels.size()); }

	const AnimationChannel* GetChannel(uint index) const;

	const vector<AnimationChannel*>& GetChannels() const { return m_channels; }

	_declspec(property(get = GetName)) const tstring& name;
	_declspec(property(get = GetDuration)) float duration;
	_declspec(property(get = GetTickPerSecond)) float tickPerSecond;
	_declspec(property(get = GetRealtimeDuration)) float realtimeDuration;
	_declspec(property(get = GetChannelCount)) uint channelCount;
	_declspec(property(get = GetChannels)) vector<AnimationChannel*>& channels;

private:

	tstring m_name;

	float m_duration;

	float m_tickPerSecond;

	float m_realtimeDuration;

	vector<AnimationChannel*> m_channels;

};

ENGINE_END
