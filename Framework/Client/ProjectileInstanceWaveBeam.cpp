#include "stdafx.h"
#include "ProjectileInstanceWaveBeam.h"
#include "Config.h"
#include "ParticleInstanceSpark.h"
#include "EffectRing01.h"
#include "Player.h"

#define DURATION 5.0f

void ProjectileInstanceWaveBeam::Awake()
{
	GameObject* goRenderer = CreateGameObjectToChild(transform);
	goRenderer->transform->localEulerAngles = V3(90, 180, 0);
	goRenderer->transform->localScale = V3::one() * 1.5f;
	m_renderer = goRenderer->AddComponent<MeshRenderer>();
	m_renderer->mesh = system->resource->Find(MESH_WAVE);

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_WAVEBEAM);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ProjectileInstanceWaveBeamMaterial"));
	m_renderer->material = material;
	m_renderer->material->SetTexture("_Texture", system->resource->Find(TEX_EFFECT_FLASH));
	m_renderer->material->SetTexture("_DistortionTexture", system->resource->Find(TEX_NOISE_01));
	m_renderer->material->SetColor("_Color", Color(1.0f, 0.9764f, 0.466f, 1.0f));
	m_renderer->material = material;

	GameObject* goTrigger = CreateGameObjectToChild(transform);
	goTrigger->transform->localPosition = V3(0.0f, 0.0f, -1.0f);
	m_trigger = goTrigger->AddComponent<SphereCollider>();
	m_trigger->radius = 1.1f;
	m_trigger->isTrigger = true;
}

void ProjectileInstanceWaveBeam::Update()
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
		if (hit.Collider->layerIndex == PhysicsLayer_Player)
		{
			Player* player = hit.Collider->rigidbody->gameObject->GetComponent<Player>();
			if (player)
			{
				DamageIn damage = {};
				damage.Damage = 1.0f;
				damage.FromComponent = this;
				damage.FromDirection = transform->forward;
				damage.Guardable = true;
				damage.Type = DamageInType::LIGHT;
				player->Damage(damage);
			}
		}

		m_hitNormal = hit.Normal;
		transform->position = hit.Point;
		gameObject->Destroy();
	}
	else
	{
		transform->position += transform->forward * dt * m_speed;
	}

	m_elapsed += dt;
	if (m_elapsed > DURATION)
	{
		gameObject->Destroy();
	}
}

void ProjectileInstanceWaveBeam::OnDestroyed()
{
	V3 position = transform->position + m_hitNormal * 1.0f;

	ParticleInstanceSpark::Create(
		gameObject->regionScene,
		position,
		m_hitNormal,
		-90.0f, 90.0f,
		1.0f, 5.0f,
		2.0f, 10.0f,
		0.4f, 1.1f,
		0.5f, 3.0f,
		10);

	EffectRing01::Create(
		gameObject->regionScene,
		position,
		0.5f,
		0.5f,
		500.0f,
		1.0f, 2.5f,
		Color(1.0f, 0.9764f, 0.466f, 1.0f));
}
