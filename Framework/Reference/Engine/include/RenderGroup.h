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

enum class ScreenRenderGroup
{
	Priority,
};

ENGINE_END
