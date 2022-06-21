#pragma once

#include "Component.h"
#include "ResourceRef.h"

struct FMOD_CHANNELGROUP;

ENGINE_BEGIN
class SoundChannel;
class AudioClip;
class AudioSourcePlaydata;
class ENGINE_API AudioSource : public Component
{
private:

	virtual void Awake() override;
	virtual void LateUpdate() override;
	virtual void OnDestroyed() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

public:

	ResourceRef<AudioClip> GetClip() const;
	void SetClip(ResourceRef<AudioClip> clip);

	_declspec(property(get = GetClip, put = SetClip)) ResourceRef<AudioClip> clip;

public:

	void PlayOneshot(ResourceRef<AudioClip> clip, float volume = 1.0f, float pitch = 1.0f);
	void Play(ResourceRef<AudioClip> clip, float volume = 1.0f, float pitch = 1.0f);

	void PlayOneshot();
	void Play();

	void Stop();
	bool IsPaused() const;
	void Pause(bool value);

public:

	int GetPriority() const { return m_priority; }
	void SetPriority(int value) { m_priority = value; }

	float GetVolume() const;
	void SetVolume(float value);

	float GetPitch() const;
	void SetPitch(float value);

	bool IsPlayAnywhere() const;
	void SetPlayAnywhere(bool value);

	bool IsPlaying() const;

	float Get3DMinDistance() const;
	void Set3DMinDistance(float value);
	float Get3DMaxDistance() const;
	void Set3DMaxDistance(float value);
	float Get3DDopplerLevel() const;
	void Set3DDopplerLevel(float value);
	float GetNormalized3DLevel() const;
	void SetNormalized3DLevel(float value);

	_declspec(property(get = IsPlaying)) bool isPlaying;

	_declspec(property(get = GetPriority, put = SetPriority)) int priority;

	_declspec(property(get = GetVolume, put = SetVolume)) float volume;
	_declspec(property(get = GetPitch, put = SetPitch)) float pitch;

	_declspec(property(get = IsPlayAnywhere, put = SetPlayAnywhere)) bool playAnywhere;
	_declspec(property(get = Get3DMinDistance, put = Set3DMinDistance)) float minDistance;
	_declspec(property(get = Get3DMaxDistance, put = Set3DMaxDistance)) float maxDistance;
	_declspec(property(get = Get3DDopplerLevel, put = Set3DDopplerLevel)) float dopplerLevel;
	_declspec(property(get = GetNormalized3DLevel, put = SetNormalized3DLevel)) float normalized3DLevel;

public:

	void OnPlaydataEndCallback(AudioSourcePlaydata* tempChannel);

private:

	FMOD_CHANNELGROUP*				m_fmodChannelGroup = nullptr;

	ResourceRef<AudioClip>			m_clip;
	list<AudioSourcePlaydata*>		m_playdatas;

	int m_priority = 100;
};

ENGINE_END
