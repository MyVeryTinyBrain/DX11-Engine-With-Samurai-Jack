#pragma once

#include "SubResourceObject.h"
#include "Animation.h"

ENGINE_BEGIN

class ENGINE_API AnimationClip : public SubResourceObject
{
public:

	AnimationClip(
		ResourceObject* basedResourceObject, 
		const Animation* animation);

	virtual ~AnimationClip();

public:

	const Animation* GetAnimation() const { return m_animation; }

	const AnimationChannel* GetChannel(uint index) const;

	uint GetChannelCount() const;

	float GetDuration() const;

	float GetTickPerSecond() const;

	float GetRealtimeDuration() const;

	_declspec(property(get = GetAnimation)) const Animation* animation;
	_declspec(property(get = GetChannelCount)) uint channelCount;
	_declspec(property(get = GetDuration)) float duration;
	_declspec(property(get = GetTickPerSecond)) float tickPerSecond;
	_declspec(property(get = GetRealtimeDuration)) float realtimeDuration;

private:

	const Animation* m_animation = nullptr;

};

ENGINE_END
