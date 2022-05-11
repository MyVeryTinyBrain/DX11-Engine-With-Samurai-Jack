#include "EnginePCH.h"
#include "Sound.h"
#include <fmod.h>

Sound::Sound(FMOD_SOUND* sound)
{
	m_fmodSound = sound;
}

Sound::~Sound()
{
	FMOD_SYSTEM* fmod = nullptr;
	if (FMOD_OK != FMOD_Sound_GetSystemObject(m_fmodSound, &fmod))
		return;

	if (FMOD_OK == FMOD_Sound_Release(m_fmodSound))
	{
		m_fmodSound = nullptr;
	}
}

Com<Sound> Sound::Create(FMOD_SOUND* sound)
{
	return new Sound(sound);
}

ULONG Sound::AddRef()
{
	return ++m_refCnt;
}

ULONG Sound::Release()
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

FMOD_SYSTEM* Sound::GetFMODSystem() const
{
	FMOD_SYSTEM* fmod = nullptr;
	if (FMOD_OK != FMOD_Sound_GetSystemObject(m_fmodSound, &fmod))
		return nullptr;

	return fmod;
}

tstring Sound::GetPath() const
{
	char cstr_name[512];
	FMOD_Sound_GetName(m_fmodSound, cstr_name, 512);
	return string_to_tstring(cstr_name);
}
