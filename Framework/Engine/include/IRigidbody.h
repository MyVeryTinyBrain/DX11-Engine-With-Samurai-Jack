#pragma once

#include "IPhysicsObject.h"

ENGINE_BEGIN

class ENGINE_API IRigidbody abstract : public IPhysicsObject
{
public:
	
	IRigidbody() = default;

	virtual ~IRigidbody() = default;

public:

	virtual PxRigidDynamic* GetPxRigidDynamic() const = 0;

};

ENGINE_END
