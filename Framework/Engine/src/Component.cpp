#include "EnginePCH.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"

Transform* Component::GetTransform() const
{
	return m_gameObject->transform;
}

void Component::SetEnable(bool value)
{
	if (m_enable == value)
		return;

	// 활성화 -> 비활성화 전환은 제약을 걸지 않습니다.
	// 비활성화 -> 활성화 전환은 오브젝트가 파괴될 예정이 아닐 때만 가능합니다.
	if (value && IsShouldDestroy())
		return;

	m_enable = value;

	ApplyEnableChange();
}

bool Component::IsActive() const
{
	return m_enable && transform->activeInHierarchy;
}

int Component::GetExecutionOrder() const
{
	return m_executionOrder;
}

void Component::SetExecutionOrder(int order)
{
	m_executionOrder = order;
}

System* Component::GetSystem() const
{
	return m_gameObject->GetSystem();
}

GameObject* Component::CreateGameObject(const tstring& name, const tstring& tag)
{
	return gameObject->regionScene->CreateGameObject(name, tag);
}

GameObject* Component::CreateGameObjectToChild(Transform* parent, const tstring& name, const tstring& tag)
{
	return gameObject->regionScene->CreateGameObjectToChild(parent, name, tag);
}

GameObject* Component::FindGameObject(const tstring& name) const
{
	return gameObject->regionScene->FindGameObject(name);
}

std::vector<GameObject*> Component::FindGameObjects(const tstring& name) const
{
	return gameObject->regionScene->FindGameObjects(name);
}

GameObject* Component::FindGameObjectWithTag(const tstring& tag) const
{
	return gameObject->regionScene->FindGameObjectWithTag(tag);
}

std::vector<GameObject*> Component::FindGameObjectsWithTag(const tstring& tag) const
{
	return gameObject->regionScene->FindGameObjectsWithTag(tag);
}

void Component::Initialize(GameObject* gameObject)
{
	m_gameObject = gameObject;

	// 컴포넌트 매니저에 이 이 컴포넌트를 추가합니다.
	IScene* iScene = gameObject->regionScene;
	iScene->RegistComponent(this);

	Awake();
	OnEnable();
}

void Component::CallStartOnce()
{
	if (!m_callStart)
		return;

	m_callStart = false;

	Start();
}

void Component::ApplyEnableChange()
{
	bool active = IsActive();

	if (active == m_beforeActive)
		return;

	if (active)
		OnEnable();
	else
		OnDisable();

	m_beforeActive = active;
}

void Component::OnDestroy()
{
	// 파괴되면 컴포넌트 매니저의 파괴된 컴포넌트 리스트에 추가합니다.
	IScene* iScene = m_gameObject->regionScene;
	iScene->ToDestoryedComponent(this);

	// 게임오브젝트에서 탈착합니다.
	m_gameObject->RemoveComponent(this);

	// 오브젝트를 비활성화 상태로 전환합니다.
	// 이에 따라서 OnDisable() 함수가 호출될 수 있습니다.
	SetEnable(false);

	// 파괴를 알리는 함수를 호출합니다.
	OnDestroyed();
}

