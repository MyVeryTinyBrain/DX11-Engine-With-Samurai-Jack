#include "EnginePCH.h"
#include "AnimationChannel.h"

AnimationChannel::AnimationChannel(const TCHAR* boneName, uint boneIndex, vector<KeyframeDesc>&& keyframes)
{
	_tcscpy_s(m_boneName, boneName);

	m_boneIndex = boneIndex;

	m_keyframes = keyframes;

	m_minTime = 0;
	if (m_keyframes.front().time < 0.0f)
	{
		m_minTime = m_keyframes.front().time;
	}

	m_maxTime = m_keyframes.back().time;
}

AnimationChannel::~AnimationChannel()
{
}

void AnimationChannel::GetKeyframeTransform(float time, V3& out_position, Q& out_rotation, V3& out_scale) const
{
	// time 이후의 키프레임 인덱스를 이진탐색합니다.
	uint nextIndex = ClosetIndex(time);
	uint maxKeyframes = (uint)m_keyframes.size();
	nextIndex = Min(nextIndex, maxKeyframes - 1);
	uint index = nextIndex == 0 ? 0 : nextIndex - 1;

	// 두 키프레임 사이에 존재하는 time의 비율을 구합니다.
	float deltaTime = m_keyframes[nextIndex].time - m_keyframes[index].time;
	float ratio = (time - m_keyframes[index].time) / deltaTime;
	ratio = Clamp01(ratio);
	if (deltaTime == 0)
		ratio = 0;

	// 두 키프레임 트랜스폼을 비율로 보간해 반환합니다.
	out_position = V3::Lerp(m_keyframes[index].position, m_keyframes[nextIndex].position, ratio);
	out_rotation = Q::SLerp(m_keyframes[index].rotation, m_keyframes[nextIndex].rotation, ratio);
	out_scale = V3::Lerp(m_keyframes[index].scale, m_keyframes[nextIndex].scale, ratio);
}

uint AnimationChannel::ClosetIndex(float time) const
{
	auto it = std::lower_bound(m_keyframes.begin(), m_keyframes.end(), time,
		[](KeyframeDesc const& x, float d)
		{
			return x.time < d;
		});

	return (uint)std::distance(m_keyframes.begin(), it);
}
