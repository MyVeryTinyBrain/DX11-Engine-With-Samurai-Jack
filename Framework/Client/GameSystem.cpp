#include "stdafx.h"
#include "GameSystem.h"
#include "EventSystem.h"
#include "ScreenRender.h"
#include "SceneChanger.h"
#include "MusicManager.h"

GameSystem* GameSystem::g_gameSystem = nullptr;

void GameSystem::Awake()
{
	if (!g_gameSystem)
	{
		g_gameSystem = this;
	}
	else
	{
		return;
	}

	m_eventSystem = gameObject->AddComponent<EventSystem>();
	m_screenRender = gameObject->AddComponent<ScreenRender>();
	m_sceneChanger = gameObject->AddComponent<SceneChanger>();
	m_musicManager = gameObject->AddComponent<MusicManager>();

	m_sceneChanger->Init(m_screenRender, m_musicManager);
}

void GameSystem::OnDestroyed()
{
	if (g_gameSystem == this)
	{
		g_gameSystem = nullptr;
	}
}

void GameSystem::StopFade()
{
	m_screenRender->StopFade();
}

void GameSystem::ChangeScene(Scene* scene)
{
	m_sceneChanger->ChangeScene(scene);
}

void GameSystem::ShowPlayerHP(Ref<Player> player)
{
	m_screenRender->ShowPlayerHP(player);
}

void GameSystem::ShowEnemyHP(Ref<Enemy> enemy)
{
	m_screenRender->ShowEnemyHP(enemy);
}

void GameSystem::PlayMusic(ResourceRef<AudioClip> music, float volume, float maxVolumeDuration)
{
	m_musicManager->Play(music, volume, maxVolumeDuration);
}

void GameSystem::StopMusic(float stopDuration)
{
	m_musicManager->Stop(stopDuration);
}

GameSystem* GameSystem::GetInstance()
{
	return g_gameSystem;
}
