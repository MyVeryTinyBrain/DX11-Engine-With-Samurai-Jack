#pragma once

#include "Ref.h"

ENGINE_BEGIN
class NodeTransform;
class ENGINE_API IAnimatorNode abstract
{
public:

	IAnimatorNode() = default;

	virtual ~IAnimatorNode() = default;

public:

	virtual void SetRootNode(const Ref<NodeTransform>& rootNode) = 0;
};

ENGINE_END
