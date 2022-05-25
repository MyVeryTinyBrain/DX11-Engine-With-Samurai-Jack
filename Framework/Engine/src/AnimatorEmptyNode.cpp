#include "EnginePCH.h"
#include "AnimatorEmptyNode.h"

AnimatorEmptyNode::AnimatorEmptyNode(const tstring& name, float duration, bool loop) :
    AnimatorNode(name, loop, true),
    m_virtualDuration(duration)
{
}

AnimatorEmptyNode* AnimatorEmptyNode::Create(const tstring& name, float duration, bool loop)
{
    assert(duration >= 0.0f); // 지속시간은 음수일수 없습니다.
    return new AnimatorEmptyNode(name, duration, loop);
}

bool AnimatorEmptyNode::AnimateNodeImpl(uint channelIndex, uint& out_nodeIndex, V3& out_position, Q& out_rotation, V3& out_scale, V3& out_delatPosition, Q& out_deltaRotation)
{
    return false;
}

float AnimatorEmptyNode::GetDurationImpl() const
{
    return m_virtualDuration;
}

void AnimatorEmptyNode::SetRootNode(const Ref<NodeTransform>& rootNode)
{
    m_rootNode = rootNode;
}
