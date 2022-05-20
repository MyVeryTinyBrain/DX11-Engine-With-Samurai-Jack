#pragma once

ENGINE_BEGIN

struct AnimationEventDesc
{
	static const unsigned int MAX_CONTEXT = 512;
	float	NormalizedTime;
	char	Context[MAX_CONTEXT];
};

struct AnimationEvent
{
	AnimationEventDesc	Desc;
	bool				used;
};

ENGINE_END

