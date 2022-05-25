#pragma once

#include "AnimatorNode.h"

ENGINE_BEGIN

class ENGINE_API AnimatorEmptyNode : public AnimatorNode
{
private:

	AnimatorEmptyNode(const tstring& name, float duration, bool loop);

public:

	static AnimatorEmptyNode* Create(const tstring& name, float duration, bool loop);

	virtual ~AnimatorEmptyNode() = default;

private:

	virtual bool AnimateNodeImpl(
		uint channelIndex, 
		uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale, 
		V3& out_delatPosition, Q& out_deltaRotation) override;

	virtual float GetDurationImpl() const override;

	virtual void SetRootNode(const Ref<NodeTransform>& rootNode) override;

private:

	float m_virtualDuration = 0.0f;
};

ENGINE_END
