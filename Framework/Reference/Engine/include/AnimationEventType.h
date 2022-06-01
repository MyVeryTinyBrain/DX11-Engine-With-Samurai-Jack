#pragma once

ENGINE_BEGIN

struct AnimationEventDesc
{
	static const unsigned int MAX_CONTEXT = 64;

	float			NormalizedTime = 0.0f;
	char			ContextByte = 0;
	int				ContextInt = 0;
	unsigned int	ContextUInt = 0;
	float			ContextFloat = 0.0f;
	char			ContextStr[MAX_CONTEXT] = {};
};

struct AnimationEvent
{
	AnimationEventDesc	Desc;
	bool				used;
};

ENGINE_END

