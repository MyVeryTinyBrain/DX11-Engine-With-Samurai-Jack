#pragma once

class ScreenRender;
class MusicManager;

class SceneChanger : public Component
{
public:

	void Init(ScreenRender* screenRender, MusicManager* musicManager);
	void ChangeScene(Scene* scene);

private:

	void OnEndFadeIn();

private:

	ScreenRender* m_screenRender;
	MusicManager* m_musicManager;
	Scene* m_nextScene = nullptr;
};

