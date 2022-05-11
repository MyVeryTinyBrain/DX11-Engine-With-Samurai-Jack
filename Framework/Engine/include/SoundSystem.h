#pragma once

#include "ISoundSystem.h"
#include <fmod_common.h>
#include "SoundPlayDesc.h"

ENGINE_BEGIN

class ENGINE_API SoundSystem : public ISoundSystem
{
public:

	SoundSystem();

	virtual ~SoundSystem();
	
public:

	Com<Sound> CreateSound(const tstring& path);
	Com<SoundListener> CreateListener();
	void ReleaseListener(Com<SoundListener>& listener);

public:

	inline FMOD_SYSTEM* GetFMOD() const { return m_fmod; }

	_declspec(property(get = GetFMOD)) FMOD_SYSTEM* fmod;

public:

	Com<SoundChannel> Play(Com<Sound> sound, const SoundPlayDesc& desc);
	void Stop(Com<SoundChannel> channel);
	bool IsPlaying(Com<SoundChannel> channel) const;

public:

	void SetMasterVolume(float value);
	float GetMasterVolume() const;
	void SetMasterPitch(float value);
	float GetMasterPitch() const;

	_declspec(property(get = GetMasterVolume, put = SetMasterVolume)) float materVolume;
	_declspec(property(get = GetMasterPitch, put = SetMasterPitch)) float materPitch;

private:

	virtual bool Initialize() override;
	virtual void Release() override;
	virtual void OnPostProcessing() override;

	static bool CreateFMODSystem(FMOD_SYSTEM** out_fmodsystem, int maxChannels, void* userData);
	static bool CreateChannels(FMOD_SYSTEM* fmodsystem, int maxChannels, vector<Com<SoundChannel>>& out_channels);

private:

	FMOD_SYSTEM*				m_fmod = nullptr;
	int							m_numChannels = 128;
	vector<Com<SoundChannel>>	m_channels;
	vector<Com<SoundListener>>	m_listeners;
};

ENGINE_END
