#pragma once

#include "Ref.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class AudioSource;
class AudioClip;
class SoundChannel;
class Component;
class ENGINE_API AudioSourceOneshot
{
private:

	AudioSourceOneshot(Ref<AudioSource> src, ResourceRef<AudioClip> clip, SoundChannel* channel);
	~AudioSourceOneshot();

public:

	static bool Play(Ref<AudioSource> src, ResourceRef<AudioClip> clip);

private:

	void OnPlayEnd(SoundChannel* soundChannel);
	void OnAudioSourceDestroyed(Component* component);

private:

	Ref<AudioSource>		m_src;
	ResourceRef<AudioClip>	m_clip;
	SoundChannel*			m_channel;
};

ENGINE_END
