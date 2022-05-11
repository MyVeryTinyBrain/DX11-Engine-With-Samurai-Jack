#pragma once

#include "Object.h"
#include "IAnimatorNode.h"
#include "AnimationClip.h"
#include "AnimatorType.h"
#include "Ref.h"

ENGINE_BEGIN

class ENGINE_API AnimatorNode abstract : public Object, public IAnimatorNode
{
	friend class AnimatorSingleNode;
	friend class AnimatorBlendNode;

public:

	AnimatorNode(const tstring& name, bool loop);

public:

	void Accumulate(float deltaTime);

	bool Animate(
		uint channelIndex, uint& out_nodeIndex, 
		V3& out_position, Q& out_rotation, V3& out_scale, 
		V3& out_deltaPosition, Q& out_deltaRotation);

public:

	inline float GetDuration() const { return GetDurationImpl(); }

	inline bool IsLoop() const { return m_isLoop; }

	inline float GetNormalizedTime() const { return m_normalizedTime; }

	void SetNormalizedTime(float value);

	_declspec(property(get = GetDuration)) float duration;
	_declspec(property(get = IsLoop)) bool isLoop;
	_declspec(property(get = GetNormalizedTime, put = SetNormalizedTime)) float normalizedTime;

protected:

	virtual bool AnimateNodeImpl(
		uint channelIndex, 
		uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale, 
		V3& out_delatPosition, Q& out_deltaRotation) = 0;

	virtual float GetDurationImpl() const = 0;

	virtual void SetRootNode(const Ref<NodeTransform>& rootNode) = 0;

protected:

	float							m_dt = 0.0f;

	bool							m_isLoop = false;

	float							m_normalizedTime = 0;

	Ref<NodeTransform>				m_rootNode;
};

ENGINE_END
