#pragma once

#include "Object.h"
#include "Ref.h"
#include "AnimatorNode.h"

ENGINE_BEGIN

class ENGINE_API AnimatorBlendNodeElement : public Object
{
private:

	AnimatorBlendNodeElement(const Ref<AnimatorNode>& node, float position);

public:

	static AnimatorBlendNodeElement* Create(const Ref<AnimatorNode>& node, float position);

	virtual ~AnimatorBlendNodeElement();

public:

	const Ref<AnimatorNode>& GetNode() const;

	inline float GetPosition() const { return m_position; }

	_declspec(property(get = GetNode)) const Ref<AnimatorNode> node;
	_declspec(property(get = GetPosition)) float position;

private:

	Ref<AnimatorNode>			m_node;

	float						m_position;
};

ENGINE_END
