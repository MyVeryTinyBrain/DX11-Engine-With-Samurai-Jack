#pragma once

ENGINE_BEGIN
class Component;
class System;
class GameObject;
class Transform;
class ComponentManagement;
class ENGINE_API IScene abstract
{
public:

	virtual ~IScene() = default;

	virtual void OnLoad() = 0;

	virtual void OnUnload() = 0;

	virtual void OnUpdate() = 0;

	virtual void OnLateUpdate() = 0;

	virtual bool Initialize(System* system) = 0;

	// GameObject ==================================================================================

	virtual void ToDestroyedGameObject(GameObject* gameObject) = 0;

	virtual void ReplaceGameObject(GameObject* gameObjectBase, const tstring& beforeTag) = 0;

	virtual void MoveGameObjects(IScene* iScene) = 0;

	// Component ====================================================================================

	virtual void RegistComponent(Component* component) = 0;
	virtual void ReregistComponent(Component* component, uint beforeExecutionOrder) = 0;
	virtual void ToDestoryedComponent(Component* component) = 0;

	virtual ComponentManagement* GetComponentManagement() const = 0;

	// ==============================================================================================

	virtual void DeleteDestroyedObjects() = 0;

	virtual void DeleteAllObjects(bool deleteAll) = 0;
};

ENGINE_END
