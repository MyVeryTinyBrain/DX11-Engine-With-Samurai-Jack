#pragma once

struct FMOD_SOUND;
struct FMOD_SYSTEM;

ENGINE_BEGIN

class ENGINE_API Sound
{
private:
	Sound(FMOD_SOUND* sound);
	~Sound();

public:

	static Com<Sound> Create(FMOD_SOUND* sound);

	ULONG AddRef();
	ULONG Release();
	inline ULONG GetRefCnt() const { return m_refCnt; }

	_declspec(property(get = GetRefCnt)) ULONG refCnt;

public:

	inline bool IsValid() const { return m_fmodSound != nullptr; }
	inline FMOD_SOUND* GetFMODSound() const { return m_fmodSound; }
	FMOD_SYSTEM* GetFMODSystem() const;

	_declspec(property(get = IsValid)) bool isValid;
	_declspec(property(get = GetFMODSound)) FMOD_SOUND* fmodsound;
	_declspec(property(get = GetFMODSystem)) FMOD_SYSTEM* fmodsystem;

public:

	void SetVolume(float value) { m_volume = value; }
	float GetVolume() const { return m_volume; }
	void SetPitch(float value) { m_pitch = value; }
	float GetPitch() const { return m_pitch; }

	_declspec(property(get = GetVolume, put = SetVolume)) float volume;
	_declspec(property(get = GetPitch, put = SetPitch)) float pitch;

public:

	tstring GetPath() const;

	_declspec(property(get = GetPath)) tstring path;

private:

	ULONG				m_refCnt = 0;
	FMOD_SOUND*			m_fmodSound;

	float				m_volume = 1.0f;
	float				m_pitch = 1.0f;
};

ENGINE_END
