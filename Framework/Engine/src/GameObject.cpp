#include "EnginePCH.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

#define TRANSFORM_INDEX			(0)
#define TRANSFORM_IN_COMPONENTS	(Transform*)(m_components[TRANSFORM_INDEX])	

GameObject::GameObject(Scene* regionSceneBase, const tstring& name, const tstring& tag) : SceneObject(name)
{
	m_regionScene = regionSceneBase;
	m_tag = tag;

	// 트랜스폼을 설정하고 벡터의 맨 처음에 위치시킵니다.
	Transform* transform = new Transform();
	IComponent* iCom = transform;
	iCom->Initialize(this);
	m_components.push_back(transform);
	m_transformCache = transform;
}

GameObject::~GameObject()
{
	DeleteAllComponents();

	m_components.clear();
}

bool GameObject::RemoveComponent(Component* component)
{
	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		if (component == *it)
		{
			component->Destroy();

			// 이 게임오브젝트에 부착된 해당 컴포넌트를 리스트에서 제거합니다.
			m_components.erase(it);

			return true;
		}
	}
	return false;
}

bool GameObject::IsDontDestroyOnLoad() const
{
	return m_dontDestroyOnLoad;
}

void GameObject::SetDontDestroyOnLoad(bool value)
{
	m_dontDestroyOnLoad = value;
}

bool GameObject::IsActiveSelf() const
{
	return transform->activeSelf;
}

void GameObject::SetActiveSelf(bool value)
{
	transform->activeSelf = value;
}

bool GameObject::IsActiveInHierarchy() const
{
	return transform->activeInHierarchy;
}

uint GameObject::GetComponentCount() const
{
	return uint(m_components.size());
}

bool GameObject::IsActive() const
{
	return transform->active;
}

const tstring & GameObject::GetTag() const
{
	return m_tag;
}

void GameObject::SetTag(const tstring& tag)
{
	tstring beforeTag = m_tag;
	m_tag = tag;

	// 게임오브젝트 매니저에 속한 이 게임오브젝트의 태그 공간을 변경합니다.
	IScene* iScene = m_regionScene;
	iScene->ReplaceGameObject(this, beforeTag);
}

System* GameObject::GetSystem() const
{
	return m_regionScene->GetSystem();
}

Scene * GameObject::GetRegionScene() const
{
	return m_regionScene;
}

void GameObject::SetRegionScene(Scene * scene)
{
	if (!scene)
		return;

	m_regionScene = scene;
}

void GameObject::BroadcastMessage(const tstring& message, void* args)
{
	/*
	이 게임오브젝트에 부착된 컴포넌트들에게
	메세지를 보냅니다.
	*/

	for (auto& component : m_components)
	{
		if (!component->active)
			continue;

		IComponent* iComponent = component;

		iComponent->OnMessage(message, args);
	}
}

void GameObject::BroadcastMessageInChilds(const tstring& message, void* args)
{
	/*
	이 게임오브젝트에 부착된 컴포넌트들을 포함하여 
	모든 자식에 부착된 컴포넌트들에게
	메세지를 보냅니다.
	*/

	queue<Transform*> s;
	s.push(transform);

	while (!s.empty())
	{
		Transform* t = s.front(); s.pop();

		t->gameObject->BroadcastMessage(message, args);

		for (uint i = 0; i < t->childCount; ++i)
			s.push(t->GetChild(i));
	}
}

void GameObject::OnSceneChanged()
{
	for (auto& com : m_components)
	{
		IComponent* iCom = com;
		iCom->OnSceneChanged();
	}
}

void GameObject::OnDestroy()
{
	// 게임오브젝트가 파괴되면 게임오브젝트 매니저의 파괴된 게임오브젝트 리스트에 추가합니다.
	IScene* iScene = m_regionScene;
	iScene->ToDestroyedGameObject(this);

	// 모든 컴포넌트들을 제거합니다.
	DeleteAllComponents();
}

void GameObject::DeleteAllComponents()
{
	/*
	모든 컴포넌트들의 Destroy 함수를 호출합니다.
	이후 모든 컴포넌트들을 게임오브젝트의 컴포넌트 리스트에서 제거합니다.
	*/

	if (m_components.empty())
		return;

	Transform* transform = TRANSFORM_IN_COMPONENTS;

	while (m_components.size() > 1)
	{
		vector<Component*> copiedComponents;
		copiedComponents.reserve(m_components.size() - 1);
		copiedComponents.insert(copiedComponents.end(), m_components.begin() + 1, m_components.end());

		m_components.resize(1);
		m_components[TRANSFORM_INDEX] = transform;

		for (auto& com : copiedComponents)
		{
			com->Destroy();
		}
	}

	transform->Destroy();
}
