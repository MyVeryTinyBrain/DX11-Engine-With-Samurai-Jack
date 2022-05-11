#pragma once

#include "Ref.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class AudioSource;
class AudioClip;
class SoundChannel;
class Component;
class ENGINE_API AudioSourcePlayOneshot
{
public:

	AudioSourcePlayOneshot(Ref<AudioSource>	m_src, ResourceRef<AudioClip> m_clip, SoundChannel* channel);
	~AudioSourcePlayOneshot();

public:

	void Stop();

private:

	void OnPlayEnd(SoundChannel* soundChannel);
	void OnAudioSourceDestroyed(Component* component);

private:

	Ref<AudioSource>		m_src;
	ResourceRef<AudioClip>	m_clip;
	SoundChannel*			m_channel;
};

ENGINE_END
