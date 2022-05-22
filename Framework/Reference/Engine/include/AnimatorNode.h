#pragma once

#include "Object.h"
#include "IAnimatorNode.h"
#include "AnimationClip.h"
#include "AnimatorType.h"
#include "Ref.h"
#include "AnimationEventType.h"
#include <string>

ENGINE_BEGIN

class ENGINE_API AnimatorNode abstract : public Object, public IAnimatorNode
{
	friend class AnimatorSingleNode;
	friend class AnimatorBlendNode;

public:

	AnimatorNode(const tstring& name, bool loop);

public:

	void AddEvent(float noramlizedTime, const string& context);

public:

	void Accumulate(float deltaTime, vector<string>& out_eventNotifications);

	bool Animate(
		uint channelIndex, uint& out_nodeIndex, 
		V3& out_position, Q& out_rotation, V3& out_scale, 
		V3& out_deltaPosition, Q& out_deltaRotation);

public:

	inline float GetDuration() const { return GetDurationImpl(); }

	inline bool IsLoop() const { return m_isLoop; }

	inline float GetNormalizedTime() const { return m_normalizedTime; }
	void SetNormalizedTime(float value);

	inline float SetSpeed() const { return m_speed; }
	inline void SetSpeed(float value) { m_speed = Max(0.0f, value); }

	_declspec(property(get = GetDuration)) float duration;
	_declspec(property(get = IsLoop)) bool isLoop;
	_declspec(property(get = GetNormalizedTime, put = SetNormalizedTime)) float normalizedTime;
	_declspec(property(get = SetSpeed, put = SetSpeed)) float speed;

protected:

	virtual bool AnimateNodeImpl(
		uint channelIndex, 
		uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale, 
		V3& out_delatPosition, Q& out_deltaRotation) = 0;
	virtual float GetDurationImpl() const = 0;
	virtual void SetRootNode(const Ref<NodeTransform>& rootNode) override = 0;

private:

	virtual void OnTransition() override;
	virtual void OnLooped();

	void SortEvents();
	void ClearEventUsed();
	void EventProcess(vector<string>& out_notifications);

protected:

	float					m_dt = 0.0f;

	float					m_speed = 1.0f;

	bool					m_isLoop = false;

	float					m_normalizedTime = 0;

	Ref<NodeTransform>		m_rootNode;

	// NormalizedTime이 오름차순으로 정렬된 이벤트 벡터입니다.
	vector<AnimationEvent>	m_events;
};

ENGINE_END
