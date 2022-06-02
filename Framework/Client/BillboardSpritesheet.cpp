#include "stdafx.h"
#include "BillboardSpritesheet.h"
#include "Config.h"

void BillboardSpritesheet::Awake()
{
	m_renderer = gameObject->AddComponent<BillboardRenderer>();
	ResourceRef<Shader> shader = system->resource->FindBinrayShader(SHADER_BILLBOARD_SPRITESHEET);
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_renderer->material = material;

	SetSliced(uint2(1, 1));
	SetIndex(uint2(0, 0));
}

void BillboardSpritesheet::Update()
{
}

void BillboardSpritesheet::SetSliced(uint2 sliced)
{
	m_renderer->material->SetUInt("_NumSlicedX", sliced.x);
	m_renderer->material->SetUInt("_NumSlicedY", sliced.y);
	m_sliced = sliced;
}

void BillboardSpritesheet::SetIndex(uint2 index)
{
	index.x = Clamp(index.x, 0u, m_sliced.x);
	index.y = Clamp(index.y, 0u, m_sliced.y);

	m_renderer->material->SetUInt("_IndexX", index.x);
	m_renderer->material->SetUInt("_IndexY", index.y);

	m_index = index;
}

void BillboardSpritesheet::SetColor(const Color& color)
{
	m_renderer->material->SetColor("_Color", color);
}

void BillboardSpritesheet::SetTexture(ResourceRef<Texture> texture)
{
	m_renderer->material->SetTexture("_Texture", texture);
}

void BillboardSpritesheet::SetMesh(ResourceRef<Mesh> mesh)
{
	ResourceRef<Material> material = m_renderer->material;
	m_renderer->mesh = mesh;
	m_renderer->material = material;
}
