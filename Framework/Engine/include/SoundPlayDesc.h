#pragma once

struct FMOD_CHANNELGROUP;

ENGINE_BEGIN

struct SoundPlayDesc
{
	bool				Loop;
	float				Volume;
	float				Pitch;
	int					Priority;
	FMOD_CHANNELGROUP*	Group;
};

ENGINE_END
