#pragma once

#include "IPhysicsObject.h"

ENGINE_BEGIN

class ENGINE_API ICollider abstract : public IPhysicsObject
{
public:

	ICollider() = default;

	virtual ~ICollider() = default;

public:

	virtual PxShape* GetPxShape() const = 0;

};

ENGINE_END
