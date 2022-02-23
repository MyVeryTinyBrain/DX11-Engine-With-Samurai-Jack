#pragma once

ENGINE_BEGIN

struct SSAODesc
{
	bool	Enable;
	uint	NumSamples; // [0~16]
	uint	BlurNumSamples; // [0~16]
	float	Transparency; // [0~1]
	float	MinZ;
	float	Radius; // [0~inf]
	float	Power;
	float	BlurPixelDistance; // [0~inf]
};

struct BlurDesc
{
	bool	DepthBlur;
	uint	NumSamples; // [0~16]
	float	PixelDistance; // [0~inf]
};

ENGINE_END

