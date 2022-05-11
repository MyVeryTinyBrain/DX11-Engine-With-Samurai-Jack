#pragma once

#include "Ref.h"
#include "ResourceRef.h"
#include "SoundChannel.h"

ENGINE_BEGIN
class AudioSource;
class AudioClip;
class SoundChannel;
class SceneObject;
class ENGINE_API AudioSourcePlaydata
{
public:

	AudioSourcePlaydata(Ref<AudioSource> src, ResourceRef<AudioClip> clip, Com<SoundChannel> channel);
	~AudioSourcePlaydata();

public:

	void Stop();
	void Update();

private:

	void OnPlayEnd(SoundChannel* soundChannel);

private:

	Ref<AudioSource>		m_src;
	ResourceRef<AudioClip>	m_clip;
	Com<SoundChannel>		m_channel;
};

ENGINE_END
