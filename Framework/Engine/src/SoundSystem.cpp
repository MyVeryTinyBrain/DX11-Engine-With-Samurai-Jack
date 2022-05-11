#include "EnginePCH.h"
#include "SoundSystem.h"
#include <fmod.h>
#include "Sound.h"
#include "SoundChannel.h"
#include "SoundListener.h"

SoundSystem::SoundSystem()
{
	
}

SoundSystem::~SoundSystem()
{
	Release();
}

Com<Sound> SoundSystem::CreateSound(const tstring& path)
{
	FMOD_SOUND* fmodsound;
	string strPath = tstring_to_string(path);

	if (FMOD_OK != FMOD_System_CreateSound(m_fmod, strPath.c_str(), FMOD_CREATECOMPRESSEDSAMPLE, NULL, &fmodsound))
		return nullptr;
	
	Com<Sound> sound = Sound::Create(fmodsound);

	FMOD_Sound_SetUserData(fmodsound, sound.Get());

	return sound;
}

Com<SoundListener> SoundSystem::CreateListener()
{
	int index = int(m_listeners.size());
	int numListeners = index + 1;

	if (FMOD_OK != FMOD_System_Set3DNumListeners(m_fmod, numListeners))
		return nullptr;

	Com<SoundListener> listener = SoundListener::Create(m_fmod, index);
	m_listeners.push_back(listener);

	return listener;
}

void SoundSystem::ReleaseListener(Com<SoundListener>& listener)
{
	// 리스너 목록에서 제거합니다.
	auto find_it = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (find_it == m_listeners.end())
	{
		m_listeners.erase(find_it);
	}

	// 리스너 번호를 재설정합니다.
	int numListeners = int(m_listeners.size());
	for (int i = 0; i < numListeners; ++i)
	{
		ISoundListener* iSoundListener = m_listeners[i].Get();
		iSoundListener->SetIndex(i);
	}

	// 리스너 갯수를 설정합니다.
	FMOD_System_Set3DNumListeners(m_fmod, numListeners);

	listener = nullptr;
}

Com<SoundChannel> SoundSystem::Play(Com<Sound> sound, const SoundPlayDesc& desc)
{
	FMOD_CHANNEL* fmodchannel = nullptr;
	int index;

	if (FMOD_OK != FMOD_System_PlaySound(m_fmod, sound->fmodsound, NULL, FALSE, &fmodchannel))
		return nullptr;
	if (FMOD_OK != FMOD_Channel_GetIndex(fmodchannel, &index))
		return nullptr;

	FMOD_MODE playmode = FMOD_DEFAULT;
	if (desc.Loop) playmode |= FMOD_LOOP_NORMAL;
	if (FMOD_OK != FMOD_Channel_SetMode(fmodchannel, playmode))
		return nullptr;

	if (FMOD_OK != FMOD_Channel_SetVolume(fmodchannel, Max(desc.Volume, 0.0f)))
		return nullptr;
	if (FMOD_OK != FMOD_Channel_SetPitch(fmodchannel, Max(desc.Pitch, 0.0f)))
		return nullptr;

	FMOD_CHANNELGROUP* channelGroup = nullptr;
	if (!desc.Group)	{ FMOD_System_GetMasterChannelGroup(m_fmod, &channelGroup); }
	else				{ channelGroup = desc.Group; }
	if (desc.Group && FMOD_OK != FMOD_Channel_SetChannelGroup(fmodchannel, desc.Group))
		return nullptr;
	
	Com<SoundChannel> channel = m_channels[index];
	channel->OnPlayed(fmodchannel, sound);

	return channel;
}

void SoundSystem::Stop(Com<SoundChannel> channel)
{
	if (!channel || !channel->fmodchannel)
		return;

	FMOD_Channel_Stop(channel->fmodchannel);
}

bool SoundSystem::IsPlaying(Com<SoundChannel> channel) const
{
	FMOD_BOOL isPlaying;
	FMOD_Channel_IsPlaying(channel->fmodchannel, &isPlaying);
	return isPlaying;
}

void SoundSystem::SetMasterVolume(float value)
{
	FMOD_CHANNELGROUP* channelgroup;
	FMOD_System_GetMasterChannelGroup(m_fmod, &channelgroup);
	FMOD_ChannelGroup_SetVolume(channelgroup, Max(value, 0.0f));
}

float SoundSystem::GetMasterVolume() const
{
	float value;
	FMOD_CHANNELGROUP* channelgroup;
	FMOD_System_GetMasterChannelGroup(m_fmod, &channelgroup);
	FMOD_ChannelGroup_GetVolume(channelgroup, &value);
	return value;
}

void SoundSystem::SetMasterPitch(float value)
{
	FMOD_CHANNELGROUP* channelgroup;
	FMOD_System_GetMasterChannelGroup(m_fmod, &channelgroup);
	FMOD_ChannelGroup_SetPitch(channelgroup, Max(value, 0.0f));
}

float SoundSystem::GetMasterPitch() const
{
	float value;
	FMOD_CHANNELGROUP* channelgroup;
	FMOD_System_GetMasterChannelGroup(m_fmod, &channelgroup);
	FMOD_ChannelGroup_GetPitch(channelgroup, &value);
	return value;
}

bool SoundSystem::Initialize()
{
	if (!CreateFMODSystem(&m_fmod, m_numChannels, this))
		return false;

	if (!CreateChannels(m_fmod, m_numChannels, m_channels))
		return false;
	
	return true;
}

void SoundSystem::Release()
{
	m_channels.clear();
	m_listeners.clear();

	if (m_fmod)
	{
		FMOD_System_Release(m_fmod);
		FMOD_System_Close(m_fmod);
		m_fmod = nullptr;
	}
}

void SoundSystem::OnPostProcessing()
{
	FMOD_System_Update(m_fmod);
}

bool SoundSystem::CreateFMODSystem(FMOD_SYSTEM** out_fmodsystem, int maxChannels, void* userData)
{
	if (!out_fmodsystem)
		return false;

	if (FMOD_OK != FMOD_System_Create(out_fmodsystem, FMOD_VERSION))
		return false;

	if (FMOD_OK != FMOD_System_Init(*out_fmodsystem, maxChannels, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, NULL))
		return false;

	if (FMOD_OK != FMOD_System_Set3DSettings(*out_fmodsystem, 1.0f, 100.0f, 1.0f))
		return false;

	FMOD_System_SetUserData(*out_fmodsystem, userData);

	return true;
}

bool SoundSystem::CreateChannels(FMOD_SYSTEM* fmodsystem, int maxChannels, vector<Com<SoundChannel>>& out_channels)
{
	vector<Com<SoundChannel>> channels;

	for (int i = 0; i < maxChannels; ++i)
	{
		channels.push_back(SoundChannel::Create());
	}

	out_channels.swap(channels);

	return true;
}
