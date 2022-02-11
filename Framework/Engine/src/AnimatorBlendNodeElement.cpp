#include "EnginePCH.h"
#include "AnimatorBlendNodeElement.h"

AnimatorBlendNodeElement::AnimatorBlendNodeElement(const Ref<AnimatorNode>& node, float position) :
    Object(node->name),
    m_node(node),
    m_position(position)
{
}

AnimatorBlendNodeElement* AnimatorBlendNodeElement::Create(const Ref<AnimatorNode>& node, float position)
{
    return new AnimatorBlendNodeElement(node, position);
}

AnimatorBlendNodeElement::~AnimatorBlendNodeElement()
{
    AnimatorNode* ptr = m_node.GetPointer();
    SafeDelete(ptr);
    m_node = nullptr;
}

const Ref<AnimatorNode>& AnimatorBlendNodeElement::GetNode() const
{
    return m_node;
}
