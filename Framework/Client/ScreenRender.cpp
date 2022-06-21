#include "stdafx.h"
#include "ScreenRender.h"
#include "Config.h"
#include "Player.h"
#include "Enemy.h"

void ScreenRender::Awake()
{
	transform->localPosition = V3::zero();
	transform->localEulerAngles = V3::zero();
	transform->localScale = V3::one();

	GameObject* goScreenColor = CreateGameObjectToChild(transform);
	m_screenColor = goScreenColor->AddComponent<ScreenRenderer>();
	m_screenColor->renderGroupOrder = 1000;
	SetFadeColor(Color::clear());

	GameObject* goPlayerHPBarBody = CreateGameObjectToChild(transform);
	goPlayerHPBarBody->transform->localPosition = V3(-0.65f, -0.9f, 0.0f);
	goPlayerHPBarBody->transform->localScale = V3(0.3f, 0.02f, 1.0f);
	m_playerHPBarBody = goPlayerHPBarBody->AddComponent<ScreenRenderer>();
	m_playerHPBarBody->renderGroupOrder = 0;
	m_playerHPBarBody->material->SetTexture("_Texture", system->resource->Find(TEX_UI_HPBAR_BODY));

	GameObject* goPlayerHPBar = CreateGameObjectToChild(transform);
	goPlayerHPBar->transform->localPosition = V3(-0.65f - 0.3f, -0.9f, 0.0f);
	GameObject* goPlayerHPBarChild = CreateGameObjectToChild(goPlayerHPBar->transform);
	goPlayerHPBarChild->transform->localPosition = V3(0.3f, 0.0f, 0.0f);
	goPlayerHPBarChild->transform->localScale = V3(0.3f, 0.02f, 1.0f);
	m_playerHPBar = goPlayerHPBarChild->AddComponent<ScreenRenderer>();
	m_playerHPBar->renderGroupOrder = 1;
	m_playerHPBar->material->SetTexture("_Texture", system->resource->Find(TEX_UI_HPBAR_RED));

	GameObject* goEnemyHPBarBody = CreateGameObjectToChild(transform);
	goEnemyHPBarBody->transform->localPosition = V3(0.0f, 0.9f, 0.0f);
	goEnemyHPBarBody->transform->localScale = V3(0.6f, 0.02f, 1.0f);
	m_enemyHPBarBody = goEnemyHPBarBody->AddComponent<ScreenRenderer>();
	m_enemyHPBarBody->renderGroupOrder = 0;
	m_enemyHPBarBody->material->SetTexture("_Texture", system->resource->Find(TEX_UI_HPBAR_BODY));

	GameObject* goEnemyHPBar = CreateGameObjectToChild(transform);
	goEnemyHPBar->transform->localPosition = V3(-0.0f - 0.6f, 0.9f, 0.0f);
	GameObject* goEnemyHPBarChild = CreateGameObjectToChild(goEnemyHPBar->transform);
	goEnemyHPBarChild->transform->localPosition = V3(0.6f, 0.0f, 0.0f);
	goEnemyHPBarChild->transform->localScale = V3(0.6f, 0.02f, 1.0f);
	m_enemyHPBar = goEnemyHPBarChild->AddComponent<ScreenRenderer>();
	m_enemyHPBar->renderGroupOrder = 1;
	m_enemyHPBar->material->SetTexture("_Texture", system->resource->Find(TEX_UI_HPBAR_RED));
}

void ScreenRender::Start()
{
	m_fadeColor = Color::black();
	FadeOut(1.0f);
}

void ScreenRender::Update()
{
	FadeUpdate();
	PlayerHPUpdate();
	EnemyHPUpdate();
}

void ScreenRender::FadeUpdate()
{
	switch (m_fadeState)
	{
		case FadeState::FadeIn:
		{
			m_fadeElapsed += system->time->deltaTime;
			float percent = Clamp01(m_fadeElapsed / m_fadeDuration);
			Color color = Color::Lerp(Color::clear(), m_fadeColor, percent);
			SetFadeColor(color);
			if (percent >= 1.0f)
			{
				m_fadeState = FadeState::None;
				OnEndFadeIn();
			}
		}
		break;
		case FadeState::FadeOut:
		{
			m_fadeElapsed += system->time->deltaTime;
			float percent = Clamp01(m_fadeElapsed / m_fadeDuration);
			Color color = Color::Lerp(m_fadeColor, Color::clear(), percent);
			SetFadeColor(color);
			if (percent >= 1.0f)
			{
				m_fadeState = FadeState::None;
				OnEndFadeOut();
			}
		}
		break;
	}
}

void ScreenRender::SetFadeColor(const Color& color)
{
	if (color.a <= 0.0f)
	{
		m_screenColor->enable = false;
	}
	else
	{
		m_screenColor->enable = true;
	}
	m_screenColor->material->SetColor("_Color", color);
}

void ScreenRender::PlayerHPUpdate()
{
	bool use = (!m_player.IsNull());
	m_playerHPBarBody->enable = use;
	m_playerHPBar->enable = use;
	if (use)
	{
		V3 localScale = m_playerHPBar->transform->parent->localScale;
		localScale.x = m_player->hp / m_player->maxHP;
		m_playerHPBar->transform->parent->localScale = localScale;
	}
}

void ScreenRender::EnemyHPUpdate()
{
	bool use = (!m_enemy.IsNull()) && (!m_enemy->isDead);
	m_enemyHPBarBody->enable = use;
	m_enemyHPBar->enable = use;
	if (use)
	{
		V3 localScale = m_enemyHPBar->transform->parent->localScale;
		localScale.x = m_enemy->hp / m_enemy->maxHP;
		m_enemyHPBar->transform->parent->localScale = localScale;
	}
}

void ScreenRender::StopFade()
{
	m_fadeState = FadeState::None;
	m_screenColor->material->SetColor("_Color", Color::clear());
}

void ScreenRender::FadeIn(const Color& fadeColor, float duration)
{
	m_fadeColor = fadeColor;
	m_fadeDuration = duration;
	m_fadeElapsed = 0.0f;
	m_fadeState = FadeState::FadeIn;
}

void ScreenRender::FadeOut(float duration)
{
	m_fadeDuration = duration;
	m_fadeElapsed = 0.0f;
	m_fadeState = FadeState::FadeOut;
}

void ScreenRender::ShowPlayerHP(Ref<Player> player)
{
	m_player = player;
}

void ScreenRender::ShowEnemyHP(Ref<Enemy> enemy)
{
	m_enemy = enemy;
}
