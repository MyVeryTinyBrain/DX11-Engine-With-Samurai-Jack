#include "stdafx.h"
#include "EffectSpark.h"
#include "Config.h"

void EffectSpark::Awake()
{
	m_rigidbody = gameObject->AddComponent<Rigidbody>();
	m_rigidbody->SetRotationLock(Rigidbody::Axis::All, true);
	
	m_collider = gameObject->AddComponent<SphereCollider>();
	m_collider->layerIndex = PhysicsLayer_Particle;
	m_collider->restitution = 0.5f;
	m_collider->radius = 0.1f;
	m_collider->OnCollisionEnter += func<void(const Collision&)>(this, &EffectSpark::OnCollisionEnter);

	GameObject* goRenderer = CreateGameObjectToChild(transform);
	m_renderer = goRenderer->AddComponent<BillboardRenderer>();
	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_BILLBOARD_EFFECT);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("EffectSparkMaterial"));
	m_renderer->material = material;
	m_renderer->material->SetTexture("_Texture", system->resource->Find(TEX_EFFECT_SPARK));
	m_renderer->material->SetColor("_Color", Color(1.0f, 0.9764f, 0.466f, 1.0f));
}

void EffectSpark::Update()
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

void EffectSpark::OnCollisionEnter(const Collision& collision)
{
}

void EffectSpark::SetVelocity(const V3& velocity)
{
	m_rigidbody->velocity = velocity;
}

void EffectSpark::Create(
	Scene* scene, 
	const V3& position, 
	const V3& normal,
	float minAngle, float maxAngle,
	float minVSpeed, float maxVSpeed, 
	float minHSpeed, float maxHSpeed, 
	float minScale, float maxScale, 
	float minDuration, float maxDuration,
	uint count)
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
		EffectSpark* effect = goEffect->AddComponent<EffectSpark>();
		goEffect->transform->position = position;

		float radianX = rdAngleGen(gen);
		float radianY = rdAngleGen(gen);
		float radianZ = rdAngleGen(gen);
		Q q = Q::Euler(radianX, radianY, radianZ);
		V3 dir = q.MultiplyVector(normal).normalized;

		float hSpeed = rdHSpeedGen(gen);
		float vSpeed = rdVSpeedGen(gen);
		effect->SetVelocity(dir * hSpeed + V3::up() * vSpeed);

		effect->m_duration = rdDurationGen(gen);
		effect->m_initScale = rdScaleGen(gen);
	}
}
