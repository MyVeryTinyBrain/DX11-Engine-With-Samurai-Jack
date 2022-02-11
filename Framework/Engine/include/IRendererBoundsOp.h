#pragma once

ENGINE_BEGIN

class ENGINE_API IRendererBoundsOp abstract
{
public:

	IRendererBoundsOp() = default;

	virtual ~IRendererBoundsOp() = default;

public:

	virtual Bounds GetBounds() const = 0;
};

ENGINE_END
