#pragma once

ENGINE_BEGIN

class ENGINE_API IPhysicsObject abstract
{
public:

	IPhysicsObject() = default;

	virtual ~IPhysicsObject() = default;

public:

	// 물리 시뮬레이션 이전에 호출됩니다.
	// 한 프레임에 여러번 호출될수 있습니다.
	virtual void BeforePhysicsSimulation() {}

	// 물리 시뮬레이션 이후에 호출됩니다.
	// 한 프레임에 여러번 호출될수 있습니다.
	virtual void AfterPhysicsSimulation() {}

	// 물리 시뮬레이션 이전에 호출됩니다.
	// 한 프레임에 한 번만 호출됩니다.
	virtual void BeforePhysicsSimulationOnce() {}

	// 물리 시뮬레이션 이후에 호출됩니다.
	// 한 프레임에 한 번만 호출됩니다.
	virtual void AfterPhysicsSimulationOnce() {}

	virtual bool IsWorking() const = 0;

public:

	virtual bool IsCCTComponent() const = 0;
};

ENGINE_END
