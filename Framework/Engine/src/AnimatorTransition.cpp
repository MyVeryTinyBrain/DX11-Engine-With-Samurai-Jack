#include "EnginePCH.h"
#include "AnimatorTransition.h"
#include "AnimatorTransitionCallback.h"

AnimatorTransition::AnimatorTransition(
	AnimatorNode* startNode, AnimatorNode* nextNode,
	const vector<PropertyValue>& propertyValues,
	float exitTime, float duration, float offset,
	AnimatorTransition::Interrupt interrupt, bool noRecursive) :
	Object((startNode ? startNode->name : TEXT("AnyNode")) + TEXT("->") + (nextNode ? nextNode->name : TEXT("ExitNode"))),
	m_startNode(startNode),
	m_nextNode(nextNode),
	m_propertyValues(propertyValues),
	m_exitTime(exitTime),
	m_duration(duration),
	m_offset(offset),
	m_interrupt(interrupt),
	m_noRecursive(noRecursive)
{
	for (auto& propValue : m_propertyValues)
	{
		if (propValue.property->type == AnimatorProperty::Type::TRIGGER)
		{
			m_propertyTriggers.push_back(propValue.property);
		}
	}
}

AnimatorTransition::~AnimatorTransition()
{
}

void AnimatorTransition::SetCallback(AnimatorTransitionCallback* callback)
{
	m_callback = callback;
}

void AnimatorTransition::Used()
{
	for (auto& trigger : m_propertyTriggers)
		trigger->valueAsBool = false;
}

bool AnimatorTransition::IsTransferable(
	Animator* animator, AnimatorLayer* layer, 
	AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition, 
	StartNode& out_startNode) const
{
	out_startNode = StartNode::Current;

	if (m_callback)
	{
		if (!m_callback->Transferable(animator, layer, this, currentNode, blendingNode, currentTransition))
			return false;
	}

	if (currentTransition)
	{
		switch (m_interrupt)
		{
			case AnimatorTransition::Interrupt::None:
			{
				return false;
			}
			break;
			case AnimatorTransition::Interrupt::Current:
			{
				if (m_startNode && currentNode != m_startNode)
					return false;
				else
					out_startNode = StartNode::Current;
			}
			break;
			case AnimatorTransition::Interrupt::Next:
			{
				if (m_startNode && blendingNode != m_startNode)
					return false;
				else
					out_startNode = StartNode::Next;
			}
			break;
			case AnimatorTransition::Interrupt::CurrentNext:
			{
				if (m_startNode)
				{
					if (currentNode == m_startNode)
					{
						out_startNode = StartNode::Current;
						break;
					}
					else if (blendingNode == m_startNode)
					{
						out_startNode = StartNode::Next;
						break;
					}
					else
					{
						return false;
					}
				}
			}
			break;
		}
	}

	if (m_noRecursive)
	{
		if (currentNode == m_nextNode)
			return false;
		if (blendingNode == m_nextNode)
			return false;
	}

	if (m_exitTime > 0)
	{
		if (m_startNode && m_startNode->normalizedTime < m_exitTime)
			return false;

		if (!m_startNode && currentNode && currentNode->normalizedTime < m_exitTime)
			return false;
	}

	for (auto& propValue : m_propertyValues)
	{
		if (!CompareProperties(propValue.property, propValue.value, propValue.property->type, propValue.compare))
			return false;
	}

	return true;
}

bool AnimatorTransition::CompareProperties(AnimatorProperty* property, AnimatorProperty::Value value, AnimatorProperty::Type type, AnimatorTransition::Compare comp) const
{
	switch (type)
	{
		case Engine::AnimatorProperty::Type::TRIGGER:
			return property->valueAsBool == true;
		case Engine::AnimatorProperty::Type::BOOL:
			switch (comp)
			{
				case Engine::AnimatorTransition::Compare::EQUAL:
					return property->valueAsBool == value.valueBool;
				case Engine::AnimatorTransition::Compare::NOT_EQUAL:
					return property->valueAsBool != value.valueBool;
				case Engine::AnimatorTransition::Compare::LESS:
					return property->valueAsBool < value.valueBool;
				case Engine::AnimatorTransition::Compare::GREATER:
					return property->valueAsBool > value.valueBool;
				case Engine::AnimatorTransition::Compare::LESS_EQAUL:
					return property->valueAsBool <= value.valueBool;
				case Engine::AnimatorTransition::Compare::GREATER_EQUAL:
					return property->valueAsBool >= value.valueBool;
				default:
					return false;
			}
			break;
		case Engine::AnimatorProperty::Type::INT:
			switch (comp)
			{
				case Engine::AnimatorTransition::Compare::EQUAL:
					return property->valueAsInt == value.valueInt;
				case Engine::AnimatorTransition::Compare::NOT_EQUAL:
					return property->valueAsInt != value.valueInt;
				case Engine::AnimatorTransition::Compare::LESS:
					return property->valueAsInt < value.valueInt;
				case Engine::AnimatorTransition::Compare::GREATER:
					return property->valueAsInt > value.valueInt;
				case Engine::AnimatorTransition::Compare::LESS_EQAUL:
					return property->valueAsInt <= value.valueInt;
				case Engine::AnimatorTransition::Compare::GREATER_EQUAL:
					return property->valueAsInt >= value.valueInt;
				default:
					return false;
			}
			break;
		case Engine::AnimatorProperty::Type::FLOAT:
			switch (comp)
			{
				case Engine::AnimatorTransition::Compare::EQUAL:
					return property->valueAsFloat == value.valueFloat;
				case Engine::AnimatorTransition::Compare::NOT_EQUAL:
					return property->valueAsFloat != value.valueFloat;
				case Engine::AnimatorTransition::Compare::LESS:
					return property->valueAsFloat < value.valueFloat;
				case Engine::AnimatorTransition::Compare::GREATER:
					return property->valueAsFloat > value.valueFloat;
				case Engine::AnimatorTransition::Compare::LESS_EQAUL:
					return property->valueAsFloat <= value.valueFloat;
				case Engine::AnimatorTransition::Compare::GREATER_EQUAL:
					return property->valueAsFloat >= value.valueFloat;
				default:
					return false;
			}
			break;
		default:
			return false;
	}

	return false;
}
