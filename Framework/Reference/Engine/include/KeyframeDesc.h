#pragma once

ENGINE_BEGIN

struct KeyframeDesc
{
	float		time;
	V3			position;
	Q			rotation;
	V3			scale;
};

//template <typename T>
//struct KeyframeDesc
//{
//	T			value;
//	float		time;
//};
//
//typedef KeyframeDesc<V3>	PositionKeyframeDesc;
//typedef KeyframeDesc<Q>		RotationKeyframeDesc;
//typedef KeyframeDesc<V3>	ScaleKeyframeDesc;

ENGINE_END
