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
	/// 애니메이터 프로퍼티를 생성하여 레이어에 추가하고 반환합니다.<para>
	/// 중복된 이름을 사용하는 경우 생성에 실패합니다.</para>
	/// </summary>
	/// <param name="name">프로퍼티 탐색에 사용될 이름입니다.</param>
	/// <param name="type">프로퍼티가 설정될 변수의 타입입니다.</param>
	AnimatorProperty* AddProperty(const tstring& name, AnimatorProperty::Type type);

	/// <summary>
	/// 애니메이터 트랜지션을 생성하여 레이어에 추가하고 반환합니다.
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
	AnimatorTransition* AddTransition(
		AnimatorNode* startNode, AnimatorNode* nextNode,
		const vector<AnimatorTransition::PropertyValue>& propertyValues,
		float exitTime = 0.0f, float duration = 0.0f, float offset = 0.0f,
		AnimatorTransition::Interrupt interrupt = AnimatorTransition::Interrupt::Current, bool noRecursive = false);

public:

	// 반드시 애니메이터에 속한 노드를 사용해야 합니다.
	// 이 노드를 재생 노드로 변경합니다.
	void Play(Ref<AnimatorNode> node, float normalizedTime = 0.0f);

	inline Ref<AnimatorNode> GetCurrentNode() const { return m_currentNode; }
	inline Ref<AnimatorNode> GetBlendNode() const { return m_blendNode; }
	inline Ref<AnimatorTransition> GetCurrentTransition() const { return m_currentTransition; }
	inline const Ref<NodeTransform>& GetRootNode() const { return m_rootNode; }

	inline Ref<AnimatorNode> GetDefaultNode() const { return m_defaultNode; }
	inline void SetDefaultNode(Ref<AnimatorNode> node) { m_defaultNode = node; }

	// CurrentNode, BlendNode 모두 검사합니다.
	bool IsPlaying(const tstring& nodeName) const;
	bool IsPlaying(Ref<AnimatorNode> node) const;
	
	// CurrentNode, BlendNode 모두 검사합니다.
	// 이 노드들의 이름에 str이 포함되어 있는지 검사합니다.
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
