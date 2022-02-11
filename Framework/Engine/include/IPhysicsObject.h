#pragma once

ENGINE_BEGIN

class ENGINE_API IPhysicsObject abstract
{
public:

	IPhysicsObject() = default;

	virtual ~IPhysicsObject() = default;

public:

	// 물리 시뮬레이션 이전에 호출됩니다.
	virtual void BeforePhysicsSimulation() = 0;

	// 물리 시뮬레이션 이후에 호출됩니다.
	virtual void AfterPhysicsSimulation() = 0;

	virtual bool IsWorking() = 0;
};

ENGINE_END
