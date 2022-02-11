#include "EnginePCH.h"
#include "AnimationClip.h"

AnimationClip::AnimationClip(
	ResourceObject* basedResourceObject, 
	const Animation* animation) :
	SubResourceObject(animation->name, basedResourceObject),
	m_animation(animation)
{
}

AnimationClip::~AnimationClip()
{
}

const Channel* AnimationClip::GetChannel(uint index) const
{
	if (!m_animation)
		return nullptr;

	return m_animation->GetChannel(index);
}

size_t AnimationClip::GetChannelCount() const
{
	if (!m_animation)
		return 0;

	return m_animation->GetChannelCount();
}

float AnimationClip::GetDuration() const
{
	if (!m_animation)
		return 0.0f;

	return m_animation->duration;
}

float AnimationClip::GetTickPerSecond() const
{
	if (!m_animation)
		return 0.0f;

	return m_animation->tickPerSecond;
}

float AnimationClip::GetRealtimeDuration() const
{
	if (!m_animation)
		return 0.0f;

	return m_animation->realtimeDuration;
}
