#include "stdafx.h"
#include "EnemyBeetleDrone.h"
#include "EnemyBeetleDroneAnimator.h"

void EnemyBeetleDrone::Awake()
{
	Enemy::Awake();
	SetupCharacterRenderers();
	SetupAnimator();

	CCT->radius = 0.8f;
	CCT->height = 0.675f;
	//CCT->height = 0.01f; // Bug mode
}

void EnemyBeetleDrone::Update()
{
	Enemy::Update();
}

void EnemyBeetleDrone::SetupCharacterRenderers()
{
	m_goCharacterRender = CreateGameObjectToChild(transform);
	m_goCharacterRender->transform->localPosition = V3(0, -1.0f, 0);
	m_goCharacterRender->transform->localEulerAngles = V3(90, 180, 0);
	m_characterRenderer = m_goCharacterRender->AddComponent<SkinnedMeshRenderer>();
	m_characterRenderer->mesh = system->resource->Find(TEXT("../Resource/BeetleDrone/BeetleDrone.FBX"));
	m_characterRenderer->SetupStandardMaterials();
}

void EnemyBeetleDrone::SetupAnimator()
{
	m_animator = m_goCharacterRender->AddComponent<EnemyBeetleDroneAnimator>();
}
