#include "stdafx.h"
#include "ParticleDust01.h"
#include "BillboardSpritesheet.h"
#include "Config.h"

void ParticleDust01::Awake()
{
	GameObject* goBillboard = CreateGameObjectToChild(transform);
	m_billboard = goBillboard->AddComponent<BillboardRenderer>();
	m_billboard->SetSelfLockFlags(BillboardRenderer::LockFlag::X | BillboardRenderer::LockFlag::Y);

	float angle = float(rand() % 361);
	m_billboard->transform->localEulerAngles = V3(0.0f, 0.0f, angle);

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_INSTANCE_UNLIT);
	ResourceRef<Material> material1 = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ParticleDust01Material_01"));
	material1->SetTexture("_Texture", system->resource->Find(TEX_DUST_PARTICLE_01));
	ResourceRef<Material> material2 = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ParticleDust01Material_02"));
	material2->SetTexture("_Texture", system->resource->Find(TEX_DUST_PARTICLE_02));
	ResourceRef<Material> material3 = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ParticleDust01Material_03"));
	material3->SetTexture("_Texture", system->resource->Find(TEX_DUST_PARTICLE_03));
	ResourceRef<Material> material4 = system->resource->factory->CreateMaterialByShaderM(shader, TEXT("ParticleDust01Material_04"));
	material4->SetTexture("_Texture", system->resource->Find(TEX_DUST_PARTICLE_04));
	ResourceRef<Material> materials[4] = { material1, material2,material3,material4 };

	uint matIndex = rand() % 4;
	m_billboard->material = materials[matIndex];
}

void ParticleDust01::Update()
{
	float dt = system->time->deltaTime;
	float percent = Clamp01(m_elapsed / m_duration);

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
	}

	transform->position += m_velocity * dt;
	m_velocity = V3::Lerp(m_velocity, V3::zero(), dt * m_damping);

	float scale = Lerp(m_startScale, m_endScale, pow(percent, m_scalePowFactor));
	transform->localScale = V3::one() * scale;

	Color color = Color::Lerp(m_startColor, m_endColor, pow(percent, m_colorPowFactor));
	m_billboard->SetInstanceData(0, color);

	m_elapsed += dt;
}

ParticleDust01* ParticleDust01::CreateOnce(
	Scene* scene,
	const V3& position, 
	const V3& velocity, float damping,
	float duration, 
	float startScale, float endScale, float scalePowFactor,
	Color startColor, Color endColor, float colorPowFactor)
{
	GameObject* goParticle = scene->CreateGameObject();
	goParticle->transform->position = position;
	ParticleDust01* particle = goParticle->AddComponent<ParticleDust01>();

	particle->m_velocity = velocity;
	particle->m_damping = damping;
	particle->m_duration = duration;
	particle->m_startScale = startScale;
	particle->m_endScale = endScale;
	particle->m_scalePowFactor = scalePowFactor,
	particle->m_startColor = startColor;
	particle->m_endColor = endColor;
	particle->m_colorPowFactor = colorPowFactor;

	return particle;
}

void ParticleDust01::CreateAroundAxis(
	Scene* scene, 
	const V3& position, 
	const V3& axis, float damping,
	float minRandomSpeed, float maxRandomSpeed,
	float minRandomDuration, float maxRandomDuration,
	float startScale, float endScale, float scalePowFactor,
	Color startColor, Color endColor, float colorPowFactor,
	uint count)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> rdSpeedGen(minRandomSpeed, maxRandomSpeed);
	std::uniform_real_distribution<float> rdDurationGen(minRandomDuration, maxRandomDuration);

	V3 forward = V3::Cross(V3::right(), axis).normalized;
	if (forward.magnitude <= Epsilon) forward = V3::Cross(axis, V3::up()).normalized;

	for (uint i = 0; i < count; ++i)
	{
		float percent = Clamp01(float(i) / float(count - 1));
		float angle = percent * 360.0f;

		Q q = Q::AxisAngle(axis, angle);
		V3 direction = q.MultiplyVector(forward).normalized;

		V3 velocity = direction * rdSpeedGen(gen);
		float duration = rdDurationGen(gen);

		ParticleDust01::CreateOnce(
			scene, 
			position, 
			velocity, damping,
			duration, 
			startScale, endScale, scalePowFactor,
			startColor, endColor, colorPowFactor);
	}
}

void ParticleDust01::CreateInSphere(
	Scene* scene, 
	const V3& position, 
	float damping, 
	float minRandomSpeed, float maxRandomSpeed, 
	float minRandomDuration, float maxRandomDuration, 
	float startScale, float endScale, float scalePowFactor, 
	Color startColor, Color endColor, float colorPowFactor, 
	uint numX, uint numY)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> rdSpeedGen(minRandomSpeed, maxRandomSpeed);
	std::uniform_real_distribution<float> rdDurationGen(minRandomDuration, maxRandomDuration);

	SphericalYaw spherical;
	spherical.Radius = 1.0f;

	for (uint y = 0; y < numY; ++y)
	{
		float heightPercent = float(y) / float(numY + 2);
		spherical.PhiAngle = 180.0f * heightPercent;

		uint numSpin = (y == 0 || y == numY - 1) ? 1 : numX;
		for (uint x = 0; x < numSpin; ++x)
		{
			float spinPercent = (float(x) / float(numSpin));
			spherical.ThetaAngle = 360.0f * spinPercent;
			V3 cartesian = spherical.cartesian;

			V3 velocity = cartesian.normalized * rdSpeedGen(gen);
			float duration = rdDurationGen(gen);

			ParticleDust01::CreateOnce(
				scene,
				position,
				velocity, damping,
				duration,
				startScale, endScale, scalePowFactor,
				startColor, endColor, colorPowFactor);
		}
	}
}
