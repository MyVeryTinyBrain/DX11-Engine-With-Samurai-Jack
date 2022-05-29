#include "stdafx.h"
#include "BillboardAnimation.h"
#include "Config.h"

void BillboardAnimation::Awake()
{
	GameObject* goRenderer = CreateGameObjectToChild(transform);
	m_renderer = goRenderer->AddComponent<BillboardRenderer>();
	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_BILLBOARD_ANIMATION);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_renderer->material = material;

	SetSliced(uint2(1, 1));
	SetIndex(uint2(0, 0));
}

void BillboardAnimation::Update()
{
	if (m_leftToIncrease <= 0.0f)
	{
		IncreaseIndex();
		m_leftToIncrease = m_autoDelay;
	}
	else
	{
		m_leftToIncrease -= system->time->deltaTime;
	}
}

void BillboardAnimation::SetSliced(uint2 sliced)
{
	m_renderer->material->SetUInt("_NumSlicedX", sliced.x);
	m_renderer->material->SetUInt("_NumSlicedY", sliced.y);
	m_sliced = sliced;
}

void BillboardAnimation::SetIndex(uint2 index)
{	
	index.x = Clamp(index.x, 0u, m_sliced.x);
	index.y = Clamp(index.y, 0u, m_sliced.y);

	m_renderer->material->SetUInt("_IndexX", index.x);
	m_renderer->material->SetUInt("_IndexY", index.y);

	m_index = index;
}

void BillboardAnimation::IncreaseIndex()
{
	uint x = m_index.x + 1;
	uint y = m_index.y;
	if (x >= m_sliced.x)
	{
		x = 0;
		++y;
	}
	if (y >= m_sliced.y)
	{
		x = 0;
		y = 0;
	}
	SetIndex(uint2(x, y));
}

void BillboardAnimation::SetColor(const Color& color)
{
	m_renderer->material->SetColor("_Color", color);
}

void BillboardAnimation::SetTexture(ResourceRef<Texture> texture)
{
	m_renderer->material->SetTexture("_Texture", texture);
}
