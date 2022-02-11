#pragma once

ENGINE_BEGIN
class GameObject;
class Transform;
class System;
class ENGINE_API IComponent abstract
{
public:

	virtual ~IComponent() = default;

	// �� ������Ʈ�� �����ϰ� �� �Լ��� ȣ���ؾ� �մϴ�.
	// Awake() �� OnEnable() �Լ��� ȣ���մϴ�.
	virtual void Initialize(GameObject* gameObject) = 0;

	// ������Ʈ ���� ���ķ� �� �Լ��� �������� �ʾҴٸ� Start() �Լ��� ȣ���մϴ�.
	virtual void CallStartOnce() = 0;

	// �� �Լ��� ȣ���������� ������ �� �Լ��� ȣ���������� Ȱ��ȭ ���°� �ٸ� ��쿡�� 
	// OnEnable() Ȥ�� OnDisable() �Լ��� ȣ���մϴ�.
	virtual void ApplyEnableChange() = 0;

public:

	// ������Ʈ�� ������ ���� ȣ��˴ϴ�.
	virtual void Awake() = 0;

	// ������Ʈ�� ������ ���� �����ӿ� ȣ��˴ϴ�.
	virtual void Start() = 0;

	// ���� ������Ʈ�Դϴ�.
	virtual void FixedUpdate() = 0;

	// ���� ������Ʈ ���Ŀ� ȣ��˴ϴ�.
	virtual void PostFixedUpdate() = 0;

	// ������Ʈ ������ ȣ��˴ϴ�.
	virtual inline void PreUpdate() = 0;

	// �ִϸ��̼� ���¸� �����ϴ� ������Ʈ�Դϴ�.
	virtual void AnimationUpdate() = 0;

	// ������Ʈ�Դϴ�.
	virtual void Update() = 0;

	// ������Ʈ ���Ŀ� ȣ��˴ϴ�.
	virtual void LateUpdate() = 0;

	// ������ ó���� ���� ������Ʈ�Դϴ�.
	virtual void PostUpdate() = 0;

	// ���� ���� ó���� �մϴ�.
	virtual void Render() = 0;

	// ������Ʈ�� ���°� ��Ȱ��ȭ ���¿��� Ȱ��ȭ ���·� ����� ��쿡 ȣ��˴ϴ�.
	virtual void OnEnable() = 0;

	// ������Ʈ�� ���°� Ȱ��ȭ ���¿��� ��Ȱ��ȭ ���·� ����� ��쿡 ȣ��˴ϴ�.
	virtual void OnDisable() = 0;

	// ������Ʈ�� �ı� ��û�� ���޵Ǹ� ȣ��˴ϴ�.
	// OnDestroyed() �� SetEnable(false) �Լ��� ȣ���մϴ�.
	// �ı��Ǳ� ������ Ȱ��ȭ ���¿��ٸ� SetEnable(false) ���ۿ� ���� OnDisable() �Լ��� ȣ��˴ϴ�.
	virtual void OnDestroyed() = 0;

	// ���� ����� ��� ȣ��˴ϴ�.
	virtual void OnSceneChanged() = 0;

	// �޼����� �۽��� ��� ȣ��˴ϴ�.
	virtual void OnMessage(const tstring& message, void* args = nullptr) = 0;
};

ENGINE_END