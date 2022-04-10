#pragma once

ENGINE_BEGIN

enum class RenderGroup 
{ 
	Priority, 
	Standard, 
	AlphaTest,
	Transparent, 
	Overlay, 
	Max 
};

enum class ShaderVariableType : uint
{
	Unknown,
	Other,
	Struct,
	Bool,
	Int,
	UInt,
	Float,
	Double,
	Vector,
	Matrix,
	Texture1D,
	Texture2D,
	Texture3D,
	Texture1DArray,
	Texture2DArray,
};

ENGINE_END
