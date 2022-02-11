#pragma once

#include "IPhysicsObject.h"

ENGINE_BEGIN
class Component;
class System;
class ENGINE_API IPhysicsSystem abstract
{
public:

	virtual bool Initialize(System* system, unsigned int subStepLimit) = 0;

	virtual bool Release() = 0;

	virtual void Simulate(unsigned int subStep, const vector<Component*>& executionBuffer) = 0;

	virtual PxPhysics* GetPhysics() const = 0;

	virtual PxCooking* GetCooking() const = 0;

	virtual PxScene* GetScene() const = 0;

	virtual void RegistPhysicsObject(IPhysicsObject* physicsObject) = 0;

	virtual void UnregistPhysicsObject(IPhysicsObject* physicsObject) = 0;
};

ENGINE_END
