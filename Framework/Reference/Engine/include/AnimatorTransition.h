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
/// �ִϸ��̼� ��尣�� ��ȯ�� ǥ���ϴ� ��ü�Դϴ�.<para>
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
	/// AnimatorTransition �� �������Դϴ�.
	/// </summary>
	/// 
	/// <param name="startNode">
	/// ��ȯ�� Ÿ�� ����Դϴ�.<para>
	/// �� ���� nullptr�̸� default ��尡 ��ȯ�� Ÿ�� ��尡 �˴ϴ�.</para><para>
	/// startNode == nextNode �̸� ������ ������� �ʽ��ϴ�.
	/// </para></param>
	/// 
	/// <param name="nextNode">
	/// ��ȯ�� Ÿ�� ����Դϴ�.<para>
	/// �� ���� nullptr�̸� default ��尡 ��ȯ�� Ÿ�� ��尡 �˴ϴ�.</para><para>
	/// startNode == nextNode �̸� ������ ������� �ʽ��ϴ�.
	/// </para></param>
	/// 
	/// <param name="propertyValues">
	/// ��ȯ�Ǳ� ���� ������Ƽ ���ǵ��Դϴ�.<para>
	/// �ƹ��͵� ������� �ʴٸ� ������Ƽ ������ ������� �ʽ��ϴ�.
	/// </para></param>
	/// 
	/// <param name="exitTime">
	/// ���� ����� ����ȭ�� �ð��� �� ������ Ŀ�� ��ȯ�� �� �ֽ��ϴ�.<para>
	/// �� ���� 0���� �۴ٸ� �� ������ ������� �ʽ��ϴ�.
	/// </para></param>
	/// 
	/// <param name="duration">
	/// ��ȯ �� ���� ���� Ÿ�� ��� ������ ������ �ϰ� �� Ÿ�� ����� ����ȭ�� �ð��Դϴ�.<para>
	/// �� ���� 0���� �۴ٸ� ��ȯ�� ������ ������� �ʽ��ϴ�.
	/// </para></param>
	/// 
	/// <param name="offset">
	/// Ÿ�� ����� ����ȭ�� �ð��� ���۰��Դϴ�.<para>
	/// ������ ����ϴ� ��쿡�� �� �ð����� ������ ����˴ϴ�.
	/// </para></param>
	/// 
	/// <param name="interrupt">
	/// �̹� �������� Ʈ�������� ������Ű�� ������ ����Դϴ�.
	/// </param>
	/// 
	/// <param name="noRecursive">
	/// ���� ������ Ʈ�������� �����մϴ�.
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
