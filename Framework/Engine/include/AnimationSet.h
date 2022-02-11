#pragma once

#include "Animation.h"

ENGINE_BEGIN

class ENGINE_API AnimationSet
{
public:

	AnimationSet() = default;

	AnimationSet(vector<Animation*>&& animations);

	~AnimationSet();

public:

	inline size_t GetAnimationCount() const { return m_animationsByIndex.size(); }

	inline const Animation* GetAnimationByIndex(uint index) const { return m_animationsByIndex[index]; }

	const Animation* GetAnimationByName(const tstring& name) const;

	_declspec(property(get = GetAnimationCount)) size_t animationCount;

private:

	vector<Animation*> m_animationsByIndex;

	unordered_map<tstring, Animation*> m_animationsByName;
};

ENGINE_END
