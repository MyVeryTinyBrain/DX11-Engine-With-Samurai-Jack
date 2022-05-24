#pragma once

#include "Object.h"
#include "IAnimatorLayer.h"
#include "AnimatorNode.h"
#include "AnimatorType.h"
#include "AnimatorProperty.h"
#include "AnimatorTransition.h"

ENGINE_BEGIN

class ENGINE_API AnimatorLayer : public Object, public IAnimatorLayer
{
public:

	enum class AnimateType { Override, Additive };

public:

	AnimatorLayer();
	AnimatorLayer(const tstring& name, AnimatorLayer::AnimateType type = AnimatorLayer::AnimateType::Override);
	virtual ~AnimatorLayer();

public:

	void Accumulate(float deltaTime, float speed);
	void Animate(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer);

public:

	bool AddNode(AnimatorNode* node);

	/// <summary>
	/// �ִϸ����� ������Ƽ�� �����Ͽ� ���̾ �߰��ϰ� ��ȯ�մϴ�.<para>
	/// �ߺ��� �̸��� ����ϴ� ��� ������ �����մϴ�.</para>
	/// </summary>
	/// <param name="name">������Ƽ Ž���� ���� �̸��Դϴ�.</param>
	/// <param name="type">������Ƽ�� ������ ������ Ÿ���Դϴ�.</param>
	AnimatorProperty* AddProperty(const tstring& name, AnimatorProperty::Type type);

	/// <summary>
	/// �ִϸ����� Ʈ�������� �����Ͽ� ���̾ �߰��ϰ� ��ȯ�մϴ�.
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
	AnimatorTransition* AddTransition(
		AnimatorNode* startNode, AnimatorNode* nextNode,
		const vector<AnimatorTransition::PropertyValue>& propertyValues,
		float exitTime = 0.0f, float duration = 0.0f, float offset = 0.0f,
		AnimatorTransition::Interrupt interrupt = AnimatorTransition::Interrupt::Current, bool noRecursive = false);

public:

	// �ݵ�� �ִϸ����Ϳ� ���� ��带 ����ؾ� �մϴ�.
	// �� ��带 ��� ���� �����մϴ�.
	void Play(Ref<AnimatorNode> node, float normalizedTime = 0.0f);

	inline Ref<AnimatorNode> GetCurrentNode() const { return m_currentNode; }
	inline Ref<AnimatorNode> GetBlendNode() const { return m_blendNode; }
	inline Ref<AnimatorTransition> GetCurrentTransition() const { return m_currentTransition; }
	inline const Ref<NodeTransform>& GetRootNode() const { return m_rootNode; }

	inline Ref<AnimatorNode> GetDefaultNode() const { return m_defaultNode; }
	inline void SetDefaultNode(Ref<AnimatorNode> node) { m_defaultNode = node; }

	// CurrentNode, BlendNode ��� �˻��մϴ�.
	bool IsPlaying(const tstring& nodeName) const;
	bool IsPlaying(Ref<AnimatorNode> node) const;
	
	// CurrentNode, BlendNode ��� �˻��մϴ�.
	// �� ������ �̸��� str�� ���ԵǾ� �ִ��� �˻��մϴ�.
	bool IsContains(const tstring& str) const;

	void SetRootNodeByName(const tstring& rootNodeName);
	V3 GetDeltaPosition() const;
	Q  GetDeltaRotation() const;

	void SetType(AnimatorLayer::AnimateType type) { m_type = type; }
	AnimatorLayer::AnimateType GetType() const { return m_type; }

	_declspec(property(get = GetCurrentNode)) Ref<AnimatorNode> currentNode;
	_declspec(property(get = GetBlendNode)) Ref<AnimatorNode> blendNode;
	_declspec(property(get = GetCurrentTransition)) Ref<AnimatorTransition> currentTransition;
	_declspec(property(get = GetRootNode)) const Ref<NodeTransform>& rootNode;
	_declspec(property(get = GetDefaultNode, put = SetDefaultNode)) Ref<AnimatorNode> defaultNode;
	_declspec(property(get = GetDeltaPosition)) V3 deltaPosition;
	_declspec(property(get = GetDeltaRotation)) Q deltaRotation;
	_declspec(property(get = GetType, put = SetType)) AnimatorLayer::AnimateType type;

public:

	Ref<AnimatorNode> GetBeginChangingNode() const;
	Ref<AnimatorNode> GetEndChangedNode() const;
	Ref<AnimatorNode> GetPreviousNode() const;

	const vector<AnimationEventDesc>& GetEventDescs() const { return m_eventDescs; }

private:

	void AnimateSingleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer);
	void AnimateDoubleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer);
	void AnimateNodeTransform(const Ref<NodeTransform>& nodeTransform, const V3& t, const Q& r, const V3& s);

private:

	virtual void SetSkinnedMeshRenderer(Ref<SkinnedMeshRenderer> skinnedMeshRenderer) override;
	virtual void ClearTransitionEvents() override;
	virtual void ClearAnimationEvents() override;
	virtual void SetAnimator(Animator* animator) override;

private:

	AnimateType													m_type = AnimateType::Override;
	AnimationBlendMode											m_blendMode = AnimationBlendMode::Additive;

	AnimatorNode*												m_defaultNode = nullptr;
	AnimatorNode*												m_currentNode = nullptr;
	AnimatorTransition*											m_currentTransition = nullptr;
	AnimatorNode*												m_blendNode = nullptr;

	float														m_blendPercent = 0.0f;

	vector<AnimatorNode*>										m_nodes;
	unordered_map<tstring, AnimatorProperty*>					m_properties;
	unordered_map<AnimatorNode*, vector<AnimatorTransition*>>	m_transitions;

	Ref<SkinnedMeshRenderer>									m_skinnedMeshRenderer;
	Ref<NodeTransform>											m_rootNode;
	V3															m_deltaPosition = V3::zero();
	Q															m_deltaRotation = Q::identity();

	// Result Data for Transition Events

	AnimatorNode*												m_beginChangingNode = nullptr;
	AnimatorNode*												m_endChangedNode = nullptr;
	AnimatorNode*												m_prevNode = nullptr;

	// ContextStr message for Animation Evnets

	vector<AnimationEventDesc>									m_eventDescs;

	// ...

	Animator*													m_animator = nullptr;
};

ENGINE_END
