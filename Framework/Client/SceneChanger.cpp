#include "stdafx.h"
#include "SceneChanger.h"
#include "ScreenRender.h"
#include "MusicManager.h"

void SceneChanger::Init(ScreenRender* screenRender, MusicManager* musicManager)
{
	m_screenRender = screenRender;
	m_screenRender->OnEndFadeIn += func<void()>(this, &SceneChanger::OnEndFadeIn);

	m_musicManager = musicManager;
}

void SceneChanger::ChangeScene(Scene* scene, float fadeDuration)
{
	if (m_screenRender->GetFadeState() == ScreenRender::FadeState::None)
	{
		m_nextScene = scene;
		m_screenRender->FadeIn(Color::black(), fadeDuration);
		m_musicManager->Stop(fadeDuration);
	}
}

void SceneChanger::ReloadScene(float fadeDuration) {
	if (m_screenRender->GetFadeState() == ScreenRender::FadeState::None) {
		m_nextScene = nullptr;
		m_screenRender->FadeIn(Color::black(), fadeDuration);
		m_musicManager->Stop(fadeDuration);
	}
}

void SceneChanger::OnEndFadeIn()
{
	if (m_nextScene) {
		system->sceneManagement->ChangeScene(m_nextScene);
	}
	else {
		system->sceneManagement->ReloadScene();
	}
}
