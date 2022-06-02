#include "stdafx.h"
#include "ParticleDust01.h"
#include "BillboardSpritesheet.h"
#include "Config.h"

void ParticleDust01::Awake()
{
	GameObject* goBillboard = CreateGameObjectToChild(transform);
	m_billboardSpritesheet = goBillboard->AddComponent<BillboardSpritesheet>();
	m_billboardSpritesheet->SetTexture(system->resource->Find(TEX_DUST_01));

	BillboardRenderer* billboardRenderer = m_billboardSpritesheet->GetBillboardRenderer();
	billboardRenderer->SetSelfLockFlags(0);

	uint ix = rand() % 8;
	uint iy = rand() % 8;
	float angle = float(rand() % 361);

	m_billboardSpritesheet->transform->localEulerAngles = V3(0.0f, 0.0f, angle);
	m_billboardSpritesheet->SetSliced(uint2(8, 8));
	m_billboardSpritesheet->SetIndex(uint2(ix, iy));
}
