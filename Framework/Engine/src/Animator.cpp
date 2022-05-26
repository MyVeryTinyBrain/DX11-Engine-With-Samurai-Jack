#include "EnginePCH.h"
#include "Animator.h"
#include "SkinnedMeshRenderer.h"
#include "GameObject.h"
#include "NodeTransform.h"
#include "System.h"
#include "Times.h"
#include "Ref.h"

void Animator::Awake()
{
	ReAttachSkinnedMeshRenderer();
}

void Animator::Start()
{
	ReAttachSkinnedMeshRenderer();
}

void Animator::AnimationUpdate()
{
	ReAttachSkinnedMeshRenderer();

	if (m_pause)
		return;

	ForceUpdate();
}

void Animator::LateUpdate()
{
	for (auto& layer : m_layers)
	{
		const Ref<AnimatorNode>& beginChangineNode = layer->GetBeginChangingNode();
		const Ref<AnimatorNode>& endChangedNode = layer->GetEndChangedNode();
		const Ref<AnimatorNode>& previousNode = layer->GetPreviousNode();

		// 트랜지션 이벤트 알림

		if (beginChangineNode)
		{
			OnBeginChanging(layer, beginChangineNode);
		}
		if (endChangedNode)
		{
			OnEndChanged(layer, endChangedNode, previousNode);
		}

		// 애니메이션 이벤트 알림

		for (auto& eventDesc : layer->GetEventDescs())
		{
			OnAnimationEvent(layer, eventDesc);
		}
	}
}

void Animator::PostUpdate()
{
	for (auto& layer : m_layers)
	{
		IAnimatorLayer* iAnimatorLayer = layer;
		iAnimatorLayer->ClearTransitionEvents();
		iAnimatorLayer->ClearAnimationEvents();
	}
}

void Animator::OnDestroyed()
{
	for (auto& layer : m_layers)
		SafeDelete(layer);
	m_layers.clear();
}

void Animator::AddLayer(AnimatorLayer* layer)
{
	if (!layer)
		return;

	IAnimatorLayer* iAnimatorLayer = layer;
	iAnimatorLayer->SetSkinnedMeshRenderer(m_skinnedMeshRenderer);
	iAnimatorLayer->SetAnimator(this);

	m_layers.push_back(layer);
}

Ref<AnimatorLayer> Animator::GetLayerByIndex(uint index) const
{
	if (index >= m_layers.size())
		return nullptr;
	return m_layers[index];
}

Ref<AnimatorLayer> Animator::GetLayerByName(const tstring& name) const
{
	auto find_it = std::find_if(m_layers.begin(), m_layers.end(), [&](const AnimatorLayer* layer)
		{
			return layer->name == name;
		});
	if (find_it == m_layers.end())
		return nullptr;
	return *find_it;
}

void Animator::ForceUpdate()
{
	if (!m_skinnedMeshRenderer)
		return;

	if (m_skinnedMeshRenderer && !m_skinnedMeshRenderer->mesh)
		return;

	float dt = 0.0f;
	
	switch (m_updateMode)
	{
		case UpdateMode::DeltaTime:
			dt = system->time->deltaTime;
			break;
		case UpdateMode::UnscaledDeltaTime:
			dt = system->time->unscaledDeltaTime;
			break;
	}

	for (auto& layer : m_layers)
	{
		layer->Accumulate(dt, m_speed);
	}

	for (auto& layer : m_layers)
	{
		layer->Animate(m_skinnedMeshRenderer);
	}
}

void Animator::SetPause(bool value)
{
	if (m_pause == value)
		return;

	m_pause = value;
	ForceUpdate();
}

void Animator::ReAttachSkinnedMeshRenderer()
{
	if (m_skinnedMeshRenderer)
		return;

	m_skinnedMeshRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();
}
