#include "stdafx.h"
#include "EffectRing01.h"
#include "Config.h"

void EffectRing01::Awake()
{
	GameObject* goRenderer = CreateGameObjectToChild(transform);
	m_renderer = goRenderer->AddComponent<BillboardRenderer>();

	m_renderer->mesh = system->resource->Find(MESH_DOME);

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_RING);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_renderer->material = material;
	m_renderer->material->SetTexture("_Texture", system->resource->Find(TEX_EFFECT_RING01));
	m_renderer->material->SetTexture("_DistortionTexture", system->resource->Find(TEX_EFFECT_RING_DISTORTION));
	m_renderer->material->SetColor("_Color", m_initColor);
}

void EffectRing01::Update()
{
	if (m_duration == 0.0f)
	{
		gameObject->Destroy();
		return;
	}

	m_elapsed += system->time->deltaTime;
	float percent = Clamp01(m_elapsed / m_duration);
	float powPercent = pow(percent, m_powFactor);

	float scale = Lerp(m_startScale, m_endScale, powPercent);
	m_renderer->transform->localScale = V3::one() * scale;

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

void EffectRing01::Create(Scene* scene, const V3& position, float duration, float powFactor, float distortion, float startScale, float endScale, const Color& color)
{
	GameObject* goEffect = scene->CreateGameObject();
	EffectRing01* effect = goEffect->AddComponent<EffectRing01>();
	goEffect->transform->position = position;

	effect->m_duration = duration;
	effect->m_powFactor = powFactor;
	effect->m_distortion = distortion;
	effect->m_startScale = startScale;
	effect->m_endScale = endScale;
	effect->m_initColor = color;
}
