#include "stdafx.h"
#include "EffectExplosion.h"
#include "BillboardAnimation.h"
#include "Config.h"

void EffectExplosion::Awake()
{
	m_billboardAnimation = gameObject->AddComponent<BillboardAnimation>();
	m_billboardAnimation->SetTexture(system->resource->Find(TEX_ANIMATION_EXPLOSION));
	m_billboardAnimation->SetSliced(uint2(4, 4));
	m_billboardAnimation->SetColor(Color::RGBA255(255, 255, 255, 255));

	m_billboardAnimation->OnLooped += func<void()>(this, &EffectExplosion::OnLooped);
}

void EffectExplosion::OnLooped()
{
	gameObject->Destroy();
}
