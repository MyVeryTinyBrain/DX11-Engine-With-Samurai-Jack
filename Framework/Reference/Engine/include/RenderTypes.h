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

enum class TransparentLightMode { None, Use };

ENGINE_END
