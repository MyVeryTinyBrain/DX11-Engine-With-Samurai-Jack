#pragma once

#include "Ref.h"

ENGINE_BEGIN
class SkinnedMeshRenderer;
class NodeTransform;
class Mesh;
class ENGINE_API IAnimatorLayer abstract
{
public:

	IAnimatorLayer() = default;

	virtual ~IAnimatorLayer() = default;

public:

	virtual void SetSkinnedMeshRenderer(Ref<SkinnedMeshRenderer> skinnedMeshRenderer) = 0;
};
ENGINE_END
