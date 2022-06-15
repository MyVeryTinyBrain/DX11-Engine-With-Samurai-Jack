#include "stdafx.h"
#include "ParticleSpark.h"
#include "Config.h"

void ParticleSpark::Awake()
{
	GameObject* goRenderer = CreateGameObjectToChild(transform);
	m_renderer = goRenderer->AddComponent<BillboardRenderer>();
	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_SPARK);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ParticleSparkMaterial"));
	m_renderer->material = material;
	m_renderer->material->SetTexture("_SparkSpreadTexture", system->resource->Find(TEX_EFFECT_SPARK_SPREAD));
	m_renderer->material->SetTexture("_SparkDotTexture", system->resource->Find(TEX_EFFECT_SPARK_DOT));
}

void ParticleSpark::Update()
{
	if (m_duration == 0.0f)
	{
		gameObject->Destroy();
		return;
	}

	float dt = system->time->deltaTime;

	if (!m_useRigidbody)
	{
		const static float damping = 1.0f;
		transform->position += m_fakeVelocity * dt;
		m_fakeVelocity = V3::Lerp(m_fakeVelocity, V3::zero(), dt * damping);
		m_fakeVelocity += V3(0.0f, -9.81f, 0.0f) * dt;
	}

	m_elapsed += dt;
	float percent = Clamp01(m_elapsed / m_duration);

	float scale = (1.0f - percent) * m_initScale;
	m_renderer->transform->localScale = V3::one() * scale;

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
		return;
	}
}

void ParticleSpark::OnCollisionEnter(const Collision& collision)
{
}

void ParticleSpark::Init(bool useRigidbody)
{
	m_useRigidbody = useRigidbody;

	if (m_useRigidbody)
	{
		m_rigidbody = gameObject->AddComponent<Rigidbody>();
		m_rigidbody->SetRotationLock(Rigidbody::Axis::All, true);
		m_rigidbody->positionSolverIterationCount = 1;
		m_rigidbody->velocitySolverIterationCount = 1;

		m_collider = gameObject->AddComponent<SphereCollider>();
		m_collider->layerIndex = PhysicsLayer_Particle;
		m_collider->restituionCombineMode = Collider::CombineMode::Max;
		m_collider->restitution = 0.5f;
		m_collider->friction = 0.5f;
		m_collider->radius = 0.1f;
		m_collider->OnCollisionEnter += func<void(const Collision&)>(this, &ParticleSpark::OnCollisionEnter);
	}
}

void ParticleSpark::SetVelocity(const V3& velocity)
{
	if (m_useRigidbody)
	{
		m_rigidbody->velocity = velocity;
	}
	else
	{
		m_fakeVelocity = velocity;
	}
}

void ParticleSpark::Create(
	Scene* scene, 
	const V3& position, 
	const V3& normal,
	float minAngle, float maxAngle,
	float minVSpeed, float maxVSpeed, 
	float minHSpeed, float maxHSpeed, 
	float minScale, float maxScale, 
	float minDuration, float maxDuration,
	uint count,
	bool useRigidbody,
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
		ParticleSpark* effect = goEffect->AddComponent<ParticleSpark>();
		goEffect->transform->position = position;

		effect->Init(useRigidbody);

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
