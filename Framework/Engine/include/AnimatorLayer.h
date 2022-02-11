#pragma once

#include "Object.h"
#include "IAnimatorLayer.h"
#include "AnimatorNode.h"
#include "AnimatorTypes.h"
#include "AnimatorProperty.h"
#include "AnimatorTransition.h"

ENGINE_BEGIN
class ENGINE_API AnimatorLayer : public Object, public IAnimatorLayer
{
public:

	AnimatorLayer();

	AnimatorLayer(const tstring& name);

	virtual ~AnimatorLayer();

public:

	void Accumulate(float deltaTime, float speed);

	void Animate(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer);

public:

	bool AddNode(AnimatorNode* node);

	bool AddProperty(AnimatorProperty* property);

	bool AddTransition(AnimatorTransition* transition);

public:

	inline Ref<AnimatorNode> GetCurrentNode() const { return m_currentNode; }

	inline Ref<AnimatorNode> GetBlendNode() const { return m_blendNode; }

	inline Ref<AnimatorTransition> GetCurrentTransition() const { return m_currentTransition; }

	inline const Ref<NodeTransform>& GetRootNode() const { return m_rootNode; }

	void SetRootNodeByName(const tstring& rootNodeName);

	V3 GetDeltaPosition() const;

	Q  GetDeltaRotation() const;

	_declspec(property(get = GetCurrentNode)) Ref<AnimatorNode> currentNode;
	_declspec(property(get = GetBlendNode)) Ref<AnimatorNode> blendNode;
	_declspec(property(get = GetCurrentTransition)) Ref<AnimatorTransition> currentTransition;
	_declspec(property(get = GetRootNode)) const Ref<NodeTransform>& rootNode;
	_declspec(property(get = GetDeltaPosition)) V3 deltaPosition;
	_declspec(property(get = GetDeltaRotation)) Q deltaRotation;

private:

	void AnimateSingleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer);

	void AnimateDoubleNode(const Ref<SkinnedMeshRenderer>& skinnedMeshRenderer);

private:

	virtual void SetSkinnedMeshRenderer(Ref<SkinnedMeshRenderer> skinnedMeshRenderer) override;

private:

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
};

ENGINE_END
