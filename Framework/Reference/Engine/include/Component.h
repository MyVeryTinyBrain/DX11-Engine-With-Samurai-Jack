#pragma once

#include "SceneObject.h"
#include "IComponent.h"

ENGINE_BEGIN
class ENGINE_API Component abstract : public SceneObject, public IComponent
{
public:

	inline Component() : SceneObject() {};

protected:

	inline virtual ~Component() = default;

protected:

	virtual inline void Awake() override {}
	virtual inline void Awake(void* arg) override {}
	virtual inline void Start() override {}
	virtual inline void FixedUpdate() override {}
	virtual inline void PostFixedUpdate() override {}
	virtual inline void PreUpdate() override {}
	virtual inline void Update() override {}
	virtual inline void AnimationUpdate() override {}
	virtual inline void LateUpdate() override {}
	virtual inline void PostUpdate() override {};
	virtual inline void Render() override {}
	virtual inline void OnEnable() override {}
	virtual inline void OnDisable() override {}
	virtual inline void OnDestroyed() override {}
	virtual inline void OnSceneChanged() override {};
	virtual inline void OnMessage(const tstring& message, void* args = nullptr) override {}

public:

	inline GameObject* GetGameObject() const { return m_gameObject; }

	Transform* GetTransform() const;

	inline bool IsEnabled() const { return m_enable; }

	void SetEnable(bool value);

	bool IsActive() const;

	int GetExecutionOrder() const;

	void SetExecutionOrder(int order);

	System* GetSystem() const;

	_declspec(property(get = GetGameObject)) GameObject* gameObject;
	_declspec(property(get = GetTransform)) Transform* transform;
	_declspec(property(get = IsEnabled, put = SetEnable)) bool enable;
	_declspec(property(get = IsActive)) bool active;
	_declspec(property(get = GetExecutionOrder, put = SetExecutionOrder)) int executionOrder;
	_declspec(property(get = GetSystem)) System* system;

protected:

	GameObject* CreateGameObject(const tstring& name = TEXT(""), const tstring& tag = TEXT(""));

	GameObject* CreateGameObjectToChild(Transform* parent, const tstring& name = TEXT(""), const tstring& tag = TEXT(""));

	GameObject* FindGameObject(const tstring& name) const;

	std::vector<GameObject*> FindGameObjects(const tstring& name) const;

	GameObject* FindGameObjectWithTag(const tstring& tag) const;

	std::vector<GameObject*> FindGameObjectsWithTag(const tstring& tag) const;

private:

	virtual void Initialize(GameObject* gameObject, void* arg = nullptr) final override;

	virtual void CallStartOnce() final override;

	virtual void ApplyEnableChange() final override;

	virtual void OnDestroy() final override;

private:

	GameObject*		m_gameObject = nullptr;

	bool			m_enable = true;

	bool			m_beforeActive = true;

	bool			m_callStart = true;

	int				m_executionOrder = 0;
};
ENGINE_END
