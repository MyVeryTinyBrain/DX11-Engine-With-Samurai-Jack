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

	// DontDestroyOnLoad 플래그가 활성화된 게임오브젝트가 다른 씬으로 이동되었을 떄 호출합니다.
	virtual void OnSceneChanged() = 0;

};

ENGINE_END
