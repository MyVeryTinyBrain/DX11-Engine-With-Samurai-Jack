#pragma once

#include "ResourceObject.h"
#include "Sound.h"

ENGINE_BEGIN

class ENGINE_API AudioClip : public ResourceObject
{
protected:

	AudioClip(
		ResourceManagement* management, bool managed, const tstring& path,
		Com<Sound> sound);

	virtual ~AudioClip();

public:

	void SetVolume(float value);
	float GetVolume() const;
	void SetPitch(float value);
	float GetPitch() const;

	_declspec(property(get = GetVolume, put = SetVolume)) float volume;
	_declspec(property(get = GetPitch, put = SetPitch)) float pitch;

public:

	inline Com<Sound> GetSound() const { return m_sound; }

	_declspec(property(get = GetSound)) Com<Sound> sound;

public:

	static ResourceRef<AudioClip> LoadAudioClipM(ResourceManagement* management, const tstring& path);
	static ResourceRef<AudioClip> LoadAudioClipUM(ResourceManagement* management, const tstring& path);

private:

	Com<Sound>		m_sound = nullptr;
};

ENGINE_END
