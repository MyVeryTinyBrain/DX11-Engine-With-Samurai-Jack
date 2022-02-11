#pragma once

#include "AnimatorNode.h"
#include "AnimatorBlendNodeElement.h"
#include "AnimatorProperty.h"

ENGINE_BEGIN

class ENGINE_API AnimatorBlendNode : public AnimatorNode
{
private:

	AnimatorBlendNode(const tstring& name, const vector<Ref<AnimatorBlendNodeElement>>& elements, AnimatorProperty* floatProperty, bool loop);

public:

	static Ref<AnimatorBlendNode> Create(const tstring& name, const vector<Ref<AnimatorBlendNodeElement>>& elements, AnimatorProperty* floatProperty, bool loop);

	virtual ~AnimatorBlendNode();

public:

	inline float GetMinPosition() const { return m_minPosition; }

	inline float GetMaxPosition() const { return m_maxPosition; }

	_declspec(property(get = GetMinPosition)) float minPosition;
	_declspec(property(get = GetMaxPosition)) float maxPosition;

private:

	bool Initialize();

	bool CalculateValues();

	// Step 1.
	// Calculate two indices which are closest to position
	bool CalculateClosetIndices(float position, uint& out_aIndex, uint& out_bIndex) const;

	// Step 2.
	// Calculate two weights by position between two indices
	void CalculateBlendWeights(float position, uint aIndex, uint bIndex, float& out_aWeight, float& out_bWeight) const;

	// Step 3.
	// Calculate two timescales which are using to calculate transform by weights of indices
	void CalculateTimeScales(uint aIndex, uint bIndex, float aWeight, float bWeight, float& out_aScale, float& out_bScale) const;

	// Step 4.
	float CalculateDuration(uint aIndex, uint bIndex, float aWeight, float bWeight) const;

private:

	bool AnimateNodeImpl(
		uint channelIndex,
		uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale,
		V3& out_deltaPosition, Q& out_deltaRotation) override;

	float GetDurationImpl() const override;

	virtual void SetRootNode(const Ref<NodeTransform>& rootNode) override;

private:

	vector<Ref<AnimatorBlendNodeElement>>	m_elements;

	AnimatorProperty*						m_positionProperty = nullptr;
	float									m_lastPosition = 0;

	float									m_minPosition;
	float									m_maxPosition;

	uint									m_indexA;
	uint									m_indexB;
	float									m_weightA;
	float									m_weightB;
	float									m_timescaleA;
	float									m_timescaleB;
	float									m_duration;
};

ENGINE_END
