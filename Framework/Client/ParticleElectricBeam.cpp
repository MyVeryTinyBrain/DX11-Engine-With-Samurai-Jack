#include "stdafx.h"
#include "ParticleElectricBeam.h"
#include "Config.h"

void ParticleElectricBeam::Awake()
{
	m_goParent = CreateGameObjectToChild(transform);
	m_goParent->transform->localEulerAngles = V3(0, 0, m_spinAngle);
	GameObject* goRenderer = CreateGameObjectToChild(m_goParent->transform);
	goRenderer->transform->localEulerAngles = V3(90, 90, 0);
	goRenderer->transform->localScale = V3::one() * 10.0f;
	m_renderer = goRenderer->AddComponent<MeshRenderer>();
}

void ParticleElectricBeam::Start()
{
	ResourceRef<Shader> shader = system->resource->builtIn->colorShader;
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_renderer->material = material;
}

void ParticleElectricBeam::Update()
{
	float dt = system->time->deltaTime;
	float percent = Clamp01(m_elapsed / m_duration);

	Color color = Color::Lerp(Color::RGBA255(255, 240, 110, 255), Color(1.0f, 1.0f, 1.0f, 0.0f), percent);
	m_renderer->material->SetColor("_Color", color);

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
	}

	V3 localEulerAngles = m_goParent->transform->localEulerAngles;
	localEulerAngles.z += dt * 10.0f;
	m_goParent->transform->localEulerAngles = localEulerAngles;

	m_elapsed += dt;
}

ParticleElectricBeam* ParticleElectricBeam::Create(
	Scene* scene, 
	ParticleElectricBeam::Type type, 
	const V3& position,
	const V3& forward,
	float spinAngle,
	float duration)
{
	GameObject* goParticle = scene->CreateGameObject();
	goParticle->transform->position = position;
	goParticle->transform->forward = forward;
	ParticleElectricBeam* particle = goParticle->AddComponent<ParticleElectricBeam>();
	
	if (type == ParticleElectricBeam::Type::RANDOM)
	{
		uint typeIndex = rand() % (uint)ParticleElectricBeam::Type::NUM_TYPES;
		type = (ParticleElectricBeam::Type)typeIndex;
	}

	tstring path = TEXT("");
	switch (type)
	{
		case ParticleElectricBeam::Type::_1:
			path = MESH_ELECTRIC_01;
			break;
		case ParticleElectricBeam::Type::_2:
			path = MESH_ELECTRIC_02;
			break;
		case ParticleElectricBeam::Type::_3:
			path = MESH_ELECTRIC_03;
			break;
		case ParticleElectricBeam::Type::_4:
			path = MESH_ELECTRIC_04;
			break;
		case ParticleElectricBeam::Type::_5:
			path = MESH_ELECTRIC_05;
			break;
		case ParticleElectricBeam::Type::_6:
			path = MESH_ELECTRIC_06;
			break;
		case ParticleElectricBeam::Type::_7:
			path = MESH_ELECTRIC_07;
			break;
	}
	ResourceRef<Mesh> mesh = scene->system->resource->Find(path);

	particle->m_renderer->mesh = mesh;
	particle->m_duration = duration;
	particle->m_spinAngle = spinAngle;

	return particle;
}
