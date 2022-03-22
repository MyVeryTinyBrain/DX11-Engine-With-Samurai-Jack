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

	// Ʈ�������� �����ϰ� ������ �� ó���� ��ġ��ŵ�ϴ�.
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

			// �� ���ӿ�����Ʈ�� ������ �ش� ������Ʈ�� ����Ʈ���� �����մϴ�.
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

	// ���ӿ�����Ʈ �Ŵ����� ���� �� ���ӿ�����Ʈ�� �±� ������ �����մϴ�.
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
	�� ���ӿ�����Ʈ�� ������ ������Ʈ�鿡��
	�޼����� �����ϴ�.
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
	�� ���ӿ�����Ʈ�� ������ ������Ʈ���� �����Ͽ� 
	��� �ڽĿ� ������ ������Ʈ�鿡��
	�޼����� �����ϴ�.
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
	// ���ӿ�����Ʈ�� �ı��Ǹ� ���ӿ�����Ʈ �Ŵ����� �ı��� ���ӿ�����Ʈ ����Ʈ�� �߰��մϴ�.
	IScene* iScene = m_regionScene;
	iScene->ToDestroyedGameObject(this);

	// ��� ������Ʈ���� �����մϴ�.
	DeleteAllComponents();
}

void GameObject::DeleteAllComponents()
{
	/*
	��� ������Ʈ���� Destroy �Լ��� ȣ���մϴ�.
	���� ��� ������Ʈ���� ���ӿ�����Ʈ�� ������Ʈ ����Ʈ���� �����մϴ�.
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
