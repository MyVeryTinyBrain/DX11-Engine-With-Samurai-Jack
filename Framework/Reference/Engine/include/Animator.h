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

	// Ʈ�����ǿ� ���� ��尡 ����Ǳ� �����ϸ� ȣ��˴ϴ�.
	// Ref<AnimatorLayer> Layer: �̺�Ʈ�� ������ ���̾��Դϴ�.
	// Ref<AnimatorNode> BeginChangingNode: ����Ƿ��� ����Դϴ�.
	delegate<void(Ref<AnimatorLayer>, Ref<AnimatorNode>)> OnBeginChanging;

	// Ʈ�����ǿ� ���� ��尡 ����Ǹ� ȣ��˴ϴ�.
	// // Ref<AnimatorLayer> Layer: �̺�Ʈ�� ������ ���̾��Դϴ�.
	// Ref<AnimatorNode> EndChangedNode: ����� ����Դϴ�.
	// Ref<AnimatorNode> PreviousNode: ����Ǳ� ������ ����Դϴ�. (nullptr�ϼ� �ֽ��ϴ�.)
	delegate<void(Ref<AnimatorLayer>, Ref<AnimatorNode>, Ref<AnimatorNode>)> OnEndChanged;

	// �ִϸ��̼� �̺�Ʈ�� �߻��ϸ� ȣ��˴ϴ�.
	// Ref<AnimatorLayer> Layer: �̺�Ʈ�� ������ ���̾��Դϴ�.
	// const string& Message: �̺�Ʈ �޽����Դϴ�.
	delegate<void(Ref<AnimatorLayer>, const AnimationEventDesc&)> OnAnimationEvent;
};

ENGINE_END
