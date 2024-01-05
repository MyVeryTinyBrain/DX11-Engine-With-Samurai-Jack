#pragma once

ENGINE_BEGIN

enum class FogType : unsigned int
{
	Distance,
	Z,
};

enum class BlurType : unsigned int
{
	Default,
	InvDepth,
	Depth,
};

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
	FogType	Type;
	float	MinZ;					// [0~inf]
	float	RangeZ;					// [0~inf]
	Color	Color;					// [rgba]
};

struct BloomDesc
{
	BOOL		Enable;				
	uint		BlurNumSamples;		// [0~16]
	float		Intensity;			// [0~inf]
	float		Threshold;			// [0~1]
	float		BlurPixelDistance;	// [0~inf]
};

struct SSRDesc
{
	BOOL		Enable;
	BOOL		BlurEnable;
	BlurType	BlurType;
	uint		NumSamples;			// [0~256]
	uint		BlurNumSamples;		// [0~16]
	float		Step;				// [0~inf]
	float		Thickness;			// [0~inf]
	float		Bias;				// [0~inf]
	float		BlurPixelDistance;	// [0~inf]
	float		ResolutionScale;	// [0~1]
};

struct DOFDesc
{
	BOOL	Enable;
	uint	BlurNumSamples;			// [0~16]
	float	MinZ;					// [0~inf]
	float	RangeZ;					// [0~inf]
	float	Power;					// [0~inf]
	float	BlurPixelDistance;		// [0~inf]
};

struct ChromaticAberrationDesc
{
	BOOL	Enable;
	float	StartSeperate;			// [0~1]
	float	MaxSeperate;			// [0~1]
	V4		Blend;					// [0~1] Blend.w: padding
	V4		Offset;					// [-inf~inf] Offset.w: padding
	V4		Angle;					// [-inf~inf] Angles.w: padding
};

struct BlurDesc
{
	BlurType	Type;
	uint		NumSamples; // [0~16]
	float		PixelDistance; // [0~inf]
};

ENGINE_END

