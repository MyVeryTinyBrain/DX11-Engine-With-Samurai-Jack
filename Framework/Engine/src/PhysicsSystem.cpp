#include "EnginePCH.h"
#include "PhysicsSystem.h"
#include "Component.h"
#include "PhysicsFilterShaderCallback.h"
#include "PhysicsSimulationEventCallback.h"
#include "System.h"
#include "Times.h"
#include "PhysicsQuery.h"

PhysicsSystem::PhysicsSystem()
{
	
}

PhysicsSystem::~PhysicsSystem()
{
    Release();
}

V3 PhysicsSystem::GetGravity() const
{
	PxVec3 pxGravity = m_scene->getGravity();
	return FromPxVec3(pxGravity);
}

void PhysicsSystem::SetGravity(V3 value)
{
	PxVec3 pxGravity = ToPxVec3(value);
	m_scene->setGravity(pxGravity);
}

bool PhysicsSystem::Initialize(System* system, unsigned int subStepLimit)
{
	if (!system)
		return false;

	m_system = system;

	if (subStepLimit == 0)
		RETURN_FALSE_ERROR_MESSAGE("PhysicsSystem::Initialize::subStepLimit is zero");
	
	m_allocater = new PxDefaultAllocator;

	m_errorCallback = new PxDefaultErrorCallback;

	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *m_allocater, *m_errorCallback);
	if (!m_foundation)
		RETURN_FALSE_ERROR_MESSAGE("PhysicsSystem::Initialize::PxCreateFoundation");
	
#ifdef _DEBUG
	m_pvd = PxCreatePvd(*m_foundation);
	if (!m_pvd)
		RETURN_FALSE_ERROR_MESSAGE("PhysicsSystem::Initialize::PxCreatePvd");
	m_pvdTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if(!m_pvdTransport)
		RETURN_FALSE_ERROR_MESSAGE("PhysicsSystem::Initialize::PxDefaultPvdSocketTransportCreate");
	m_pvd->connect(*m_pvdTransport, PxPvdInstrumentationFlag::eALL);
#endif

	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, m_pvd);
	if (!m_physics)
		RETURN_FALSE_ERROR_MESSAGE("PhysicsSystem::Initialize::PxCreatePhysics");

	m_cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	if (!m_cpuDispatcher)
		RETURN_FALSE_ERROR_MESSAGE("PhysicsSystem::Initialize::PxDefaultCpuDispatcherCreate");

	m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, PxCookingParams(m_physics->getTolerancesScale()));
	if (!m_cooking)
		RETURN_FALSE_ERROR_MESSAGE("PhysicsSystem::Initialize::PxCreateCooking");

	m_physicsLayerManager = new PhysicsLayerManager;

	m_physicsFilterShaderCallback = new PhysicsFilterShaderCallback(m_physicsLayerManager);

	m_physicsSimulationEventCallback = new PhysicsSimulationEventCallback;

	if (!SetupScene())
		return false;

	m_controllerManager = PxCreateControllerManager(*m_scene);

	m_physicsQuery = new PhysicsQuery(m_scene);
	
	return true;
}

bool PhysicsSystem::Release()
{
	SafeDelete(m_physicsQuery);

	SafeDelete(m_physicsSimulationEventCallback);

	SafeDelete(m_physicsFilterShaderCallback);

	SafeDelete(m_physicsLayerManager);

	PxRelease(m_controllerManager);

	PxRelease(m_scene);

	PxRelease(m_cooking);

	PxRelease(m_cpuDispatcher);
	
	PxRelease(m_physics);

	if (m_pvd && m_pvd->isConnected())
		m_pvd->disconnect();
	if (m_pvdTransport && m_pvdTransport->isConnected())
		m_pvdTransport->disconnect();

	PxRelease(m_pvd);

	PxRelease(m_pvdTransport);

	PxRelease(m_foundation);

	SafeDelete(m_errorCallback);

	SafeDelete(m_allocater);

	return true;
}

void PhysicsSystem::Simulate(unsigned int subStep, const map<uint, vector<Component*>>& executionBuffer)
{
	if (!m_scene)
		return;

	for (auto& physicsObj : m_physicsObjects)
	{
		if (!physicsObj->IsWorking())
			continue;

		physicsObj->BeforePhysicsSimulationOnce();
	}

	subStep = subStep < m_subStepLimit ? subStep : m_subStepLimit;

	for (unsigned int i = 0; i < subStep; ++i)
	{
		SimulateOnce(executionBuffer);
	}

	for (auto& physicsObj : m_physicsObjects)
	{
		if (!physicsObj->IsWorking())
			continue;

		physicsObj->AfterPhysicsSimulationOnce();
	}
}

void PhysicsSystem::SimulateOnce(const map<uint, vector<Component*>>& executionBuffer)
{
	if (!m_scene)
		return;

	for (auto& physicsObj : m_physicsObjects)
	{
		if (!physicsObj->IsWorking())
			continue;

		physicsObj->BeforePhysicsSimulation();
	}

	//m_controllerManager->computeInteractions(m_system->time->fixedDeltaTime);

	m_scene->simulate(m_system->time->fixedDeltaTime);
	m_scene->fetchResults(true);
	
	for (auto& physicsObj : m_physicsObjects)
	{
		if (!physicsObj->IsWorking())
			continue;

		physicsObj->AfterPhysicsSimulation();
	}

	for (auto& coms : executionBuffer)
	{
		for (auto& com : coms.second)
		{
			if (!com->active)
				continue;

			IComponent* iCom = com;
			iCom->FixedUpdate();
		}
	}

	for (auto& coms : executionBuffer)
	{
		for (auto& com : coms.second)
		{
			if (!com->active)
				continue;

			IComponent* iCom = com;
			iCom->PostFixedUpdate();
		}
	}

	m_physicsSimulationEventCallback->ExecuteNotify();
	m_physicsSimulationEventCallback->ClearBuffers();
}

void PhysicsSystem::RegistPhysicsObject(IPhysicsObject* physicsObject)
{
	auto find_it = std::find(m_physicsObjects.begin(), m_physicsObjects.end(), physicsObject);
	if (find_it != m_physicsObjects.end())
		return;

	m_physicsObjects.push_back(physicsObject);
}

void PhysicsSystem::UnregistPhysicsObject(IPhysicsObject* physicsObject)
{
	auto find_it = std::find(m_physicsObjects.begin(), m_physicsObjects.end(), physicsObject);
	if (find_it == m_physicsObjects.end())
		return;

	m_physicsObjects.erase(find_it);
}

bool PhysicsSystem::SetupScene()
{
	if (m_scene)
		return false;

	PxSceneDesc desc(m_physics->getTolerancesScale());

	desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	desc.cpuDispatcher = m_cpuDispatcher;
	desc.filterShader = PhysicsFilterShaderCallback::PxSimulationFilterShader;
	desc.filterCallback = m_physicsFilterShaderCallback;
	desc.simulationEventCallback = m_physicsSimulationEventCallback;
	
	desc.flags |=
		PxSceneFlag::eMUTABLE_FLAGS |
		PxSceneFlag::eENABLE_CCD |
		PxSceneFlag::eADAPTIVE_FORCE |
		PxSceneFlag::eENABLE_PCM |
		PxSceneFlag::eENABLE_FRICTION_EVERY_ITERATION;

	desc.kineKineFilteringMode = PxPairFilteringMode::eDEFAULT;
	desc.staticKineFilteringMode = PxPairFilteringMode::eDEFAULT;

	m_scene = m_physics->createScene(desc);

	return m_scene != nullptr;
}
