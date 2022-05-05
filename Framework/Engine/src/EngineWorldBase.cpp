#include "EnginePCH.h"
#include "EngineWorldBase.h"

#include "System.h"
#include "CentralTimeElement.h"
#include "SceneManagement.h"
#include "PhysicsSystem.h"
#include "Input.h"
#include "GraphicSystem.h"
#include "ImGuiSystem.h"

#include "Scene.h"
#include "Component.h"
#include "ComponentManagement.h"

#include <WinUser.h>
#include <time.h>

#include "ThreadPool.h"

EngineWorldBase::EngineWorldBase()
{
}

EngineWorldBase::~EngineWorldBase()
{
	Release();
}

System* EngineWorldBase::GetSystem() const
{
	return m_system;
}

bool EngineWorldBase::Initialize(EngineWorldDesc* desc)
{
	if (desc->makeWindowDesc)
	{
		m_hInstance = desc->makeWindowDesc->hInstance;
		m_hWnd = MakeWindow(desc->makeWindowDesc);

		if (!m_hWnd)
			RETURN_FALSE_ERROR_MESSAGE("EngineWorldBase::Initialize::MakeWindow");
	}
	else if (desc->alreadedWindowDesc)
	{
		m_hInstance = GetHInstance(desc->alreadedWindowDesc);

		if (!m_hInstance)
			RETURN_FALSE_ERROR_MESSAGE("EngineWorldBase::Initialize::alreadedWindowDesc is nullptr");

		m_hWnd = desc->alreadedWindowDesc->hWnd;
	}
	else return false;

	if (!m_hWnd)
		RETURN_FALSE_ERROR_MESSAGE("EngineWorldBase::Initialize::m_hWnd is nullptr");

	unsigned int winCX, winCY;
	{
		RECT rect = {};
		GetClientRect(m_hWnd, &rect);
		winCX = rect.right - rect.left;
		winCY = rect.bottom - rect.top;
	}

	m_system = new System;
	ISystem* iSystem = m_system;
	if (!iSystem->Initialize(desc, m_hWnd, winCX, winCY))
		RETURN_FALSE_ERROR_MESSAGE("EngineWorldBase::System::Initialize");

	srand(unsigned int(time(NULL)));

	m_threadPool = new ThreadPool;

	return true;
}

bool EngineWorldBase::Step()
{
	if (!m_hInstance || !m_hWnd)
		return false;

	if (!IsWindow(m_hWnd))
		return false;

	if (!m_system)
		return false;

	ISystem* iSystem = m_system;
	ICentralTimeElement* iCentralTimeElement = iSystem->GetTimeElement();

	unsigned int update = iCentralTimeElement->UpdateAccumulate();
	unsigned int fixedUpdate = iCentralTimeElement->FixedUpdateAccumulate();
	unsigned int step = update + fixedUpdate;

	if (step)
	{
		iCentralTimeElement->ProgramAccumulate();

		ISceneManagement* iSceneManagement = m_system->sceneManagement;
		bool changedScene = iSceneManagement->TryChangeScene();
		if (changedScene)
			iCentralTimeElement->Reset();

		Scene* scene = m_system->sceneManagement->currentScene;
		IScene* iScene = scene;

		if (!scene)
			return true;

		ComponentManagement* componentManagement = iScene->GetComponentManagement();

		// 생성된 컴포넌트를 이동시키고 Start 함수를 호출합니다.
		componentManagement->ReadyComponents();

		const ComponentManagement::ComponentsByExecutionOrder& componentsByExecutionOrder = componentManagement->GetComponentsByExecutionOrders();

		// 고정 업데이트를 진행합니다.
		if (fixedUpdate)
		{
			IPhysicsSystem* iPhysicsSystem = m_system->physics;
			iPhysicsSystem->Simulate(fixedUpdate, componentsByExecutionOrder);

			iScene->DeleteDestroyedObjects();
		}

		// 업데이트를 진행합니다.
		if (update)
		{
			IImGuiSystem* iImGuiSystem = m_system->imguiSystem;
			iImGuiSystem->BeginRender();

			IInput* iInput = m_system->input;
			iInput->SetEnable(!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow));
			iInput->Fetch();

			for (auto& coms : componentsByExecutionOrder)
			{
				for (auto& com : coms.second)
				{
					if (!com->active)
						continue;

					IComponent* iCom = com;
					iCom->PreUpdate();
				}
			}

			for (auto& coms : componentsByExecutionOrder)
			{
				for (auto& com : coms.second)
				{
					if (!com->active)
						continue;

					IComponent* iCom = com;
					iCom->Update();
				}
			}

			iScene->OnUpdate();

			for (auto& coms : componentsByExecutionOrder)
			{
				for (auto& com : coms.second)
				{
					if (!com->active)
						continue;

					IComponent* iCom = com;
					m_threadPool->AddJobFunction([=]() { iCom->AnimationUpdate(); });
				}
			}
			m_threadPool->Join();

			for (auto& coms : componentsByExecutionOrder)
			{
				for (auto& com : coms.second)
				{
					if (!com->active)
						continue;

					IComponent* iCom = com;
					iCom->LateUpdate();
				}
			}

			iScene->OnLateUpdate();

			for (auto& coms : componentsByExecutionOrder)
			{
				for (auto& com : coms.second)
				{
					if (!com->active)
						continue;

					IComponent* iCom = com;
					iCom->PostUpdate();
				}
			}

			for (auto& coms : componentsByExecutionOrder)
			{
				for (auto& com : coms.second)
				{
					if (!com->active)
						continue;

					IComponent* iCom = com;
					iCom->Render();
				}
			}

			iInput->End();

			IGraphicSystem* iGraphicSystem = m_system->graphic;
			iGraphicSystem->Render();

			iImGuiSystem->EndRender();

			iGraphicSystem->Present();
		}
	}

	return true;
}


bool EngineWorldBase::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_hWnd)
		return true;

	if (!m_system)
		return false;

	ISystem* iSystem = m_system;
	iSystem->WndProc(hWnd, message, wParam, lParam);

	return true;
}

void EngineWorldBase::Release()
{
	SafeDelete(m_threadPool);

	if (m_system)
	{
		ISystem* iSystem = m_system;
		iSystem->Release();
	}
	SafeDelete(m_system);
}

HWND EngineWorldBase::MakeWindow(MakeWindowDesc* desc) const
{
	if (!desc->hInstance)
		return NULL;
	if (!desc->WndProc)
		return NULL;
	if (!desc->Title)
		return NULL;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)desc->WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = desc->hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = FALSE;
	wc.lpszClassName = desc->Title;

	if (!RegisterClass(&wc))
		return NULL;

	RECT size = { 0, 0, LONG(desc->WinCX), LONG(desc->WinCY) };
	AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hWnd = CreateWindow(
		desc->Title,
		desc->Title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		size.right - size.left, size.bottom - size.top,
		NULL, NULL,
		desc->hInstance,
		NULL);

	if (!hWnd)
		return NULL;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}

HINSTANCE EngineWorldBase::GetHInstance(AlreadedWindowDesc* desc) const
{
	if (!desc->hWnd)
		return NULL;

#ifndef _WIN64
	unsigned int index = GWL_HINSTANCE;
#else
	unsigned int index = GWLP_HINSTANCE;
#endif

	return (HINSTANCE)(UINT_PTR)GetWindowLong(desc->hWnd, index);
}
