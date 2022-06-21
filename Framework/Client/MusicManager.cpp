#include "stdafx.h"
#include "MusicManager.h"

void MusicManager::Awake()
{
	m_audioSource = gameObject->AddComponent<AudioSource>();
	m_audioSource->playAnywhere = true;
}

void MusicManager::Update()
{
	m_elapsed += system->time->deltaTime;
	switch (m_state)
	{
		case State::Play:
		{
			float percent = m_elapsed / m_duration;
			m_audioSource->volume = Lerp(0.0f, 1.0f, percent);
		}
		break;
		case State::Stop:
		{
			float percent = m_elapsed / m_duration;
			m_audioSource->volume = Lerp(1.0f, 0.0f, percent);
			if (percent >= 1.0f)
			{
				m_audioSource->Stop();
			}
		}
		break;
	}
}

void MusicManager::Play(ResourceRef<AudioClip> clip, float volume, float maxVolumeDuration)
{
	m_audioSource->Stop();
	m_audioSource->volume = 0.0f;
	m_audioSource->Play(clip, volume);
	m_state = State::Play;
	m_duration = maxVolumeDuration;
	m_elapsed = 0.0f;
}

void MusicManager::Stop(float stopDuration)
{
	if (m_state == State::Stop)
		return;

	m_state = State::Stop;
	m_duration = stopDuration;
	m_elapsed = 0.0f;
}
