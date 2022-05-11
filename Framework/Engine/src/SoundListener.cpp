#include "EnginePCH.h"
#include "SoundListener.h"
#include "SoundSystem.h"
#include <fmod.h>

SoundListener::SoundListener(FMOD_SYSTEM* fmodsystem, int index)
{
	m_fmodsystem = fmodsystem;
	m_index = index;
}

Com<SoundListener> SoundListener::Create(FMOD_SYSTEM* fmodsystem, int index)
{
	return new SoundListener(fmodsystem, index);
}

ULONG SoundListener::AddRef()
{
	return ++m_refCnt;
}

ULONG SoundListener::Release()
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

void SoundListener::Update(const V3& position, const V3& velocity, const V3& forward, const V3& up)
{
	FMOD_System_Set3DListenerAttributes(
		m_fmodsystem, 
		m_index, 
		(FMOD_VECTOR*)&position, 
		(FMOD_VECTOR*)&velocity, 
		(FMOD_VECTOR*)&forward, 
		(FMOD_VECTOR*)&up
	);
}
