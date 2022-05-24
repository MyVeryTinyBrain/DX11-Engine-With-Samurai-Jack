#pragma once

ENGINE_BEGIN

struct AnimationEventDesc
{
	static const unsigned int MAX_CONTEXT = 64;

	float	NormalizedTime;
	char	ContextStr[MAX_CONTEXT];
	int		ContextInt;
};

struct AnimationEvent
{
	AnimationEventDesc	Desc;
	bool				used;
};

ENGINE_END

