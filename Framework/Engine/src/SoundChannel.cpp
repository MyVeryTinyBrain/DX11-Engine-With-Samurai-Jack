#include "EnginePCH.h"
#include "SoundChannel.h"
#include <fmod.h>
#include "Sound.h"

SoundChannel::SoundChannel()
{
}

Com<SoundChannel> SoundChannel::Create()
{
	return new SoundChannel();
}

ULONG SoundChannel::AddRef()
{
	return ++m_refCnt;
}

ULONG SoundChannel::Release()
{
	if (m_refCnt <= 1)
	{
		delete this;
		return 0;
	}
	else
	{
		return --m_refCnt;
	}
}

void SoundChannel::OnPlayed(FMOD_CHANNEL* fmodchannel, Com<Sound> sound)
{
	m_channel = fmodchannel;

	FMOD_Channel_SetUserData(m_channel, this);
	FMOD_Channel_SetCallback(m_channel, SoundChannel::Callback);

	m_playedSound = sound;
}

void SoundChannel::Stop()
{
	FMOD_Channel_Stop(m_channel);
}

bool SoundChannel::IsValid() const
{
	return m_channel != nullptr;
}

int SoundChannel::GetIndex() const
{
	int index;
	FMOD_Channel_GetIndex(m_channel, &index);
	return index;
}

FMOD_SYSTEM* SoundChannel::GetFMODSystem() const
{
	FMOD_SYSTEM* fmod = nullptr;
	if (FMOD_RESULT::FMOD_OK != FMOD_Channel_GetSystemObject(m_channel, &fmod))
		return nullptr;

	return fmod;
}

Com<Sound> SoundChannel::GetPlayedSound() const
{
	return m_playedSound;
}

void SoundChannel::OnPlayEndCallback()
{
	OnPlayEnd(this);

	m_channel = nullptr;
	m_playedSound = nullptr;
}

FMOD_RESULT F_CALLBACK SoundChannel::Callback(FMOD_CHANNELCONTROL* ch, FMOD_CHANNELCONTROL_TYPE chType, FMOD_CHANNELCONTROL_CALLBACK_TYPE cbType, void* cd0, void* dc1)
{
	FMOD_CHANNEL* fmodchannel = (FMOD_CHANNEL*)ch;
	int index = -1;
	SoundChannel* channel = nullptr;

	if (FMOD_OK != FMOD_Channel_GetIndex(fmodchannel, &index))
		return FMOD_OK;

	if (FMOD_OK != FMOD_Channel_GetUserData(fmodchannel, (void**)&channel))
		return FMOD_OK;

	if (!channel)
		return FMOD_OK;

	if (chType == FMOD_CHANNELCONTROL_CHANNEL)
	{
		if (cbType == FMOD_CHANNELCONTROL_CALLBACK_END)
		{
			channel->OnPlayEndCallback();
		}
	}
	else
	{

	}

	return FMOD_OK;
}
