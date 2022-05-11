#include "EnginePCH.h"
#include "AudioClip.h"
#include "ResourceManagement.h"
#include "System.h"
#include "SoundSystem.h"
#include "Sound.h"
#include <fmod.h>

AudioClip::AudioClip(ResourceManagement* management, bool managed, const tstring& path, Com<Sound> sound) :
	ResourceObject(management, managed, path)
{
	m_sound = sound;
}

AudioClip::~AudioClip()
{
}

void AudioClip::SetVolume(float value)
{
	m_sound->volume = value;
}

float AudioClip::GetVolume() const
{
	return m_sound->volume;
}

void AudioClip::SetPitch(float value)
{
	m_sound->pitch = value;
}

float AudioClip::GetPitch() const
{
	return m_sound->pitch;
}

ResourceRef<AudioClip> AudioClip::LoadAudioClipM(ResourceManagement* management, const tstring& path)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(path);
	if (find)
		return find;

	Com<Sound> sound = management->system->sound->CreateSound(path);
	if (!sound)
		return nullptr;

	return new AudioClip(management, true, path, sound);
}

ResourceRef<AudioClip> AudioClip::LoadAudioClipUM(ResourceManagement* management, const tstring& path)
{
	if (!management)
		return nullptr;

	Com<Sound> sound = management->system->sound->CreateSound(path);
	if (!sound)
		return nullptr;

	return new AudioClip(management, false, path, sound);
}
