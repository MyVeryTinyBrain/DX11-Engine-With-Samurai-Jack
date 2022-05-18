#include "EnginePCH.h"
#include "ComponentManagement.h"
#include "Component.h"
#include "GameObject.h"

ComponentManagement::ComponentManagement(Scene* scene)
{
	m_scene = scene;
}

ComponentManagement::~ComponentManagement()
{
	DeleteAllComponents(false);
}

void ComponentManagement::RegistComponent(Component* component)
{
	m_createdComponents.push_back(component);
}

void ComponentManagement::ReregistComponent(Component* component, uint beforeExecutionOrder)
{
	auto find_from_components_map = m_components.find(beforeExecutionOrder);
	if (find_from_components_map != m_components.end())
	{
		auto find_from_components = std::find(find_from_components_map->second.begin(), find_from_components_map->second.end(), component);
		if (find_from_components_map != m_components.end() &&					// execution order�� �ش�Ǵ� �� ������ �ִ� ���
			find_from_components != find_from_components_map->second.end())		// �ش� �� �������� ������Ʈ�� ã�� ���
		{
			find_from_components_map->second.erase(find_from_components);
			m_components[component->executionOrder].push_back(component);
			return;
		}
	}
}

void ComponentManagement::ToDestroyedComponent(Component* component)
{
	auto find_it = std::find(m_destroyedComponents.begin(), m_destroyedComponents.end(), component);
	if (find_it != m_destroyedComponents.end())
		return;

	m_destroyedComponents.push_back(component);
}

bool ComponentManagement::UnregistComponent(Component* component)
{
	bool erased = false;

	auto find_from_createdComponents = std::find(m_createdComponents.begin(), m_createdComponents.end(), component);
	if (find_from_createdComponents != m_createdComponents.end())
	{
		m_createdComponents.erase(find_from_createdComponents);
		erased = true;
	}

	auto find_from_components_map = m_components.find(component->executionOrder);
	if (find_from_components_map != m_components.end())
	{
		auto find_from_components = std::find(find_from_components_map->second.begin(), find_from_components_map->second.end(), component);
		if (find_from_components_map != m_components.end() &&					// execution order�� �ش�Ǵ� �� ������ �ִ� ���
			find_from_components != find_from_components_map->second.end())		// �ش� �� �������� ������Ʈ�� ã�� ���
		{
			find_from_components_map->second.erase(find_from_components);
			erased = true;
		}
	}

	return erased;
}

void ComponentManagement::DeleteAllComponents(bool deleteAll)
{
	// ������ ������Ʈ�� ��� ����Ʈ�Դϴ�.
	ComponentsByExecutionOrder dontDestroyComponents;

	while (!m_createdComponents.empty() || !m_components.empty())
	{
		// ������ ������Ʈ�� �����ϰ� ������Ʈ ����Ʈ�� �ű�ϴ�.
		ReadyComponents();

		ComponentsByExecutionOrder copiedComponents = m_components;
		m_components.clear();

		for (auto& components : copiedComponents)
		{
			for (auto& component : components.second)
			{
				if (component->gameObject->dontDestroyOnLoad)
				{
					dontDestroyComponents[component->executionOrder].push_back(component);
					continue;
				}

				component->Destroy();
				ToDestroyedComponent(component);
			}
		}
	}

	// ������ ���ӿ�����Ʈ���� ���ܵӴϴ�.
	m_components = dontDestroyComponents;

	// �ı��Ǿ�� �� ������Ʈ�� �����մϴ�.
	DeleteDestroyedComponents();
}

void ComponentManagement::ReadyComponents()
{
	if (m_createdComponents.empty())
		return;

	Components copiedCreatedComponents = m_createdComponents;
	m_createdComponents.clear();

	for (auto& createdComponent : copiedCreatedComponents)
	{
		IComponent* iComponent = createdComponent;
		iComponent->Start();

		m_components[createdComponent->executionOrder].push_back(createdComponent);
	}
}

const ComponentManagement::Components& ComponentManagement::GetComponents(uint executionOrder) const
{
	const static ComponentManagement::Components emptyComponents = ComponentManagement::Components();
	auto find_it = m_components.find(executionOrder);
	if (find_it == m_components.end())
		return emptyComponents;
	else
		return find_it->second;
}

void ComponentManagement::DeleteDestroyedComponents()
{
	if (m_destroyedComponents.empty())
		return;

	for (auto& component : m_destroyedComponents)
	{
		UnregistComponent(component);

		IComponent* iComponent = component;
		SafeDelete(iComponent);
	}

	m_destroyedComponents.clear();

	for (auto it = m_components.begin(); it != m_components.end();)
	{
		Components& componentsVector = it->second;
		if (componentsVector.empty())
		{
			it = m_components.erase(it);
		}
		else
		{
			++it;
		}
	}
}
