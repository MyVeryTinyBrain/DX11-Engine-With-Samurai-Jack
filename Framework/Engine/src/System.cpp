#include "EnginePCH.h"
#include "EngineWorldDesc.h"
#include "System.h"
#include "Input.h"
#include "CentralTimeElement.h"
#include "Times.h"
#include "SceneManagement.h"
#include "PhysicsSystem.h"
#include "ResourceManagement.h"
#include "GraphicSystem.h"
#include "ImGuiSystem.h"
#include "SoundSystem.h"

System::System()
{
}

System::~System()
{
}

bool System::Initialize(EngineWorldDesc* desc, HWND hWnd, unsigned int width, unsigned int height)
{
	if (!desc)
		return false;

	m_graphicSystem = new GraphicSystem;
	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	if (!iGraphicSystem->Initialize(hWnd, width, height, unsigned int(desc->updateDesc.UpdateFPS), desc->updateDesc.vsync, desc->windowCreateDesc.fullScreen))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::IGraphicSystem::Initialize");

	m_input = new Input;
	IInput* iInput = m_input;
	if (!iInput->Initialize(this))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::InputBase::Initialize");

	m_timeElement = new CentralTimeElement;
	ICentralTimeElement* iCentralTimeElement = m_timeElement;
	if (!iCentralTimeElement->Initialize(desc->updateDesc.UpdateFPS, desc->updateDesc.FixedUpdateFPS))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::CentralTimeElement::Initialize");

	m_timeElementAdapter = new Times(m_timeElement);

	m_sceneManagement = new SceneManagement;
	ISceneManagement* iSceneManagement = m_sceneManagement;
	if (!iSceneManagement->Initialize(this))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::SceneManagement::Initialize");

	m_physicsSystem = new PhysicsSystem;
	IPhysicsSystem* iPhysicsSystem = m_physicsSystem;
	if (!iPhysicsSystem->Initialize(this, desc->updateDesc.PhysSubStepLimit))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::PhysicsSimulator::Initialize");

	m_resourceManagement = new ResourceManagement;
	IResourceManagement* iResourceManagement = m_resourceManagement;
	if (!iResourceManagement->Initialize(this))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::ResourceManagement::Initialize");

	m_imguiSystem = new ImGuiSystem;
	IImGuiSystem* iImGuiSystem = m_imguiSystem;
	if(!iImGuiSystem->Initialize(this, m_graphicSystem))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::ImGuiSystem::Initialize");

	m_soundSystem = new SoundSystem;
	ISoundSystem* iSoundSystem = m_soundSystem;
	if(!iSoundSystem->Initialize(desc->soundDesc.NumChannels))
		RETURN_FALSE_ERROR_MESSAGE("System::Initialize::SoundSystem::Initialize");

	m_initialized = true;

	return true;
}

void System::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_initialized)
		return;

	IGraphicSystem* iGraphicSystem = m_graphicSystem;
	iGraphicSystem->WndProc(hWnd, message, wParam, lParam);

	IInput* iInput = m_input;
	iInput->WndProc(hWnd, message, wParam, lParam);

	IImGuiSystem* iImGuiSystem = m_imguiSystem;
	iImGuiSystem->WndProc(hWnd, message, wParam, lParam);
}

void System::Release()
{
	SafeDelete(m_imguiSystem);
	SafeDelete(m_sceneManagement);
	SafeDelete(m_timeElementAdapter);
	SafeDelete(m_timeElement);
	SafeDelete(m_input);
	SafeDelete(m_physicsSystem);
	SafeDelete(m_resourceManagement);
	SafeDelete(m_soundSystem);
	SafeDelete(m_graphicSystem);
}

Input* System::GetInput() const
{
	return m_input;
}

CentralTimeElement* System::GetTimeElement() const
{
	return m_timeElement;
}

Times* System::GetTime() const
{
	return m_timeElementAdapter;
}

SceneManagement* System::GetSceneManagement() const
{
	return m_sceneManagement;
}

PhysicsSystem* System::GetPhysicsSystem() const
{
	return m_physicsSystem;
}

ResourceManagement* System::GetResourceManagement() const
{
	return m_resourceManagement;
}

GraphicSystem* System::GetGraphicSystem() const
{
	return m_graphicSystem;
}

ImGuiSystem* System::GetImGuiSystem() const
{
	return m_imguiSystem;
}

SoundSystem* System::GetSoundSystem() const
{
	return m_soundSystem;
}
