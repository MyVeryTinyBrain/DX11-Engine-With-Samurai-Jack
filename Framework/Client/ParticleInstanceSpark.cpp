#include "stdafx.h"
#include "ParticleInstanceSpark.h"
#include "Config.h"

void ParticleInstanceSpark::Awake()
{
	m_rigidbody = gameObject->AddComponent<Rigidbody>();
	m_rigidbody->SetRotationLock(Rigidbody::Axis::All, true);
	m_rigidbody->positionSolverIterationCount = 1;
	m_rigidbody->velocitySolverIterationCount = 1;
	
	m_collider = gameObject->AddComponent<SphereCollider>();
	m_collider->layerIndex = PhysicsLayer_Particle;
	m_collider->restitution = 0.9f;
	m_collider->friction = 0.0f;
	m_collider->radius = 0.1f;
	m_collider->OnCollisionEnter += func<void(const Collision&)>(this, &ParticleInstanceSpark::OnCollisionEnter);

	GameObject* goRenderer = CreateGameObjectToChild(transform);
	m_renderer = goRenderer->AddComponent<BillboardRenderer>();
	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_SPARK);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ParticleInstanceYellowSparkMaterial"));
	m_renderer->material = material;
	m_renderer->material->SetTexture("_SparkSpreadTexture", system->resource->Find(TEX_EFFECT_SPARK_SPREAD));
	m_renderer->material->SetTexture("_SparkDotTexture", system->resource->Find(TEX_EFFECT_SPARK_DOT));
}

void ParticleInstanceSpark::Update()
{
	if (m_duration == 0.0f)
	{
		gameObject->Destroy();
		return;
	}

	m_elapsed += system->time->deltaTime;
	float percent = Clamp01(m_elapsed / m_duration);

	float scale = (1.0f - percent) * m_initScale;
	m_renderer->transform->localScale = V3::one() * scale;

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
		return;
	}
}

void ParticleInstanceSpark::OnCollisionEnter(const Collision& collision)
{
}

void ParticleInstanceSpark::SetVelocity(const V3& velocity)
{
	m_rigidbody->velocity = velocity;
}

void ParticleInstanceSpark::Create(
	Scene* scene, 
	const V3& position, 
	const V3& normal,
	float minAngle, float maxAngle,
	float minVSpeed, float maxVSpeed, 
	float minHSpeed, float maxHSpeed, 
	float minScale, float maxScale, 
	float minDuration, float maxDuration,
	uint count,
	const Color& color)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> rdAngleGen(-(maxAngle - minAngle), +(maxAngle - minAngle));
	std::uniform_real_distribution<float> rdScaleGen(minScale, maxScale);
	std::uniform_real_distribution<float> rdHSpeedGen(minHSpeed, maxHSpeed);
	std::uniform_real_distribution<float> rdVSpeedGen(minVSpeed, maxVSpeed);
	std::uniform_real_distribution<float> rdDurationGen(minDuration, maxDuration);

	for (uint i = 0; i < count; ++i)
	{
		GameObject* goEffect = scene->CreateGameObject();
		ParticleInstanceSpark* effect = goEffect->AddComponent<ParticleInstanceSpark>();
		goEffect->transform->position = position;

		float angleX = rdAngleGen(gen);
		float angleY = rdAngleGen(gen);
		float angleZ = rdAngleGen(gen);
		Q q = Q::Euler(angleX, angleY, angleZ);
		V3 dir = q.MultiplyVector(normal).normalized;

		float hSpeed = rdHSpeedGen(gen);
		float vSpeed = rdVSpeedGen(gen);
		effect->SetVelocity(dir * hSpeed + V3::up() * vSpeed);

		effect->m_duration = rdDurationGen(gen);
		effect->m_initScale = rdScaleGen(gen);
		
		effect->m_renderer->SetInstanceData(0, color);
	}
}
