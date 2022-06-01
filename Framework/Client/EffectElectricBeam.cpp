#include "stdafx.h"
#include "EffectElectricBeam.h"
#include "ParticleElectricBeam.h"
#include "Config.h"
#include "Character.h"

void EffectElectricBeam::Awake()
{
	m_goBox = CreateGameObjectToChild(transform);
	m_goBox->transform->localPosition = V3::forward() * 0.5f;
	m_boxTrigger = m_goBox->AddComponent<BoxCollider>();
	m_boxTrigger->isTrigger = true;

	m_attackLayermask = 1 << PhysicsLayer_Player;
}

void EffectElectricBeam::Update()
{
	if (m_particleWaitCount <= 0.0f)
	{
		uint numParticles = uint(transform->lossyScale.z * m_particleDensity);
		numParticles = Max(1u, numParticles);

		for (uint i = 0; i < numParticles; ++i)
		{
			float percent = float(i) / float(numParticles - 1);
			CreateParticle(percent);
		}

		m_particleWaitCount = m_particleDelay;
	}
	else
	{
		m_particleWaitCount -= system->time->deltaTime;
	}

	AttackTriggerQuery();
}

void EffectElectricBeam::OnEnable()
{
	ClearHitBuffer();
}

ParticleElectricBeam* EffectElectricBeam::CreateParticle(float percent)
{
	V3 lossyScale = transform->lossyScale;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> rdXDist(-m_boxTrigger->extents.x * lossyScale.x, m_boxTrigger->extents.x * lossyScale.x);
	std::uniform_real_distribution<float> rdYDist(-m_boxTrigger->extents.y * lossyScale.y, m_boxTrigger->extents.y * lossyScale.y);
	std::uniform_real_distribution<float> rdZDist(0.0f, m_boxTrigger->extents.z * 2.0f * lossyScale.z);
	std::uniform_real_distribution<float> rdSpinAngle(0.0f, 360.0f);
	std::uniform_real_distribution<float> rdDuration(0.2f, 0.5f);

	float depth = rdZDist(gen);
	float xDist = rdXDist(gen);
	float yDist = rdYDist(gen);
	float spinAngle = rdSpinAngle(gen);
	float duration = rdDuration(gen);

	V3 position = transform->position;
	position += transform->forward * depth;
	position += transform->right * xDist;
	position += transform->up * yDist;

	ParticleElectricBeam* particle = ParticleElectricBeam::Create(
		gameObject->regionScene,
		ParticleElectricBeam::Type::RANDOM,
		position,
		transform->forward,
		spinAngle,
		duration
	);

	return particle;
}

void EffectElectricBeam::AttackTriggerQuery()
{
	vector<Collider*> overlaps = system->physics->query->OverlapAll(m_boxTrigger, m_attackLayermask, PhysicsQueryType::Collider);
	for (auto& overlap : overlaps)
	{
		Rigidbody* rigidbody = overlap->rigidbody;
		if (!rigidbody) continue;

		Character* character = rigidbody->gameObject->GetComponent<Character>();
		if (!character) continue;

		auto result = m_hitBuffer.insert(rigidbody);
		if (!result.second) continue; // 이미 힛 버퍼에 존재합니다.

		DamageIn damage = {};
		damage.FromComponent = this;
		damage.FromDirection = character->transform->position - transform->position;
		damage.Guardable = false;
		damage.Type = DamageInType::BLOW;
		damage.Damage = 1.0f;
		character->Damage(damage);
	}
}

void EffectElectricBeam::ClearHitBuffer()
{
	if (!m_hitBuffer.empty())
		m_hitBuffer.clear();
}
