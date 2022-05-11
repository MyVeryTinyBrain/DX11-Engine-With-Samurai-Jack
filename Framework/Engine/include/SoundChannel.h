#pragma once

#include <fmod_common.h>
#include "Sound.h"

ENGINE_BEGIN

class ENGINE_API SoundChannel
{
private:

	SoundChannel();
	virtual ~SoundChannel() = default;

public:

	static Com<SoundChannel> Create();

	ULONG AddRef();
	ULONG Release();
	inline ULONG GetRefCnt() const { return m_refCnt; }

	_declspec(property(get = GetRefCnt)) ULONG refCnt;

public:

	void OnPlayed(FMOD_CHANNEL* fmodchannel, Com<Sound> sound);

public:

	void Stop();

public:

	bool IsValid() const;
	int GetIndex() const;
	FMOD_SYSTEM* GetFMODSystem() const;
	inline FMOD_CHANNEL* GetFMODChannel() const { return m_channel; }

	_declspec(property(get = IsValid)) bool isValid;
	_declspec(property(get = GetIndex)) int index;
	_declspec(property(get = GetFMODSystem)) FMOD_SYSTEM* fmodsystem;
	_declspec(property(get = GetFMODChannel)) FMOD_CHANNEL* fmodchannel;
	
public:

	Com<Sound> GetPlayedSound() const;

	_declspec(property(get = GetPlayedSound)) Com<Sound> playedSound;

public:

	void OnPlayEndCallback();

private:

	static FMOD_RESULT F_CALLBACK Callback(FMOD_CHANNELCONTROL* ch, FMOD_CHANNELCONTROL_TYPE chType, FMOD_CHANNELCONTROL_CALLBACK_TYPE cbType, void* cd0, void* dc1);

private:

	ULONG			m_refCnt = 0;
	FMOD_CHANNEL*	m_channel = nullptr;
	Com<Sound>		m_playedSound;

public:

	delegate<void(SoundChannel*)> OnPlayEnd;
};

ENGINE_END
