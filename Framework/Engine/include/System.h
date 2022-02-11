#pragma once

#include "ISystem.h"

ENGINE_BEGIN
class ENGINE_API System : public ISystem
{
public:

	System();

	virtual ~System();

private:

	bool Initialize(EngineWorldDesc* desc, HWND hWnd, unsigned int width, unsigned int height);

	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Release();

public:

	virtual Input* GetInput() const override;

	Times* GetTime() const;

	virtual SceneManagement* GetSceneManagement() const override;

	virtual PhysicsSystem* GetPhysicsSystem() const final override;

	virtual ResourceManagement* GetResourceManagement() const override;

	virtual GraphicSystem* GetGraphicSystem() const override;

	virtual ImGuiSystem* GetImGuiSystem() const override;

	_declspec(property(get = GetInput)) Input* input;
	_declspec(property(get = GetSceneManagement)) SceneManagement* sceneManagement;
	_declspec(property(get = GetPhysicsSystem)) PhysicsSystem* physicsSystem;
	_declspec(property(get = GetTime)) Times* time;
	_declspec(property(get = GetResourceManagement)) ResourceManagement* resourceManagement;
	_declspec(property(get = GetGraphicSystem)) GraphicSystem* graphicSystem;
	_declspec(property(get = GetImGuiSystem)) ImGuiSystem* imguiSystem;

private:

	virtual CentralTimeElement* GetTimeElement() const final override;

private:

	bool						m_initialized = false;

	Input*						m_input = nullptr;

	CentralTimeElement*			m_timeElement = nullptr;

	Times*						m_timeElementAdapter = nullptr;

	SceneManagement*			m_sceneManagement = nullptr;

	PhysicsSystem*				m_physicsSystem = nullptr;

	ResourceManagement*			m_resourceManagement = nullptr;

	GraphicSystem*				m_graphicSystem = nullptr;

	ImGuiSystem*				m_imguiSystem = nullptr;
};
ENGINE_END
