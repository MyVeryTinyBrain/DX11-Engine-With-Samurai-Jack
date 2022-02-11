#include "EnginePCH.h"
#include "AnimationSet.h"

AnimationSet::AnimationSet(vector<Animation*>&& animations) :
	m_animationsByIndex(animations)
{
	for (auto& animation : m_animationsByIndex)
	{
		bool result = m_animationsByName.emplace(animation->name, animation).second;
	}
}

AnimationSet::~AnimationSet()
{
	for (auto& animation : m_animationsByIndex)
	{
		SafeDelete(animation);
	}
	m_animationsByIndex.clear();
}

const Animation* AnimationSet::GetAnimationByName(const tstring& name) const
{
	auto find_it = m_animationsByName.find(name);
	if (find_it == m_animationsByName.end())
		return nullptr;

	return find_it->second;
}
