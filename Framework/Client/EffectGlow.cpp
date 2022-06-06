#include "stdafx.h"
#include "EffectGlow.h"
#include "Config.h"

void EffectGlow::Awake()
{
	GameObject* goRenderer = CreateGameObjectToChild(transform);
	m_billboardRenderer = goRenderer->AddComponent<BillboardRenderer>();

	ResourceRef<Shader> shader = system->resource->builtIn->unlitShader;
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	material->SetTexture("_Texture", system->resource->Find(TEX_EFFECT_GLOW));
	m_billboardRenderer->material = material;
}

void EffectGlow::Update()
{
	float dt = system->time->deltaTime;
	float percent = Clamp01(m_elapsed / m_duration);

	V2 v2Scale = V2::Lerp(m_startScale, m_endScale, pow(percent, m_scalePowFactor));
	m_billboardRenderer->transform->localScale = V3(v2Scale.x, v2Scale.y, 1.0f);

	Color color = Color::Lerp(m_startColor, m_endColor, pow(percent, m_colorPowFactor));
	m_billboardRenderer->material->SetColor("_Color", color);

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
	}

	m_elapsed += dt;
}

EffectGlow* EffectGlow::Create(
	Scene* scene,
	const V3& position, 
	float duration, 
	const V2& startScale, const V2& endScale, float scalePowFactor, 
	const Color& startColor, const Color& endColor, float colorPowFactor)
{
	GameObject* goEffect = scene->CreateGameObject();
	goEffect->transform->position = position;

	EffectGlow* effect = goEffect->AddComponent<EffectGlow>();
	effect->m_duration = duration;
	effect->m_startScale = startScale;
	effect->m_endScale = endScale;
	effect->m_scalePowFactor = scalePowFactor;
	effect->m_startColor = startColor;
	effect->m_endColor = endColor;
	effect->m_colorPowFactor = colorPowFactor;

	return effect;
}
