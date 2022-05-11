#include "stdafx.h"
#include "SoundTest.h"
#include <fmod.h>

void SoundTest::Awake()
{
	m_clip = AudioClip::LoadAudioClipM(system->resource, TEXT("../Resource/test.ogg"));
	m_audioSource = gameObject->AddComponent<AudioSource>();
	m_audioSource->clip = m_clip;
	m_audioSource->playAnywhere = false;
	m_audioSource->minDistance = 1.0f;
	m_audioSource->maxDistance = 0.0f;

	MeshRenderer* meshRenderer = gameObject->AddComponent<MeshRenderer>();
	meshRenderer->mesh = system->resource->builtInResources->sphereMesh;
}

void SoundTest::Update()
{
	if (system->input->GetKeyDown(Key::One))
	{
		m_audioSource->Play();
	}
	else if (system->input->GetKeyDown(Key::Two))
	{
		m_audioSource->Stop();
	}
	else if (system->input->GetKeyDown(Key::Three))
	{
		gameObject->Destroy();
	}

	m_audioSource->volume += system->input->GetMouseWheelDelta() * 0.1f;
}
