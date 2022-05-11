#pragma once

struct FMOD_SOUND;
struct FMOD_SYSTEM;
struct FMOD_CHANNEL;

ENGINE_BEGIN

class ENGINE_API Sound
{
public:

	Sound(FMOD_SOUND* sound, const tstring& path);
	~Sound();

public:

	inline bool IsValid() const { return m_fmodSound != nullptr; }
	inline const FMOD_SOUND* GetFMODSound() const { return m_fmodSound; }
	FMOD_SYSTEM* GetFMODSystem() const;

	_declspec(property(get = IsValid)) bool isValid;
	_declspec(property(get = GetFMODSound)) FMOD_SOUND* fmodsound;
	_declspec(property(get = GetFMODSystem)) FMOD_SYSTEM* fmodsystem;

private:

	FMOD_SOUND*			m_fmodSound;
	tstring				m_path;
};

class ENGINE_API SoundChannel
{
public:

	SoundChannel(FMOD_CHANNEL* channel);

public:

	inline bool IsValid() const { return m_fmodChannel != nullptr; }
	inline FMOD_CHANNEL* GetFMODChannel() const { return m_fmodChannel; }
	inline int GetIndex() const { return m_index; }
	FMOD_SYSTEM* GetFMODSystem() const;

	_declspec(property(get = IsValid)) bool isValid;
	_declspec(property(get = GetFMODChannel)) FMOD_CHANNEL* fmodchannel;
	_declspec(property(get = GetIndex)) int index;
	_declspec(property(get = GetFMODSystem)) FMOD_SYSTEM* fmodsystem;

private:

	FMOD_CHANNEL*		m_fmodChannel;
	int					m_index;
};

ENGINE_END
