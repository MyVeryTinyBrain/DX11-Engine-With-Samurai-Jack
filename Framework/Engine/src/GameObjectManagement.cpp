#include "EnginePCH.h"
#include "GameObjectManagement.h"
#include "GameObject.h"
#include "Transform.h"

GameObjectManagement::GameObjectManagement(Scene* scene)
{
	m_scene = scene;
}

GameObjectManagement::~GameObjectManagement()
{
	DeleteAllGameObjects(false);
}

GameObject * GameObjectManagement::CreateGameObject(const tstring & name, const tstring & tag)
{
	GameObject* gameObject = new GameObject(m_scene, name, tag);

	// ������ ���ӿ�����Ʈ�� ����Ʈ�� �߰��մϴ�.
	RegistGameObject(gameObject);

	return gameObject;
}

GameObject * GameObjectManagement::CreateGameObjectToChild(Transform * parent, const tstring & name, const tstring & tag)
{
	GameObject* gameObject = new GameObject(m_scene, name, tag);

	gameObject->transform->parent = parent;
	gameObject->transform->localPosition = V3::zero();
	gameObject->transform->localRotation = Q::identity();
	gameObject->transform->localScale = V3::one();

	// ������ ���ӿ�����Ʈ�� ����Ʈ�� �߰��մϴ�.
	RegistGameObject(gameObject);

	return gameObject;
}

GameObject * GameObjectManagement::FindGameObject(const tstring & name) const
{
	for (auto& pair : m_tagMap)
	{
		auto& list = pair.second;

		for (auto& gameObject : list)
		{
			if (gameObject->IsShouldDestroy())
				continue;

			if (gameObject->name == name)
				return gameObject;
		}
	}

	return nullptr;
}

std::vector<GameObject*> GameObjectManagement::FindGameObjects(const tstring & name) const
{
	std::vector<GameObject*> v;

	for (auto& pair : m_tagMap)
	{
		auto& list = pair.second;

		for (auto& gameObject : list)
		{
			if (gameObject->IsShouldDestroy())
				continue;

			if (gameObject->name == name)
				v.push_back(gameObject);
		}
	}

	return v;
}

GameObject * GameObjectManagement::FindGameObjectWithTag(const tstring & tag) const
{
	auto it = m_tagMap.find(tag);

	if (it == m_tagMap.end())
		return nullptr;

	auto& list = it->second;

	for (auto& gameObject : list)
	{
		if (gameObject->IsShouldDestroy())
			continue;

		if (gameObject->tag == tag)
			return gameObject;
	}

	return nullptr;
}

std::vector<GameObject*> GameObjectManagement::FindGameObjectsWithTag(const tstring & tag) const
{
	std::vector<GameObject*> v;

	auto it = m_tagMap.find(tag);

	if (it == m_tagMap.end())
		return v;

	auto& list = it->second;

	for (auto& gameObject : list)
	{
		if (gameObject->IsShouldDestroy())
			continue;

		if (gameObject->tag == tag)
			v.push_back(gameObject);
	}

	return v;
}

void GameObjectManagement::RegistGameObject(GameObject* gameObject)
{
	m_tagMap[gameObject->tag].push_back(gameObject);
}

bool GameObjectManagement::UnregistGameObject(GameObject* gameObject, const tstring & tag)
{
	auto it_map = m_tagMap.find(tag);
	if (it_map == m_tagMap.end())
		return false;

	GameObjects& gameObjectList = it_map->second;

	auto it_list = find(gameObjectList.begin(), gameObjectList.end(), gameObject);
	if (it_list == gameObjectList.end())
		return false;

	gameObjectList.erase(it_list);

	if (gameObjectList.empty())
	{
		// ���� Ű �����ȿ� ���ӿ�����Ʈ�� �������� �ʰ� �Ǹ�
		// ���� Ű ������ ����ϴ�.
		m_tagMap.erase(it_map);
	}

	return true;
}

void GameObjectManagement::ToDestroyedGameObject(GameObject* gameObject)
{
	auto find_it = std::find(m_destroyedGameObjects.begin(), m_destroyedGameObjects.end(), gameObject);
	if (find_it != m_destroyedGameObjects.end())
		return;

	m_destroyedGameObjects.push_back(gameObject);
}

bool GameObjectManagement::ReplaceGameObject(GameObject* gameObject, const tstring& beforeTag)
{
	if (!UnregistGameObject(gameObject, beforeTag))
		return false;

	RegistGameObject(gameObject);

	return true;
}

void GameObjectManagement::DeleteAllGameObjects(bool deleteAll)
{	
	// ������ ���ӿ�����Ʈ���� ��� ���Դϴ�.
	TagMap dontDestroyGameObjects;

	// ���� ������ ������������� ���ӿ�����Ʈ�� �����մϴ�.
	while (!m_tagMap.empty())
	{
		TagMap copiedTagMap = m_tagMap;
		m_tagMap.clear();

		for (auto& pair : copiedTagMap)
		{
			auto& list = pair.second;

			for (auto& gameObject : list)
			{
				if (!deleteAll && gameObject->dontDestroyOnLoad)
				{
					dontDestroyGameObjects[gameObject->tag].push_back(gameObject);
					continue;
				}

				gameObject->Destroy();
			}
		}
	}

	// ������ ���ӿ�����Ʈ���� ���ܵӴϴ�.
	m_tagMap = dontDestroyGameObjects;
}

void GameObjectManagement::MoveGameObjects(GameObjectManagement * destManagement)
{
	for (auto& pair : m_tagMap)
	{
		auto& list = pair.second;

		for (auto& gameObject : list)
		{
			destManagement->m_tagMap[gameObject->tag].push_back(gameObject);
			IGameObject* iGameObject = gameObject;
			iGameObject->OnSceneChanged();
		}
		list.clear();
	}
	m_tagMap.clear();
}

void GameObjectManagement::DeleteDestroyedGameObjects()
{	
	if (m_destroyedGameObjects.empty())
		return;

	for (auto& gameObject : m_destroyedGameObjects)
	{
		UnregistGameObject(gameObject, gameObject->tag);

		SafeDelete(gameObject);
	}

	m_destroyedGameObjects.clear();
}
