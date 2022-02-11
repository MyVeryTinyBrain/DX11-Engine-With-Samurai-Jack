#pragma once

#include "AnimatorNode.h"
#include "AnimationClip.h"

ENGINE_BEGIN

class ENGINE_API AnimatorSingleNode : public AnimatorNode
{
private:

	AnimatorSingleNode(const tstring& name, const SubResourceRef<AnimationClip>& animationClip, bool loop);

	AnimatorSingleNode(const SubResourceRef<AnimationClip>& animationClip, bool loop);

public:

	static AnimatorSingleNode* Create(const tstring& name, const SubResourceRef<AnimationClip>& animationClip, bool loop);

	static AnimatorSingleNode* Create(const SubResourceRef<AnimationClip>& animationClip, bool loop);

	virtual ~AnimatorSingleNode() = default;

private:

	virtual bool AnimateNodeImpl(
		uint channelIndex,
		uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale, 
		V3& out_deltaPosition, Q& out_deltaRotation) override;

	virtual float GetDurationImpl() const override;

	virtual void SetRootNode(const Ref<NodeTransform>& rootNode) override;

private:

	SubResourceRef<AnimationClip> m_animationClip;

};

ENGINE_END
