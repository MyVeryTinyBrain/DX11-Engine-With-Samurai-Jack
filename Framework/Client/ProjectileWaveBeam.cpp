#include "stdafx.h"
#include "ProjectileWaveBeam.h"
#include "EffectExplosion.h"
#include "Config.h"

void ProjectileWaveBeam::Awake()
{
	GameObject* goRenderer = CreateGameObjectToChild(transform);
	goRenderer->transform->localEulerAngles = V3(90, 180, 0);
	goRenderer->transform->localScale = V3::one() * 1.0f;
	m_renderer = goRenderer->AddComponent<MeshRenderer>();
	m_renderer->mesh = system->resource->Find(MESH_WAVE);
	m_renderer->material = system->resource->builtIn->whiteColorMaterial;

	m_trigger = gameObject->AddComponent<SphereCollider>();
	m_trigger->isTrigger = true;
}

void ProjectileWaveBeam::Update()
{
	float dt = system->time->deltaTime;
	float delta = dt * m_speed;

	PhysicsHit hit;
	if (system->physics->query->Sweep(
		hit,
		m_trigger,
		transform->forward, delta,
		1 << PhysicsLayer_Player | 1 << PhysicsLayer_Default,
		PhysicsQueryType::Collider))
	{
		transform->position = hit.Point;
		gameObject->Destroy();
	}
	else
	{
		transform->position += transform->forward * dt * m_speed;
	}
}

void ProjectileWaveBeam::OnDestroyed()
{
	GameObject* goEffect = CreateGameObject();
	goEffect->transform->position = transform->position;
	goEffect->transform->localScale = V3::one() * 10.0f;
	goEffect->AddComponent<EffectExplosion>();
}
