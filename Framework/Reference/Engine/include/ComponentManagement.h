#pragma once

ENGINE_BEGIN
class Scene;
class Component;
class ComponentManagement final
{
public: 

	using Components = vector<Component*>;
	using ComponentsByExecutionOrder = map<uint, Components>;

public:

	ComponentManagement(Scene* scene);

	virtual ~ComponentManagement();

public:

	void RegistComponent(Component* component);
	void ReregistComponent(Component* component, uint beforeExecutionOrder);
	void ToDestroyedComponent(Component* component);

private:

	bool UnregistComponent(Component* component);

public:

	void DeleteAllComponents(bool deleteAll);

public:

	void ReadyComponents();

	const ComponentsByExecutionOrder& GetComponentsByExecutionOrders() const { return m_components; }
	const Components& GetComponents(uint executionOrder) const;

	void DeleteDestroyedComponents();

private:

	Scene*						m_scene = nullptr;

	Components					m_createdComponents;
	Components					m_destroyedComponents;

	ComponentsByExecutionOrder	m_components;
};

ENGINE_END
