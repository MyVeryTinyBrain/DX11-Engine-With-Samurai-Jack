#pragma once

struct FMOD_CHANNELGROUP;

ENGINE_BEGIN

struct SoundPlayDesc
{
	bool				Loop;
	float				Volume;
	float				Pitch;
	FMOD_CHANNELGROUP*	Group;
};

ENGINE_END
