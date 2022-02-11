#include "EnginePCH.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene.h"

Transform* Component::GetTransform() const
{
	return m_gameObject->transform;
}

void Component::SetEnable(bool value)
{
	if (m_enable == value)
		return;

	// Ȱ��ȭ -> ��Ȱ��ȭ ��ȯ�� ������ ���� �ʽ��ϴ�.
	// ��Ȱ��ȭ -> Ȱ��ȭ ��ȯ�� ������Ʈ�� �ı��� ������ �ƴ� ���� �����մϴ�.
	if (value && IsShouldDestroy())
		return;

	m_enable = value;

	ApplyEnableChange();
}

bool Component::IsActive() const
{
	return m_enable && transform->activeInHierarchy;
}

int Component::GetExecutionOrder() const
{
	return m_executionOrder;
}

void Component::SetExecutionOrder(int order)
{
	m_executionOrder = order;
}

System* Component::GetSystem() const
{
	return m_gameObject->GetSystem();
}

GameObject* Component::CreateGameObject(const tstring& name, const tstring& tag)
{
	return gameObject->regionScene->CreateGameObject(name, tag);
}

GameObject* Component::CreateGameObjectToChild(Transform* parent, const tstring& name, const tstring& tag)
{
	return gameObject->regionScene->CreateGameObjectToChild(parent, name, tag);
}

GameObject* Component::FindGameObject(const tstring& name) const
{
	return gameObject->regionScene->FindGameObject(name);
}

std::vector<GameObject*> Component::FindGameObjects(const tstring& name) const
{
	return gameObject->regionScene->FindGameObjects(name);
}

GameObject* Component::FindGameObjectWithTag(const tstring& tag) const
{
	return gameObject->regionScene->FindGameObjectWithTag(tag);
}

std::vector<GameObject*> Component::FindGameObjectsWithTag(const tstring& tag) const
{
	return gameObject->regionScene->FindGameObjectsWithTag(tag);
}

void Component::Initialize(GameObject* gameObject)
{
	m_gameObject = gameObject;

	// ������Ʈ �Ŵ����� �� �� ������Ʈ�� �߰��մϴ�.
	IScene* iScene = gameObject->regionScene;
	iScene->RegistComponent(this);

	Awake();
	OnEnable();
}

void Component::CallStartOnce()
{
	if (!m_callStart)
		return;

	m_callStart = false;

	Start();
}

void Component::ApplyEnableChange()
{
	bool active = IsActive();

	if (active == m_beforeActive)
		return;

	if (active)
		OnEnable();
	else
		OnDisable();

	m_beforeActive = active;
}

void Component::OnDestroy()
{
	// �ı��Ǹ� ������Ʈ �Ŵ����� �ı��� ������Ʈ ����Ʈ�� �߰��մϴ�.
	IScene* iScene = m_gameObject->regionScene;
	iScene->ToDestoryedComponent(this);

	// ���ӿ�����Ʈ���� Ż���մϴ�.
	m_gameObject->RemoveComponent(this);

	// ������Ʈ�� ��Ȱ��ȭ ���·� ��ȯ�մϴ�.
	// �̿� ���� OnDisable() �Լ��� ȣ��� �� �ֽ��ϴ�.
	SetEnable(false);

	// �ı��� �˸��� �Լ��� ȣ���մϴ�.
	OnDestroyed();
}

