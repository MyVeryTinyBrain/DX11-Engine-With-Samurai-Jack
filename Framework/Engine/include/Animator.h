#pragma once

#include "Component.h"
#include "Ref.h"
#include "AnimatorLayer.h"
#include "AnimationEventType.h"

ENGINE_BEGIN

class SkinnedMeshRenderer;

class ENGINE_API Animator : public Component
{
public:

	enum class UpdateMode { DeltaTime, UnscaledDeltaTime };

protected:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void AnimationUpdate() override;
	virtual void LateUpdate() override;
	virtual void PostUpdate() override;
	virtual void OnDestroyed() override;

public:

	void AddLayer(AnimatorLayer* layer);
	Ref<AnimatorLayer> GetLayerByIndex(uint index) const;
	Ref<AnimatorLayer> GetLayerByName(const tstring& name) const;

	void ForceUpdate();

public:

	inline float GetSpeed() const { return m_speed; }
	inline void SetSpeed(float value) { m_speed = Max(0.0f, value); }

	inline bool IsPaused() const { return m_pause; }
	void SetPause(bool value);

	inline Animator::UpdateMode GetUpdateMode() const { return m_updateMode; }
	inline void SetUpdateMode(Animator::UpdateMode value) { m_updateMode = value; }

	_declspec(property(get = GetSpeed, put = SetSpeed)) float speed;
	_declspec(property(get = IsPaused, put = SetPuase)) bool pause;
	_declspec(property(get = GetUpdateMode, put = SetUpdateMode)) Animator::UpdateMode updateMode;

private:

	void ReAttachSkinnedMeshRenderer();

private:

	Ref<SkinnedMeshRenderer>			m_skinnedMeshRenderer;

	vector<AnimatorLayer*>				m_layers;

	float								m_speed = 1.0f;
	bool								m_pause = false;

	UpdateMode							m_updateMode = Animator::UpdateMode::DeltaTime;

public:

	// 트랜지션에 의해 노드가 변경되기 시작하면 호출됩니다.
	// Ref<AnimatorLayer> Layer: 이벤트가 생성된 레이어입니다.
	// Ref<AnimatorNode> BeginChangingNode: 변경되려는 노드입니다.
	delegate<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)> OnBeginChanging;

	// 트랜지션에 의해 노드가 변경되면 호출됩니다.
	// // Ref<AnimatorLayer> Layer: 이벤트가 생성된 레이어입니다.
	// Ref<AnimatorNode> EndChangedNode: 변경된 노드입니다.
	// Ref<AnimatorNode> PreviousNode: 변경되기 이전의 노드입니다. (nullptr일수 있습니다.)
	delegate<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)> OnEndChanged;

	// 애니메이션 이벤트가 발생하면 호출됩니다.
	// Ref<AnimatorLayer> Layer: 이벤트가 생성된 레이어입니다.
	// const string& Message: 이벤트 메시지입니다.
	delegate<void(Ref<AnimatorLayer>, const AnimationEventDesc&)> OnAnimationEvent;
};

ENGINE_END
