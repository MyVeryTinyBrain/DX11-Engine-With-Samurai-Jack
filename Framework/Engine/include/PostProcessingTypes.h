#pragma once

ENGINE_BEGIN

struct SSAODesc
{
	BOOL	Enable;
	uint	NumSamples;				// [0~16]
	uint	BlurNumSamples;			// [0~16]
	float	Transparency;			// [0~1]
	float	MinZ;					// [0~inf]
	float	Radius;					// [0~inf]
	float	Power;					// [0~inf]
	float	BlurPixelDistance;		// [0~inf]
};

struct FogDesc
{
	BOOL	Enable;					
	float	MinZ;					// [0~inf]
	float	RangeZ;					// [0~inf]
	Color	Color;					// [rgba]
};

struct LinearDOFDesc
{
	BOOL	Enable;
	uint	BlurNumSamples;			// [0~16]
	float	MinZ;					// [0~inf]
	float	RangeZ;					// [0~inf]
	float	Power;					// [0~inf]
	float	BlurPixelDistance;		// [0~inf]
};

enum class BloomType : unsigned int
{
	Add,
	Mix,
};

struct BloomDesc
{
	BOOL		Enable;				
	BloomType	Type;				
	uint		BlurNumSamples;		// [0~16]
	float		Intensity;			// [0~inf]
	float		Threshold;			// [0~1]
	float		BlurPixelDistance;	// [0~inf]
};

enum class BlurType : unsigned int
{
	Default,
	InvDepth,
	Depth,
};

struct BlurDesc
{
	BlurType	Type;
	uint		NumSamples; // [0~16]
	float		PixelDistance; // [0~inf]
};

ENGINE_END

