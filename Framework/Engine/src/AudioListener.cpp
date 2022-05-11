#include "EnginePCH.h"
#include "AudioListener.h"
#include "Transform.h"
#include "System.h"
#include "SoundSystem.h"

void AudioListener::LateUpdate()
{
	// �������� Ʈ�������� �����մϴ�.
	if (m_listener)
	{
		m_listener->Update(transform->position, V3::zero(), transform->forward, transform->up);
	}
}

void AudioListener::OnEnable()
{
	m_listener = system->sound->CreateListener();
}

void AudioListener::OnDisable()
{
	system->sound->ReleaseListener(m_listener);
	m_listener = nullptr;
}
