#pragma once

#include "Object.h"
#include "Ref.h"
#include "AnimatorNode.h"
#include "AnimatorProperty.h"

ENGINE_BEGIN

class Animator;
class AnimatorLayer;
class AnimatorTransitionCallback;

/// <summary>
/// 애니메이션 노드간의 전환을 표현하는 객체입니다.<para>
/// </para>
/// </summary>
class ENGINE_API AnimatorTransition : public Object
{
public:

	enum class Interrupt { None, Current, Next, CurrentNext };

	enum class Compare { EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQAUL, GREATER_EQUAL };

	struct PropertyValue
	{
		inline static AnimatorTransition::PropertyValue Trigger(AnimatorProperty* Property) { return AnimatorTransition::PropertyValue(Property, true, AnimatorTransition::Compare::EQUAL); }
		inline AnimatorTransition::PropertyValue(AnimatorProperty* Property, bool Value, Compare comp) : property(Property), value(Value), compare(comp) {}
		inline AnimatorTransition::PropertyValue(AnimatorProperty* Property, int Value, Compare comp) : property(Property), value(Value), compare(comp) {}
		inline AnimatorTransition::PropertyValue(AnimatorProperty* Property, float Value, Compare comp) : property(Property), value(Value), compare(comp) {}
		AnimatorProperty*				property;
		AnimatorProperty::Value			value;		// Type::Trigger -> always true of bool
		AnimatorTransition::Compare		compare;	// Type::Trigger -> always equals to true
	};

public:

	/// <summary>
	/// AnimatorTransition 의 생성자입니다.
	/// </summary>
	/// 
	/// <param name="startNode">
	/// 전환될 타겟 노드입니다.<para>
	/// 이 값이 nullptr이면 default 노드가 전환될 타겟 노드가 됩니다.</para><para>
	/// startNode == nextNode 이면 블렌딩이 적용되지 않습니다.
	/// </para></param>
	/// 
	/// <param name="nextNode">
	/// 전환될 타겟 노드입니다.<para>
	/// 이 값이 nullptr이면 default 노드가 전환될 타겟 노드가 됩니다.</para><para>
	/// startNode == nextNode 이면 블렌딩이 적용되지 않습니다.
	/// </para></param>
	/// 
	/// <param name="propertyValues">
	/// 전환되기 위한 프로퍼티 조건들입니다.<para>
	/// 아무것도 들어있지 않다면 프로퍼티 조건을 사용하지 않습니다.
	/// </para></param>
	/// 
	/// <param name="exitTime">
	/// 시작 노드의 정규화된 시간이 이 값보다 커야 전환될 수 있습니다.<para>
	/// 이 값이 0보다 작다면 이 조건을 사용하지 않습니다.
	/// </para></param>
	/// 
	/// <param name="duration">
	/// 전환 시 시작 노드와 타겟 노드 사이의 블렌딩을 하게 될 타겟 노드의 정규화된 시간입니다.<para>
	/// 이 값이 0보다 작다면 전환시 블렌딩을 사용하지 않습니다.
	/// </para></param>
	/// 
	/// <param name="offset">
	/// 타겟 노드의 정규화된 시간의 시작값입니다.<para>
	/// 블렌딩을 사용하는 경우에는 이 시간부터 블렌딩이 진행됩니다.
	/// </para></param>
	/// 
	/// <param name="interrupt">
	/// 이미 동작중인 트랜지션을 중지시키고 끼어드는 경우입니다.
	/// </param>
	/// 
	/// <param name="noRecursive">
	/// 같은 노드로의 트랜지션을 금지합니다.
	/// </param>
	AnimatorTransition(
		AnimatorNode* startNode, AnimatorNode* nextNode,
		const vector<PropertyValue>& propertyValues,
		float exitTime = 0.0f, float duration = 0.0f, float offset = 0.0f,
		AnimatorTransition::Interrupt interrupt = AnimatorTransition::Interrupt::Current, 
		bool noRecursive = false);

	virtual ~AnimatorTransition();

	void SetCallback(AnimatorTransitionCallback* callback);

public:

	inline AnimatorNode* GetStartNode() const { return m_startNode; }
	inline AnimatorNode* GetNextNode() const { return m_nextNode; }
	inline float GetExitTime() const { return m_exitTime; }
	inline float GetDuration() const { return m_duration; }
	inline float GetOffset() const { return m_offset; }
	inline AnimatorTransition::Interrupt GetInterrupt() const { return m_interrupt; }
	inline bool IsNoRecursive() const { return m_noRecursive; }

	_declspec(property(get = GetStartNode)) AnimatorNode* startNode;
	_declspec(property(get = GetNextNode)) AnimatorNode* nextNode;
	_declspec(property(get = GetExitTime)) float exitTime;
	_declspec(property(get = GetDuration)) float duration;
	_declspec(property(get = GetOffset)) float offset;
	_declspec(property(get = GetInterrupt)) AnimatorTransition::Interrupt interrupt;
	_declspec(property(get = IsCantRecursive)) bool isNoRecursive;

public:

	void Used();

	enum class StartNode { Current, Next };
	bool IsTransferable(
		Animator* animator, AnimatorLayer* layer, 
		AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition, 
		StartNode& out_startNode) const;

private:

	bool CompareProperties(AnimatorProperty* property, AnimatorProperty::Value value, AnimatorProperty::Type type, AnimatorTransition::Compare comp) const;

private:

	AnimatorNode* m_startNode;
	AnimatorNode* m_nextNode;

	vector<PropertyValue>			m_propertyValues;
	vector<AnimatorProperty*>		m_propertyTriggers;

	float							m_exitTime = 0.0f;
	float							m_duration = 0.0f;
	float							m_offset = 0.0f;

	AnimatorTransition::Interrupt	m_interrupt = AnimatorTransition::Interrupt::Current;

	bool							m_noRecursive = false;
	AnimatorTransitionCallback*		m_callback = nullptr;

};

ENGINE_END
