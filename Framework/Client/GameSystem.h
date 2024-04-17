#pragma once

class EventSystem;
class ScreenRender;
class SceneChanger;
class MusicManager;

class Player;
class Enemy;

class GameSystem : public Component
{
private:

	virtual void Awake() override;
	virtual void OnDestroyed() override;

public:

	void StopFade();
	void ChangeScene(Scene* scene, float fadeDuration = 1.0f);
	void ReloadScene(float fadeDuration = 1.0f);
	void ShowPlayerHP(Ref<Player> player);
	void ShowEnemyHP(Ref<Enemy> enemy);
	void PlayMusic(ResourceRef<AudioClip> music, float volume, float maxVolumeDuration);
	void StopMusic(float stopDuration);

private:

	EventSystem* m_eventSystem;
	ScreenRender* m_screenRender;
	SceneChanger* m_sceneChanger;
	MusicManager* m_musicManager;

private:

	static GameSystem* g_gameSystem;

public:

	static GameSystem* GetInstance();
};

