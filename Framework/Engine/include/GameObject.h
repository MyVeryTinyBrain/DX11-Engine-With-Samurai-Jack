#pragma once

#include "IGameObject.h"
#include "SceneObject.h"

ENGINE_BEGIN
class ENGINE_API GameObject final : public SceneObject, public IGameObject
{
public:

	GameObject(Scene* regionSceneBase, const tstring& name = TEXT(""), const tstring& tag = TEXT(""));

	virtual ~GameObject();

public:

	bool RemoveComponent(Component* component);

public:

	Transform* GetTransform() const { return m_transformCache; }

	bool IsActiveSelf() const;

	void SetActiveSelf(bool value);

	bool IsActiveInHierarchy() const;

	bool IsActive() const;

	const tstring& GetTag() const;

	void SetTag(const tstring& tag);

	Scene* GetRegionScene() const;

	void SetRegionScene(Scene* scene);

	bool IsDontDestroyOnLoad() const;

	void SetDontDestroyOnLoad(bool value);

	System* GetSystem() const;

	_declspec(property(get = GetTransform)) Transform* transform;
	_declspec(property(get = IsActiveSelf, put = SetActiveSelf)) bool activeSelf;
	_declspec(property(get = IsActiveInHierarchy)) bool activeInHierarchy;
	_declspec(property(get = IsActive)) bool active;
	_declspec(property(get = GetTag, put = SetTag)) tstring& tag;
	_declspec(property(get = GetRegionScene, put = SetRegionScene)) Scene* regionScene;
	_declspec(property(get = IsDontDestroyOnLoad, put = SetDontDestroyOnLoad)) bool dontDestroyOnLoad;
	_declspec(property(get = GetSystem)) System* system;

public:

	void BroadcastMessage(const tstring& message, void* args = nullptr);

	void BroadcastMessageInChilds(const tstring& message, void* args = nullptr);

public:

	template <class T>
	T* AddComponent();

	template <class T>
	bool RemoveComponent();

	template <class T>
	T* GetComponent() const;

	template <class T>
	vector<T*> GetComponents() const;

	template <class T>
	T* GetComponentInChilds() const;

	template <class T>
	vector<T*> GetComponentsInChilds() const;

	size_t GetComponentCount() const;

	inline Component* GetComponent(size_t index) { return index < m_components.size() ? m_components[index] : nullptr; }

private:

	virtual void OnSceneChanged() final override;

	virtual void OnDestroy() final override;

	void DeleteAllComponents();

private:

	Scene*					m_regionScene = nullptr;

	tstring					m_tag = TEXT("");

	vector<Component*>		m_components;

	Transform*				m_transformCache = nullptr;

	bool					m_dontDestroyOnLoad = false;
};
ENGINE_END

#include "GameObject.hpp"
