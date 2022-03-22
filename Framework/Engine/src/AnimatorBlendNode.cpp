#include "EnginePCH.h"
#include "AnimatorBlendNode.h"

AnimatorBlendNode::AnimatorBlendNode(const tstring& name, const vector<Ref<AnimatorBlendNodeElement>>& elements, AnimatorProperty* floatProperty, bool loop) :
	AnimatorNode(name, loop),
	m_elements(elements),
	m_positionProperty(floatProperty)
{
	m_lastPosition = m_positionProperty->valueAsFloat;

	m_minPosition = 0;
	m_maxPosition = 0;

	m_indexA = 0;
	m_indexB = 0;
	m_weightA = 0;
	m_weightB = 0;
	m_timescaleA = 0;
	m_timescaleB = 0;
	m_duration = 0;
}

Ref<AnimatorBlendNode> AnimatorBlendNode::Create(const tstring& name, const vector<Ref<AnimatorBlendNodeElement>>& elements, AnimatorProperty* floatProperty, bool loop)
{
	if (!floatProperty)
		return nullptr;
		
	AnimatorBlendNode* node = new AnimatorBlendNode(name, elements, floatProperty, loop);
	
	if (!node->Initialize())
	{
		SafeDelete(node);
		return nullptr;
	}

	return node;
}

AnimatorBlendNode::~AnimatorBlendNode()
{
	for (auto& element : m_elements)
	{
		AnimatorBlendNodeElement* ptr = element.GetPointer();
		SafeDelete(ptr);
		element = nullptr;
	}
	m_elements.clear();
}

bool AnimatorBlendNode::Initialize()
{
	if (m_elements.size() == 0)
		return false;

	m_minPosition = FLT_MAX;
	m_maxPosition = FLT_MIN;

	if (!m_elements.empty())
	{
		m_minPosition = m_elements.front()->position;
		m_maxPosition = m_elements.front()->position;
	}

	for (auto& elements : m_elements)
	{
		m_minPosition = Min(elements->position, m_minPosition);
		m_maxPosition = Max(elements->position, m_maxPosition);
	}

	return CalculateValues();
}

bool AnimatorBlendNode::CalculateValues()
{
	if (!CalculateClosetIndices(m_positionProperty->valueAsFloat, m_indexA, m_indexB))
		return false;

	CalculateBlendWeights(m_positionProperty->valueAsFloat, m_indexA, m_indexB, m_weightA, m_weightB);
	CalculateTimeScales(m_indexA, m_indexB, m_weightA, m_weightB, m_timescaleA, m_timescaleB);
	m_duration = CalculateDuration(m_indexA, m_indexB, m_weightA, m_weightB);

	return true;
}

bool AnimatorBlendNode::CalculateClosetIndices(float position, uint& out_aIndex, uint& out_bIndex) const
{
	if (m_elements.size() == 0)
		return false;

	auto it = std::lower_bound(m_elements.begin(), m_elements.end(), position,
		[](const Ref<AnimatorBlendNodeElement>& x, float d)
		{
			return x->position < d;
		});

	uint bIndex = (uint)std::distance(m_elements.begin(), it);
	//bIndex = Clamp(bIndex, 0u, uint(m_elements.size() - 1));
	bIndex = Min(bIndex, uint(m_elements.size() - 1));

	uint aIndex = bIndex == 0 ? 1 : bIndex - 1;
	//aIndex = Clamp(aIndex, 0u, uint(m_elements.size() - 1));
	aIndex = Min(aIndex, uint(m_elements.size() - 1));

	out_aIndex = aIndex;
	out_bIndex = bIndex;

	return true;
}

void AnimatorBlendNode::CalculateBlendWeights(float position, uint aIndex, uint bIndex, float& out_aWeight, float& out_bWeight) const
{
	float aDistance = Abs(position - m_elements[aIndex]->position);
	float bDistance = Abs(m_elements[bIndex]->position - position);
	float sumDistance = aDistance + bDistance;

	float ratioA = aDistance / sumDistance;
	float ratioB = bDistance / sumDistance;

	float weightA = ratioB;
	float weightB = ratioA;

	if (aIndex == bIndex)
	{
		weightA = 1.0f;
		weightB = 0.0f;
	}

	out_aWeight = weightA;
	out_bWeight = weightB;
}

void AnimatorBlendNode::CalculateTimeScales(uint aIndex, uint bIndex, float aWeight, float bWeight, float& out_aScale, float& out_bScale) const
{
	float aDuration = m_elements[aIndex]->node->duration * aWeight; 
	float bDuration = m_elements[bIndex]->node->duration * bWeight;
	float sumDuration = aDuration + bDuration; 

	float ratioA = aDuration / sumDuration;
	float ratioB = bDuration / sumDuration;

	float scaleA = ratioA;
	float scaleB = ratioB;

	if (aWeight != 0)
		scaleA /= aWeight;
	if (bWeight != 0)
		scaleB /= bWeight;

	out_aScale = scaleA;
	out_bScale = scaleB;
}

float AnimatorBlendNode::CalculateDuration(uint aIndex, uint bIndex, float aWeight, float bWeight) const
{
	float a = m_elements[aIndex]->node->duration * aWeight;
	float b = m_elements[bIndex]->node->duration * bWeight;
	float result = a + b;
	return result;
}

bool AnimatorBlendNode::AnimateNodeImpl(
	uint channelIndex,
	uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale,
	V3& out_deltaPosition, Q& out_deltaRotation)
{
	if (m_elements.size() == 0)
		return false;

	if (m_lastPosition != m_positionProperty->valueAsFloat)
	{
		CalculateValues();
		m_lastPosition = m_positionProperty->valueAsFloat;
	}

	if (m_weightA * m_weightB == 0)
	{
		uint index = m_weightA > m_weightB ? m_indexA : m_indexB;
		float timeScale = m_weightA > m_weightB ? m_timescaleA : m_timescaleB;
		m_elements[index]->node->m_normalizedTime = m_normalizedTime * timeScale;
		m_elements[index]->node->m_dt = m_dt;
		return m_elements[index]->node->AnimateNodeImpl(
			channelIndex, out_nodeIndex,
			out_position, out_rotation, out_scale, 
			out_deltaPosition, out_deltaRotation);
	}
	else
	{
		m_elements[m_indexA]->node->m_normalizedTime = m_normalizedTime * m_timescaleA;
		m_elements[m_indexA]->node->m_dt = m_dt;
		m_elements[m_indexB]->node->m_normalizedTime = m_normalizedTime * m_timescaleB;
		m_elements[m_indexB]->node->m_dt = m_dt;

		uint nodeIndex;
		V3   position[2];
		Q    rotation[2];
		V3   scale[2];
		V3   deltaPosition[2];
		Q    deltaRotation[2];
		if (!m_elements[m_indexA]->node->AnimateNodeImpl(
			channelIndex, nodeIndex,
			position[0], rotation[0], scale[0], 
			deltaPosition[0], deltaRotation[0]))
			return false;
		if (!m_elements[m_indexB]->node->AnimateNodeImpl(
			channelIndex, nodeIndex,
			position[1], rotation[1], scale[1],
			deltaPosition[1], deltaRotation[1]))
			return false;

		//float deltaWeightB = m_weightB;
		float deltaWeightB = m_weightB * m_weightB;

		out_nodeIndex = nodeIndex;
		out_position = V3::Lerp(position[0], position[1], m_weightB);
		out_rotation = Q::SLerp(rotation[0], rotation[1], m_weightB);
		out_scale = V3::Lerp(scale[0], scale[1], m_weightB);
		out_deltaPosition = V3::Lerp(deltaPosition[0], deltaPosition[1], deltaWeightB);
		out_deltaRotation = Q::SLerp(deltaRotation[0], deltaRotation[1], deltaWeightB);
	}

	return true;
}

float AnimatorBlendNode::GetDurationImpl() const
{
	return m_duration;
}

void AnimatorBlendNode::SetRootNode(const Ref<NodeTransform>& rootNode)
{
	m_rootNode = rootNode;

	for (auto& element : m_elements)
	{
		element->node->SetRootNode(rootNode);
	}
}
