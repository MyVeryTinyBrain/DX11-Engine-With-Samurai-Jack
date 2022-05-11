#pragma once

class SoundTest : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	AudioSource* m_audioSource = nullptr;
	ResourceRef<AudioClip> m_clip;
};

