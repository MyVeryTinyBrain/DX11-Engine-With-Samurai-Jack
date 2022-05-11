#pragma once

#include "KeyframeDesc.h"

ENGINE_BEGIN

class ENGINE_API AnimationChannel
{
public:

	AnimationChannel(const TCHAR* boneName, uint boneIndex, vector<KeyframeDesc>&& keyframes);

	~AnimationChannel();

public:

	inline const TCHAR* GetBoneName() const { return m_boneName; }

	inline uint GetBoneIndex() const { return m_boneIndex; }

	inline float GetMinTime() const { return m_minTime; }

	inline float GetMaxTime() const { return m_maxTime; }

	_declspec(property(get = GetBoneName)) const TCHAR* boneName;
	_declspec(property(get = GetBoneIndex)) uint boneIndex;
	_declspec(property(get = GetMinTime)) float minTime;
	_declspec(property(get = GetMaxTime)) float maxTime;

public:

	void GetKeyframeTransform(float time, V3& out_position, Q& out_rotation, V3& out_scale) const;

private:

	uint ClosetIndex(float time) const;

private:

	TCHAR m_boneName[MAXLEN];

	uint m_boneIndex;

	float m_minTime = 0;

	float m_maxTime = 0;

	vector<KeyframeDesc> m_keyframes;
};

ENGINE_END
