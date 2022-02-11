#pragma once

ENGINE_BEGIN
class Scene;
class Component;
class Transform;
class System;
class ENGINE_API IGameObject abstract
{
public:

	virtual ~IGameObject() = default;

	// DontDestroyOnLoad �÷��װ� Ȱ��ȭ�� ���ӿ�����Ʈ�� �ٸ� ������ �̵��Ǿ��� �� ȣ���մϴ�.
	virtual void OnSceneChanged() = 0;

};

ENGINE_END
