#pragma once

ENGINE_BEGIN
class Scene;
class System;
class ENGINE_API ISceneManagement abstract
{
public:

	virtual ~ISceneManagement() = default;

	virtual bool Initialize(System* system) = 0;

	virtual bool TryChangeScene() = 0;
};
ENGINE_END
