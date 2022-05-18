#pragma once
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"

template<class T>
inline T * GameObject::AddComponent(void* arg)
{
	if (IsShouldDestroy()) 
	{
		RETURN_NULL_ERROR_MESSAGE("GameObject::AddComponent<T>::You're adding onto destroyed game object");
	}

	T* component = new T();
	IComponent* iCom = component;
	iCom->Initialize(this, arg);
	m_components.push_back(component);
	return component;
}

template<class T>
inline bool GameObject::RemoveComponent()
{
	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		Component* component = *it;
		T* cast = dynamic_cast<T*>(component);
		if (cast)
		{
			component->Destroy();

			// �� ���ӿ�����Ʈ�� ������ �ش� ������Ʈ�� ����Ʈ���� �����մϴ�.
			m_components.erase(it);

			return true;
		}
	}
	return false;
}

template<class T>
inline T * GameObject::GetComponent() const
{
	for (auto& component : m_components)
	{
		T* cast = dynamic_cast<T*>(component);
		if (cast)
			return cast;
	}
	return nullptr;
}

template<class T>
inline vector<T*> GameObject::GetComponents() const
{
	std::vector<T*> lightBlend;
	for (auto& component : m_components)
	{
		T* cast = dynamic_cast<T*>(component);
		if (cast)
			lightBlend.push_back(cast);
	}
	return lightBlend;
}

template<class T>
inline T * GameObject::GetComponentInChilds() const
{
	std::stack<Transform*> s;
	s.push(transform);

	while (!s.empty())
	{
		Transform* t = s.top(); s.pop();
		T* component = t->gameObject->GetComponent<T>();
		if (component)
			return component;

		for (uint i = 0; i < t->childCount; ++i)
			s.push(t->GetChild(i));
	}
	return nullptr;
}

template<class T>
inline vector<T*> GameObject::GetComponentsInChilds() const
{
	std::stack<Transform*> s;
	s.push(transform);

	std::vector<T*> components;
	while (!s.empty())
	{
		Transform* t = s.top(); s.pop();
		T* component = t->gameObject->GetComponent<T>();
		if (component)
			components.push_back(component);

		for (uint i = 0; i < t->childCount; ++i)
			s.push(t->GetChild(i));
	}
	return components;
}