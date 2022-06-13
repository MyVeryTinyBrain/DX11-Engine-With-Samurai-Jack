#include "stdafx.h"
#include "ParticleFlame.h"
#include "Config.h"

void ParticleFlame::Awake()
{
	GameObject* goRenderer = CreateGameObjectToChild(transform);
	m_billboardRenderer = goRenderer->AddComponent<BillboardRenderer>();
	m_billboardRenderer->SetSelfLockFlags(BillboardRenderer::LockFlag::X | BillboardRenderer::LockFlag::Y);
	float angle = float(rand() % 361);
	m_billboardRenderer->transform->localEulerAngles = V3(0.0f, 0.0f, angle);

	ResourceRef<Texture> noise = system->resource->Find(TEX_NOISE_01);
	ResourceRef<Texture> fire = system->resource->Find(TEX_FIRE_01);

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_FIRE);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ParticleFireMaterial"));
	material->SetTexture("_DistortionTexture", noise);
	material->SetTexture("_BaseTexture", fire);
	m_billboardRenderer->material = material;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> rdElapsedMultiplier(0.7f, 1.5f);
	m_randomElapsedMultiplier = rdElapsedMultiplier(gen);
}

void ParticleFlame::Update()
{
	float dt = system->time->deltaTime;
	float percent = Clamp01(m_elapsed / m_duration);

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
	}

	transform->position += m_velocity * dt;
	m_velocity = V3::Lerp(m_velocity, V3::zero(), dt * m_damping);

	float startToBridgePercent = percent / m_bridgePercent;
	float bridgeToEndPercent = Max(0.0f, percent - m_bridgePercent) / Max(0.0f, 1.0f - m_bridgePercent);
	float scale = Lerp(m_startScale, m_bridgeScale, pow(startToBridgePercent, m_scalePowFactor));
	scale = Lerp(scale, m_endScale, pow(bridgeToEndPercent, m_scalePowFactor));
	transform->localScale = V3::one() * scale;

	Color color = Color::Lerp(m_startColor, m_endColor, pow(percent, m_colorPowFactor));
	m_billboardRenderer->SetInstanceData(0, color);

	V4 time;
	time.x = m_elapsed * m_randomElapsedMultiplier;
	m_billboardRenderer->SetInstanceData(1, time);

	m_billboardRenderer->SetInstanceData(2, m_minColor);
	m_billboardRenderer->SetInstanceData(3, m_maxColor);

	m_elapsed += dt;
}

ParticleFlame* ParticleFlame::Create(
	Scene* scene, 
	const V3& position, 
	float duration, 
	const V3& velocity, float damping, 
	float startScale, float bridgePercent, float bridgeScale, float endScale, float scalePowFactor,
	Color startColor, Color endColor, float colorPowFactor)
{
	GameObject* goParticle = scene->CreateGameObject();
	goParticle->transform->position = position;
	ParticleFlame* particle = goParticle->AddComponent<ParticleFlame>();

	particle->m_duration = duration;
	particle->m_velocity = velocity;
	particle->m_damping = damping;
	particle->m_startScale = startScale;
	particle->m_bridgePercent = bridgePercent;
	particle->m_bridgeScale = bridgeScale;
	particle->m_endScale = endScale;
	particle->m_scalePowFactor = scalePowFactor,
	particle->m_startColor = startColor;
	particle->m_endColor = endColor;
	particle->m_colorPowFactor = colorPowFactor;

	return particle;
}
