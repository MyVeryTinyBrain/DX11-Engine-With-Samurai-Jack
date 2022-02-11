#pragma once

ENGINE_BEGIN
class Scene;
class Component;
class ComponentManagement final
{
public: 

	using Components = vector<Component*>;

public:

	ComponentManagement(Scene* scene);

	virtual ~ComponentManagement();

public:

	void RegistComponent(Component* component);

	void ToDestroyedComponent(Component* component);

private:

	bool UnregistComponent(Component* component);

public:

	void DeleteAllComponents(bool deleteAll);

public:

	void ReadyComponents();

	inline const Components& GetComponents() { return m_components; }

	void DeleteDestroyedComponents();

private:

	Scene*				m_scene = nullptr;

	Components			m_createdComponents;
	Components			m_destroyedComponents;

	Components			m_components;
};

ENGINE_END
