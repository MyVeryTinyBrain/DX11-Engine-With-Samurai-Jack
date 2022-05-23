#pragma once

ENGINE_BEGIN

struct AnimationEventDesc
{
	static const unsigned int MAX_CONTEXT = 512;
	float	NormalizedTime;
	char	ContextStr[MAX_CONTEXT];
	int		ContextInt;
	V4		ContextV4;
};

struct AnimationEvent
{
	AnimationEventDesc	Desc;
	bool				used;
};

ENGINE_END

