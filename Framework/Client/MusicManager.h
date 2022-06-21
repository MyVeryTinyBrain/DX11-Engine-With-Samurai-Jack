#pragma once

class MusicManager : public Component
{
public:

	enum class State { Play, Stop };

private:

	virtual void Awake() override;
	virtual void Update() override;

public:

	void Play(ResourceRef<AudioClip> clip, float volume, float maxVolumeDuration);
	void Stop(float stopDuration);

private:

	AudioSource* m_audioSource;
	State m_state;
	float m_duration;
	float m_elapsed = 0.0f;
};

