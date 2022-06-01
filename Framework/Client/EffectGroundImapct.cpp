#include "stdafx.h"
#include "EffectGroundImapct.h"
#include "Config.h"
#include "Character.h"

void EffectGroundImapct::Awake()
{
	m_parent = CreateGameObjectToChild(transform);

	GameObject* goRenderer = CreateGameObjectToChild(m_parent->transform);
	goRenderer->transform->localScale = V3::one() * 2.0f;
	goRenderer->transform->localEulerAngles = V3(90.0f, 0.0f, 0.0f);
	m_renderer = goRenderer->AddComponent<MeshRenderer>();
	m_renderer->mesh = system->resource->builtIn->quadMesh;

	m_trigger = m_parent->AddComponent<SphereCollider>();
	m_trigger->isTrigger = true;

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_RING);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_renderer->material = material;
	m_renderer->material->SetTexture("_Texture", system->resource->Find(TEX_EFFECT_RING02));
	m_renderer->material->SetTexture("_DistortionTexture", system->resource->Find(TEX_EFFECT_RING_DISTORTION));
	m_renderer->material->SetColor("_Color", m_initColor);

	m_attackLayermask = 1 << PhysicsLayer_Player;
}

void EffectGroundImapct::Update()
{
	AttackTriggerQuery();

	if (m_duration == 0.0f)
	{
		gameObject->Destroy();
		return;
	}

	m_elapsed += system->time->deltaTime;
	float percent = Clamp01(m_elapsed / m_duration);
	float powPercent = pow(percent, m_powFactor);

	float scale = Lerp(m_startScale, m_endScale, powPercent);
	m_parent->transform->localScale = V3::one() * scale;

	float distortion = Lerp(m_distortion, 0.0f, powPercent);
	m_renderer->material->SetFloat("_DistortionPower", distortion);

	Color color = m_initColor;
	color.a = Clamp01(1.0f - powPercent);
	m_renderer->material->SetColor("_Color", color);

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
		return;
	}
}

void EffectGroundImapct::OnEnable()
{
	ClearHitBuffer();
}

void EffectGroundImapct::AttackTriggerQuery()
{
	vector<Collider*> overlaps = system->physics->query->OverlapAll(m_trigger, m_attackLayermask, PhysicsQueryType::Collider);
	for (auto& overlap : overlaps)
	{
		Rigidbody* rigidbody = overlap->rigidbody;
		if (!rigidbody) continue;

		Character* character = rigidbody->gameObject->GetComponent<Character>();
		if (!character) continue;

		if (!character->CCT->isGrounded) continue;

		auto result = m_hitBuffer.insert(rigidbody);
		if (!result.second) continue; // 이미 힛 버퍼에 존재합니다.

		DamageIn damage = {};
		damage.FromComponent = this;
		damage.FromDirection = character->transform->position - transform->position;
		damage.FromDirection.y = 0.0f;
		damage.FromDirection.Normalize();
		damage.Guardable = false;
		damage.Type = DamageInType::BLOW;
		damage.Damage = 1.0f;
		character->Damage(damage);
	}
}

void EffectGroundImapct::ClearHitBuffer()
{
	if (!m_hitBuffer.empty())
		m_hitBuffer.clear();
}

void EffectGroundImapct::Create(Scene* scene, const V3& position, float duration, float powFactor, float distortion, float startScale, float endScale, const Color& color)
{
	GameObject* goEffect = scene->CreateGameObject();
	EffectGroundImapct* effect = goEffect->AddComponent<EffectGroundImapct>();
	goEffect->transform->position = position;

	effect->m_duration = duration;
	effect->m_powFactor = powFactor;
	effect->m_distortion = distortion;
	effect->m_startScale = startScale;
	effect->m_endScale = endScale;
	effect->m_initColor = color;
}
