#pragma once

#include "ISceneManagement.h"

ENGINE_BEGIN
class ENGINE_API SceneManagement : public ISceneManagement
{
public:

	SceneManagement();

	virtual ~SceneManagement();

private:

	virtual bool Initialize(System* system) override;

	virtual bool TryChangeScene() override;

public:

	void ChangeScene(Scene* scene);

	void ReloadScene();

	Scene* GetCurrentScene() const;

	_declspec(property(get = GetCurrentScene)) Scene* currentScene;

private:

	Scene*		m_current = nullptr;

	Scene*		m_next = nullptr;

	System*		m_system = nullptr;
};
ENGINE_END
