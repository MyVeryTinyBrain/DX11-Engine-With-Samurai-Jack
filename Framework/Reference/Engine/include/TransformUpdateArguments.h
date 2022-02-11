#pragma once

ENGINE_BEGIN
enum TransformUpdateFlag
{
	None = 0,
	FromEulerAngles = 1 << 0,
};

using TransformUpdateFlags = uint8_t;
ENGINE_END