#pragma once

#include "Component.h"
#include "Ref.h"
#include "AnimatorLayer.h"

ENGINE_BEGIN
class SkinnedMeshRenderer;
class ENGINE_API Animator : public Component
{
protected:

	virtual void Awake() override;

	virtual void Start() override;

	virtual void AnimationUpdate() override;

	virtual void OnDestroyed() override;

public:

	void AddLayer(AnimatorLayer* layer);

	Ref<AnimatorLayer> GetLayerByIndex(uint index) const;

	Ref<AnimatorLayer> GetLayerByName(const tstring& name) const;

	void ForceUpdate();

public:

	inline float GetSpeed() const { return m_speed; }

	inline void SetSpeed(float value) { m_speed = Clamp01(value); }

	inline bool IsPaused() const { return m_pause; }

	void SetPause(bool value);

	_declspec(property(get = GetSpeed, put = SetSpeed)) float speed;
	_declspec(property(get = IsPaused, put = SetPuase)) bool pause;

private:

	void ReAttachSkinnedMeshRenderer();

private:

	Ref<SkinnedMeshRenderer>			m_skinnedMeshRenderer;

	vector<AnimatorLayer*>				m_layers;

	float								m_speed = 1.0f;

	bool								m_pause = false;
};

ENGINE_END
