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

	auto find_from_components = std::find(m_components.begin(), m_components.end(), component);
	if (find_from_components != m_components.end())
	{
		m_components.erase(find_from_components);
		erased = true;
	}

	return erased;
}

void ComponentManagement::DeleteAllComponents(bool deleteAll)
{
	// ������ ������Ʈ�� ��� ����Ʈ�Դϴ�.
	Components dontDestroyComponents;

	while (!m_createdComponents.empty() || !m_components.empty())
	{
		// ������ ������Ʈ�� �����ϰ� ������Ʈ ����Ʈ�� �ű�ϴ�.
		ReadyComponents();

		Components copiedComponents = m_components;
		m_components.clear();

		for (auto& component : copiedComponents)
		{
			if (component->gameObject->dontDestroyOnLoad)
			{
				dontDestroyComponents.push_back(component);
				continue;
			}

			component->Destroy();
			ToDestroyedComponent(component);
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

		m_components.push_back(createdComponent);
	}
}

void ComponentManagement::DeleteDestroyedComponents()
{
	if (m_destroyedComponents.empty())
		return;

	for (auto& component : m_destroyedComponents)
	{
		IComponent* iComponent = component;
		SafeDelete(iComponent);
	}

	m_destroyedComponents.clear();
}
