#pragma once

ENGINE_BEGIN

class ENGINE_API IPhysicsObject abstract
{
public:

	IPhysicsObject() = default;

	virtual ~IPhysicsObject() = default;

public:

	// ���� �ùķ��̼� ������ ȣ��˴ϴ�.
	virtual void BeforePhysicsSimulation() = 0;

	// ���� �ùķ��̼� ���Ŀ� ȣ��˴ϴ�.
	virtual void AfterPhysicsSimulation() = 0;

	virtual bool IsWorking() = 0;
};

ENGINE_END
