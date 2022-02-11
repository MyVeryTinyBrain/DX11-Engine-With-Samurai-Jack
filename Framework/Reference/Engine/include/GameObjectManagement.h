#pragma once

ENGINE_BEGIN
class Scene;
class GameObject;
class Component;
class Transform;
class GameObjectManagement final
{
	using GameObjects = list<GameObject*>;

	using TagMap = unordered_map<tstring, GameObjects>;

public:

	GameObjectManagement(Scene* scene);

	virtual ~GameObjectManagement();

public:

	GameObject* CreateGameObject(const tstring& name = TEXT(""), const tstring& tag = TEXT(""));

	GameObject* CreateGameObjectToChild(Transform* parent, const tstring& name = TEXT(""), const tstring& tag = TEXT(""));

	GameObject* FindGameObject(const tstring& name) const;

	std::vector<GameObject*> FindGameObjects(const tstring& name) const;

	GameObject* FindGameObjectWithTag(const tstring& tag) const;

	std::vector<GameObject*> FindGameObjectsWithTag(const tstring& tag) const;

private:

	void RegistGameObject(GameObject* gameObject);

	bool UnregistGameObject(GameObject* gameObject, const tstring& tag);

public:

	void ToDestroyedGameObject(GameObject* gameObject);

	bool ReplaceGameObject(GameObject* gameObject, const tstring& beforeTag);

	void DeleteAllGameObjects(bool deleteAll);

	void MoveGameObjects(GameObjectManagement* destManagement);

public:

	void DeleteDestroyedGameObjects();

private:

	Scene*				m_scene = nullptr;

	TagMap				m_tagMap;

	GameObjects			m_destroyedGameObjects;

};

ENGINE_END
