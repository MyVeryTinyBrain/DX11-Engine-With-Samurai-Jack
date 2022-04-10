#pragma once

#include "IPhysicsSystem.h"
#include "PhysicsLayerManager.h"

ENGINE_BEGIN
class PhysicsSystem final : public IPhysicsSystem
{
public:

	PhysicsSystem();

	virtual ~PhysicsSystem();

public:

	inline const class PhysicsQuery* GetQuery() const { return m_physicsQuery; }

	V3 GetGravity() const;

	void SetGravity(V3 value);

	_declspec(property(get = GetQuery)) const class PhysicsQuery* query;
	_declspec(property(get = GetGravity, put = SetGravity)) V3 gravity;

private:

	inline PxPhysics* GetPhysics() const override { return m_physics; }

	inline PxCooking* GetCooking() const override { return m_cooking; }

	inline PxScene* GetScene() const override { return m_scene; }

private:

	virtual bool Initialize(System* system, unsigned int subStepLimit) override;

	virtual bool Release() override;

	void Simulate(unsigned int subStep, const map<uint, vector<Component*>>& executionBuffer) override;

	void SimulateOnce(const map<uint, vector<Component*>>& executionBuffer);

private:

	virtual void RegistPhysicsObject(IPhysicsObject* physicsObject) override;

	virtual void UnregistPhysicsObject(IPhysicsObject* physicsObject) override;

private:

	bool SetupScene();

private:

	System*									m_system = nullptr;

	PxDefaultAllocator*						m_allocater = nullptr;
	
	PxDefaultErrorCallback*					m_errorCallback = nullptr;
	
	PxFoundation*							m_foundation = nullptr;

	PxPvd*									m_pvd = nullptr;

	PxPvdTransport*							m_pvdTransport = nullptr;
	
	PxPhysics*								m_physics = nullptr;
	
	PxDefaultCpuDispatcher*					m_cpuDispatcher = nullptr;

	PxCooking*								m_cooking = nullptr;

	PxScene*								m_scene = nullptr;

	unsigned int							m_subStepLimit = 3;

	PhysicsLayerManager*					m_physicsLayerManager = nullptr;

	class PhysicsFilterShaderCallback*		m_physicsFilterShaderCallback = nullptr;

	class PhysicsSimulationEventCallback*	m_physicsSimulationEventCallback = nullptr;

	class PhysicsQuery*						m_physicsQuery = nullptr;

	list<IPhysicsObject*>					m_physicsObjects;
};
ENGINE_END
