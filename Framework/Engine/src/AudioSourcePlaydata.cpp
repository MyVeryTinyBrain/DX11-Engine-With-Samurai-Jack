#include "EnginePCH.h"
#include "AudioSourcePlaydata.h"
#include "AudioSource.h"
#include "AudioClip.h"
#include <fmod.h>
#include "System.h"
#include "SoundSystem.h"
#include "SoundChannel.h"

AudioSourcePlaydata::AudioSourcePlaydata(Ref<AudioSource> src, ResourceRef<AudioClip> clip, Com<SoundChannel> channel)
{
	m_src = src;
	m_clip = clip;
	m_channel = channel;

	m_channel->OnPlayEnd += func<void(SoundChannel*)>(this, &AudioSourcePlaydata::OnPlayEnd);
}

AudioSourcePlaydata::~AudioSourcePlaydata()
{
	if (m_channel)
	{
		m_channel->OnPlayEnd -= func<void(SoundChannel*)>(this, &AudioSourcePlaydata::OnPlayEnd);
	}

	m_src = nullptr;
	m_clip = nullptr;
}

void AudioSourcePlaydata::Stop()
{
	if (!m_src.IsNull())
	{
		m_src->system->sound->Stop(m_channel);
	}
}

void AudioSourcePlaydata::Update()
{
}

void AudioSourcePlaydata::OnPlayEnd(SoundChannel* soundChannel)
{
	if (!m_src.IsNull())
	{
		m_src->OnPlaydataEndCallback(this);
	}
}
