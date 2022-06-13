#include "EnginePCH.h"
#include "AudioSource.h"
#include <fmod.h>
#include "SoundSystem.h"
#include "System.h"
#include "AudioClip.h"
#include "AudioSourcePlaydata.h"
#include "SoundChannel.h"
#include "Transform.h"

void AudioSource::Awake()
{
	FMOD_RESULT fr = FMOD_System_CreateChannelGroup(system->sound->fmod, NULL, &m_fmodChannelGroup);
	assert(FMOD_OK == fr); // 채널그룹 생성이 실패했습니다.

	FMOD_ChannelGroup_SetMode(m_fmodChannelGroup, FMOD_3D_LINEARROLLOFF);

	SetPlayAnywhere(false + fr * 0.0f);
	Set3DMinDistance(5.0f);
	Set3DMaxDistance(10.0f);
}

void AudioSource::LateUpdate()
{
	// 오디오소스의 3D 공간에서의 출력 위치를 설정합니다.
	if (!IsPlayAnywhere())
	{
		V3 position = transform->position;
		FMOD_VECTOR fmodPosition, fmodVelocity;
		memcpy(&fmodPosition, &position, sizeof(FMOD_VECTOR));
		fmodVelocity = {};

		FMOD_ChannelGroup_Set3DAttributes(m_fmodChannelGroup, &fmodPosition, &fmodVelocity);
	}
}

void AudioSource::OnDestroyed()
{
	if (m_fmodChannelGroup)
	{
		FMOD_ChannelGroup_Release(m_fmodChannelGroup);
		m_fmodChannelGroup = nullptr;
	}

	Stop();
}

void AudioSource::OnEnable()
{
}

void AudioSource::OnDisable()
{
	Stop();
}

ResourceRef<AudioClip> AudioSource::GetClip() const
{
	return m_clip;
}

void AudioSource::SetClip(ResourceRef<AudioClip> clip)
{
	m_clip = clip;
}

void AudioSource::PlayOneshot(ResourceRef<AudioClip> clip, float volume, float pitch)
{
	if (!active)
		return;

	if (!clip)
		return;

	SoundPlayDesc desc = {};
	desc.Loop = false;
	desc.Volume = clip->volume * volume;
	desc.Pitch = clip->pitch * pitch;
	desc.Priority = m_priority;
	desc.Group = m_fmodChannelGroup;

	Com<SoundChannel> channel = system->sound->Play(clip->sound, desc);
	if (!channel)
		return;

	AudioSourcePlaydata* data = new AudioSourcePlaydata(this, clip, channel);
	m_playdatas.push_back(data);
}

void AudioSource::Play(ResourceRef<AudioClip> clip, float volume, float pitch)
{
	if (!active)
		return;

	if (!clip)
		return;

	SoundPlayDesc desc = {};
	desc.Loop = true;
	desc.Pitch = clip->volume * volume;
	desc.Volume = clip->pitch * pitch;
	desc.Priority = m_priority;
	desc.Group = m_fmodChannelGroup;

	Com<SoundChannel> channel = system->sound->Play(clip->sound, desc);
	if (!channel)
		return;

	AudioSourcePlaydata* data = new AudioSourcePlaydata(this, clip, channel);
	m_playdatas.push_back(data);
}

void AudioSource::PlayOneshot()
{
	PlayOneshot(m_clip);
}

void AudioSource::Play()
{
	Play(m_clip);
}

void AudioSource::Stop()
{
	while (!m_playdatas.empty())
	{
		m_playdatas.front()->Stop();
	}
}

bool AudioSource::IsPaused() const
{
	FMOD_BOOL paused;
	FMOD_ChannelGroup_GetPaused(m_fmodChannelGroup, &paused);
	return bool(paused);
}

void AudioSource::Pause(bool value)
{
	FMOD_BOOL paused = value;
	FMOD_ChannelGroup_SetPaused(m_fmodChannelGroup, paused);
}

float AudioSource::GetVolume() const
{
	float value;
	FMOD_ChannelGroup_GetVolume(m_fmodChannelGroup, &value);
	return value;
}

void AudioSource::SetVolume(float value)
{
	FMOD_ChannelGroup_SetVolume(m_fmodChannelGroup, Max(value, 0.0f));
}

float AudioSource::GetPitch() const
{
	float value;
	FMOD_ChannelGroup_GetPitch(m_fmodChannelGroup, &value);
	return value;
}

void AudioSource::SetPitch(float value)
{
	FMOD_ChannelGroup_SetPitch(m_fmodChannelGroup, Max(value, 0.0f));
}

bool AudioSource::IsPlayAnywhere() const
{
	FMOD_MODE mode;
	FMOD_ChannelGroup_GetMode(m_fmodChannelGroup, &mode);

	return !(mode & FMOD_3D);
}

void AudioSource::SetPlayAnywhere(bool value)
{
	FMOD_MODE mode;
	FMOD_ChannelGroup_GetMode(m_fmodChannelGroup, &mode);

	if (value)
	{
		mode &= (~FMOD_3D);
	}
	else
	{
		mode |= FMOD_3D;
	}
	FMOD_ChannelGroup_SetMode(m_fmodChannelGroup, mode);
}

float AudioSource::Get3DMinDistance() const
{
	float mind, maxd;
	FMOD_ChannelGroup_Get3DMinMaxDistance(m_fmodChannelGroup, &mind, &maxd);
	return mind;
}

void AudioSource::Set3DMinDistance(float value)
{
	float mind, maxd;
	FMOD_ChannelGroup_Get3DMinMaxDistance(m_fmodChannelGroup, &mind, &maxd);
	mind = value;
	FMOD_ChannelGroup_Set3DMinMaxDistance(m_fmodChannelGroup, mind, maxd);
}

float AudioSource::Get3DMaxDistance() const
{
	float mind, maxd;
	FMOD_ChannelGroup_Get3DMinMaxDistance(m_fmodChannelGroup, &mind, &maxd);
	return maxd;
}

void AudioSource::Set3DMaxDistance(float value)
{
	float mind, maxd;
	FMOD_ChannelGroup_Get3DMinMaxDistance(m_fmodChannelGroup, &mind, &maxd);
	maxd = value;
	FMOD_ChannelGroup_Set3DMinMaxDistance(m_fmodChannelGroup, mind, maxd);
}

float AudioSource::Get3DDopplerLevel() const
{
	float value;
	FMOD_ChannelGroup_Get3DDopplerLevel(m_fmodChannelGroup, &value);
	return value;
}

void AudioSource::Set3DDopplerLevel(float value)
{
	FMOD_ChannelGroup_Set3DDopplerLevel(m_fmodChannelGroup, value);
}

float AudioSource::GetNormalized3DLevel() const
{
	float value;
	FMOD_ChannelGroup_Get3DLevel(m_fmodChannelGroup, &value);
	return value;
}

void AudioSource::SetNormalized3DLevel(float value)
{
	FMOD_ChannelGroup_Set3DLevel(m_fmodChannelGroup, value);
}

void AudioSource::OnPlaydataEndCallback(AudioSourcePlaydata* tempChannel)
{
	auto find_it = std::find(m_playdatas.begin(), m_playdatas.end(), tempChannel);
	if (find_it != m_playdatas.end())
	{
		SafeDelete(*find_it);
		m_playdatas.erase(find_it);
	}
}
