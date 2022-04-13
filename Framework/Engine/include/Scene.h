#pragma once

#include "Object.h"
#include "IScene.h"

ENGINE_BEGIN
class ENGINE_API Scene abstract : public Object, public IScene
{
public:

	Scene();

	Scene(const tstring& name);

	virtual ~Scene();

public:

	virtual Scene* Clone() = 0;

protected:

	virtual void OnLoad() override = 0;

	virtual void OnUnload() override = 0;

	virtual void OnUpdate() override = 0;

	virtual void OnLateUpdate() override = 0;

public:

	System* GetSystem() const;

	_declspec(property(get = GetSystem)) System* system;

public:

	GameObject* CreateGameObject(const tstring& name = TEXT(""), const tstring& tag = TEXT(""));

	GameObject* CreateGameObjectToChild(Transform* parent, const tstring& name = TEXT(""), const tstring& tag = TEXT(""));

	GameObject* FindGameObject(const tstring& name) const;

	std::vector<GameObject*> FindGameObjects(const tstring& name) const;

	GameObject* FindGameObjectWithTag(const tstring& tag) const;

	std::vector<GameObject*> FindGameObjectsWithTag(const tstring& tag) const;

private:

	virtual void ToDestroyedGameObject(class GameObject* gameObject) override;

	virtual void ReplaceGameObject(class GameObject* gameObjectBase, const tstring& beforeTag) override;

	virtual void MoveGameObjects(IScene* iScene) override;

private:

	virtual void RegistComponent(Component* component) override;
	virtual void ReregistComponent(Component* component, uint beforeExecutionOrder) override;
	virtual void ToDestoryedComponent(Component* component) override;

	inline virtual ComponentManagement* GetComponentManagement() const override { return m_componentManagement; }

private:

	virtual bool Initialize(System* system) final override;

	virtual void DeleteDestroyedObjects() final override;

	virtual void DeleteAllObjects(bool deleteAll) override;

private:

	System*							m_system = nullptr;

	class GameObjectManagement*		m_gameObjectManagement = nullptr;

	class ComponentManagement*		m_componentManagement = nullptr;
};
ENGINE_END
