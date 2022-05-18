#include "EnginePCH.h"
#include "SceneManagement.h"
#include "Scene.h"

SceneManagement::SceneManagement()
{
}

SceneManagement::~SceneManagement()
{
	IScene* currentIScene = m_current;
	IScene* nextIScene = m_next;

	if (currentIScene)
	{
		currentIScene->OnUnload();
		currentIScene->DeleteAllObjects(true);
	}

	if (nextIScene)
	{
		nextIScene->OnUnload();
		nextIScene->DeleteAllObjects(true);
	}

	SafeDelete(m_next);
	SafeDelete(m_current);
}

bool SceneManagement::Initialize(System* system)
{
	if (!system)
		return false;

	m_system = system;

    return true;
}

bool SceneManagement::TryChangeScene()
{
	if (!m_next)
		return false;

	Scene* before = m_current;
	Scene* next = m_next;
	IScene* iBefore = m_current;
	IScene* iNext = m_next;

	m_current = next;
	m_next = nullptr;

	if (before)
	{
		iBefore->MoveGameObjects(next);
		iBefore->OnUnload();
		SafeDelete(before);
	}

	iNext->OnLoad();

    return true;
}

void SceneManagement::ChangeScene(Scene* scene)
{
	// 다음 씬이 nullptr이라면 종료합니다.
	if (!scene)
		return;

	// 현재 씬 인스턴스가 변경하려는 씬 인스턴스와 같다면 종료합니다.
	if (scene == m_current)
		return;

	// 다음 씬을 재설정합니다.
	SafeDelete(m_next);
	m_next = scene;

	IScene* iNextScene = m_next;
	bool result = iNextScene->Initialize(m_system);
	if (!result)
		ERROR_MESSAGE("SceneManagement::ChangeScene::Failed to initialize next scene");

	// 현재 씬이 null이라면 씬 전환을 즉시 실행합니다.
	if (!m_current)
		TryChangeScene();
}

void SceneManagement::ReloadScene()
{
	if (!m_current)
		return;

	Scene* clone = m_current->Clone();
	if (!clone)
		ERROR_MESSAGE("SceneManagement::ReloadScene::Clone is nullptr");

	ChangeScene(clone);
}

Scene* SceneManagement::GetCurrentScene() const
{
    return m_current;
}
