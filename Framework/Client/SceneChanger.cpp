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

void SceneChanger::ChangeScene(Scene* scene)
{
	if (m_screenRender->GetFadeState() == ScreenRender::FadeState::None)
	{
		m_nextScene = scene;
		m_screenRender->FadeIn(Color::black(), 1.0f);
		m_musicManager->Stop(1.0f);
	}
}

void SceneChanger::OnEndFadeIn()
{
	system->sceneManagement->ChangeScene(m_nextScene);
}
