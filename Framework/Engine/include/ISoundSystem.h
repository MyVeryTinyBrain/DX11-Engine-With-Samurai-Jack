#pragma once

#include "SoundChannel.h"
#include "SoundListener.h"

struct FMOD_SOUND;
struct FMOD_CHANNEL;
struct FMOD_SYSTEM;

ENGINE_BEGIN
class Sound;
class ENGINE_API ISoundSystem abstract
{
public:

	ISoundSystem() {};
	virtual ~ISoundSystem() {};

public:

	virtual bool Initialize(int numChannels) = 0;;
	virtual void Release() = 0;
	virtual void OnPostProcessing() = 0;
};

ENGINE_END
