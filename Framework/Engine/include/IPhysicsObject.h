#pragma once

ENGINE_BEGIN

class ENGINE_API IPhysicsObject abstract
{
public:

	IPhysicsObject() = default;

	virtual ~IPhysicsObject() = default;

public:

	// ���� �ùķ��̼� ������ ȣ��˴ϴ�.
	// �� �����ӿ� ������ ȣ��ɼ� �ֽ��ϴ�.
	virtual void BeforePhysicsSimulation() {}

	// ���� �ùķ��̼� ���Ŀ� ȣ��˴ϴ�.
	// �� �����ӿ� ������ ȣ��ɼ� �ֽ��ϴ�.
	virtual void AfterPhysicsSimulation() {}

	// ���� �ùķ��̼� ������ ȣ��˴ϴ�.
	// �� �����ӿ� �� ���� ȣ��˴ϴ�.
	virtual void BeforePhysicsSimulationOnce() {}

	// ���� �ùķ��̼� ���Ŀ� ȣ��˴ϴ�.
	// �� �����ӿ� �� ���� ȣ��˴ϴ�.
	virtual void AfterPhysicsSimulationOnce() {}

	virtual bool IsWorking() const = 0;

public:

	virtual bool IsCCTComponent() const = 0;
};

ENGINE_END
