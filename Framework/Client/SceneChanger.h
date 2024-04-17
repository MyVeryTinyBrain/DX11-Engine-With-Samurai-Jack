#pragma once

class ScreenRender;
class MusicManager;

class SceneChanger : public Component
{
public:

	void Init(ScreenRender* screenRender, MusicManager* musicManager);
	void ChangeScene(Scene* scene, float fadeDuration = 1.0f);
	void ReloadScene(float fadeDuration = 1.0f);

private:

	void OnEndFadeIn();

private:

	ScreenRender* m_screenRender;
	MusicManager* m_musicManager;
	Scene* m_nextScene = nullptr;
};

