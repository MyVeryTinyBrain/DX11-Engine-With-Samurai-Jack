#include "stdafx.h"
#include "EnemyBeetleDroneAnimator.h"

#define LOOP		true
#define NOLOOP		false
#define EXIT		nullptr
#define ANY			nullptr

void EnemyBeetleDroneAnimator::Awake()
{
    Animator::Awake();
	SetupMesh();
	SetupLayer();
	SetupProperties();
	SetupNodes();
	SetupTransitions();
	speed = 1.2f;
}

SubResourceRef<AnimationClip> EnemyBeetleDroneAnimator::GetClip(const tstring& name)
{
    return Mesh->GetAnimationClipByName(TEXT("ROOT|") + name);
}

void EnemyBeetleDroneAnimator::SetupMesh()
{
	Mesh = system->resource->Find(TEXT("../Resource/BeetleDrone/BeetleDrone.FBX"));
}

void EnemyBeetleDroneAnimator::SetupLayer()
{
	Layer = new AnimatorLayer;
	AddLayer(Layer);
	Layer->SetRootNodeByName(TEXT("CharacterRoot"));
}

void EnemyBeetleDroneAnimator::SetupProperties()
{
}

void EnemyBeetleDroneAnimator::SetupNodes()
{
	BH_STD_IDLE = AnimatorSingleNode::Create(GetClip(TEXT("BH_STD_IDLE")), LOOP);
	Layer->AddNode(BH_STD_IDLE);
}

void EnemyBeetleDroneAnimator::SetupTransitions()
{
}
