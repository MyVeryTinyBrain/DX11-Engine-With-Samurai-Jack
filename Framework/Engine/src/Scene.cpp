#include "EnginePCH.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObjectManagement.h"
#include "ComponentManagement.h"

Scene::Scene() : Object()
{
	m_gameObjectManagement = new GameObjectManagement(this);
	m_componentManagement = new ComponentManagement(this);
}

Scene::Scene(const tstring & name) : Object(name)
{
	m_gameObjectManagement = new GameObjectManagement(this);
	m_componentManagement = new ComponentManagement(this);
}

Scene::~Scene()
{
	SafeDelete(m_gameObjectManagement);
	SafeDelete(m_componentManagement);
}

System* Scene::GetSystem() const
{
	return m_system;
}

GameObject* Scene::CreateGameObject(const tstring& name, const tstring& tag)
{
	return m_gameObjectManagement->CreateGameObject(name, tag);
}

GameObject* Scene::CreateGameObjectToChild(Transform* parent, const tstring& name, const tstring& tag)
{
	return m_gameObjectManagement->CreateGameObjectToChild(parent, name, tag);
}

GameObject* Scene::FindGameObject(const tstring& name) const
{
	return m_gameObjectManagement->FindGameObject(name);
}

std::vector<GameObject*> Scene::FindGameObjects(const tstring& name) const
{
	return m_gameObjectManagement->FindGameObjects(name);
}

GameObject* Scene::FindGameObjectWithTag(const tstring& tag) const
{
	return m_gameObjectManagement->FindGameObjectWithTag(tag);
}

std::vector<GameObject*> Scene::FindGameObjectsWithTag(const tstring& tag) const
{
	return m_gameObjectManagement->FindGameObjectsWithTag(tag);
}

void Scene::ToDestroyedGameObject(GameObject* gameObject)
{
	m_gameObjectManagement->ToDestroyedGameObject(gameObject);
}

void Scene::ReplaceGameObject(GameObject* gameObjectBase, const tstring& beforeTag)
{
	m_gameObjectManagement->ReplaceGameObject(gameObjectBase, beforeTag);
}

void Scene::MoveGameObjects(IScene* iScene)
{
	Scene* scene = static_cast<Scene*>(iScene);
	m_gameObjectManagement->MoveGameObjects(scene->m_gameObjectManagement);
}

void Scene::RegistComponent(Component* component)
{
	m_componentManagement->RegistComponent(component);
}

void Scene::ToDestoryedComponent(Component* component)
{
	m_componentManagement->ToDestroyedComponent(component);
}

bool Scene::Initialize(System* system)
{
	if (!system)
		return false;

	m_system = system;

	return true;
}

void Scene::DeleteDestroyedObjects()
{
	m_gameObjectManagement->DeleteDestroyedGameObjects();
	m_componentManagement->DeleteDestroyedComponents();
}

void Scene::DeleteAllObjects(bool deleteAll)
{
	m_gameObjectManagement->DeleteAllGameObjects(deleteAll);
	m_componentManagement->DeleteAllComponents(deleteAll);

	DeleteDestroyedObjects();
}

