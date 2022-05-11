#pragma once

#include "Ref.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class AudioSource;
class AudioClip;
class SoundChannel;
class SceneObject;
class ENGINE_API AudioSourceTempChannel
{
public:

	AudioSourceTempChannel(Ref<AudioSource>	m_src, ResourceRef<AudioClip> m_clip, SoundChannel* channel);
	~AudioSourceTempChannel();

public:

	void Stop();
	void Update();

private:

	void OnPlayEnd(SoundChannel* soundChannel);

private:

	Ref<AudioSource>		m_src;
	ResourceRef<AudioClip>	m_clip;
	SoundChannel*			m_channel;
};

ENGINE_END
