#include "stdafx.h"
#include "AudioTest.h"
#include "Config.h"

void AudioTest::Awake()
{
	m_audioSource = gameObject->AddComponent<AudioSource>();
	//m_audioSource->minDistance = 1.0f;
	m_audioSource->maxDistance = 10.0f;

	GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
	m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->builtIn->sphereMesh;
}

void AudioTest::Update()
{
	m_meshRenderer->transform->localScale = V3::one() * m_audioSource->minDistance * 2.0f;

	if (system->input->GetKeyDown(Key::One))
	{
		ResourceRef<AudioClip> clip = system->resource->Find(SOUND_GUARD_01);
		m_audioSource->PlayOneshot(clip);
	}
}
