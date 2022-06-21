#pragma once

class Player;
class Enemy;

class ScreenRender : public Component
{
public:

	enum class FadeState { FadeIn, FadeOut, None };

private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	void FadeUpdate();
	void SetFadeColor(const Color& color);
	void PlayerHPUpdate();
	void EnemyHPUpdate();

public:

	void StopFade();
	void FadeIn(const Color& fadeColor, float duration);
	void FadeOut(float duration);
	FadeState GetFadeState() const { return m_fadeState; }

	void ShowPlayerHP(Ref<Player> player);
	void ShowEnemyHP(Ref<Enemy> enemy);

private:

	ScreenRenderer* m_screenColor;
	float m_fadeDuration = 0.0f;
	float m_fadeElapsed = 0.0f;
	Color m_fadeColor = Color::black();
	FadeState m_fadeState = FadeState::None;

	ScreenRenderer* m_playerHPBarBody;
	ScreenRenderer* m_playerHPBar;
	ScreenRenderer* m_enemyHPBarBody;
	ScreenRenderer* m_enemyHPBar;
	Ref<Player> m_player;
	Ref<Enemy> m_enemy;

public:

	delegate<void()> OnEndFadeIn;
	delegate<void()> OnEndFadeOut;
};

