#include "stdafx.h"
#include "EffectShockwave.h"
#include "Config.h"

void EffectShockwave::Awake()
{
	m_goCnt = CreateGameObjectToChild(transform);

	GameObject* goRenderer = CreateGameObjectToChild(m_goCnt->transform);
	goRenderer->transform->localEulerAngles = V3(90, 180, 0);
	goRenderer->transform->localScale = V3(0.5f, 0.5f, 0.5f);

	m_renderer = goRenderer->AddComponent<MeshRenderer>();
	m_renderer->mesh = system->resource->Find(MESH_CYILNDER_THIN_TWOSIDES);

	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_SHOCKWAVE);
	material = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_renderer->material = material;

	ResourceRef<Texture> texture = system->resource->Find(TEX_EFFECT_ELECTRIC_H0);
	material->SetTexture("_Texture", texture);
	material->SetColor("_Color", m_startColor);
}

void EffectShockwave::Update()
{
	float dt = system->time->deltaTime;

	const static float speed = 3.0f;
	m_uvAdjust.x += dt * speed;
	m_uvAdjust.y += dt * 1.0f;
	material->SetVector("_UVAdjust", m_uvAdjust);
	
	m_elapsed += dt;
	float percent = Clamp01(m_elapsed / m_duration);
	
	float scale = Lerp(m_startScale, m_endScale, pow(percent, 0.6f));
	m_goCnt->transform->localScale = V3(scale, 1.0f, scale);

	Color color = Color::Lerp(m_startColor, m_endColor, pow(percent, 5.0f));
	material->SetColor("_Color", color);

	if (percent >= 1.0f)
	{
		gameObject->Destroy();
		return;
	}
}

void EffectShockwave::Create(
	Scene* scene,
	const V3& groundPosition,
	float duration, 
	float height,
	float startScale, float endScale, 
	const Color& startColor, const Color& endColor)
{
	{
		GameObject* goShockwave = scene->CreateGameObject();
		goShockwave->transform->position = groundPosition + V3::up() * height * 0.5f;
		goShockwave->transform->localScale = V3(1.0f, height, 1.0f);

		EffectShockwave* effect = goShockwave->AddComponent<EffectShockwave>();
		effect->m_duration = duration;
		effect->m_startScale = startScale;
		effect->m_endScale = endScale;
		effect->m_startColor = startColor;
		effect->m_endColor = endColor;
	}
	{
		GameObject* goShockwave = scene->CreateGameObject();
		goShockwave->transform->position = groundPosition + V3::up() * height * 0.5f;
		goShockwave->transform->localScale = V3(1.0f, height, 1.0f);

		EffectShockwave* effect = goShockwave->AddComponent<EffectShockwave>();
		effect->m_duration = duration + 0.3f;
		effect->m_startScale = startScale;
		effect->m_endScale = endScale;
		effect->m_startColor = startColor;
		effect->m_endColor = endColor;
	}
}
