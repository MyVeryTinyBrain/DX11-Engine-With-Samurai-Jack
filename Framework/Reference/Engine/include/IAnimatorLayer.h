#pragma once

#include "Ref.h"

ENGINE_BEGIN
class SkinnedMeshRenderer;
class NodeTransform;
class Mesh;
class Animator;
class ENGINE_API IAnimatorLayer abstract
{
public:

	IAnimatorLayer() = default;

	virtual ~IAnimatorLayer() = default;

public:

	virtual void SetSkinnedMeshRenderer(Ref<SkinnedMeshRenderer> skinnedMeshRenderer) = 0;
	virtual void ClearTransitionEvents() = 0;
	virtual void ClearAnimationEvents() = 0;
	virtual void SetAnimator(Animator* animator) = 0;
};
ENGINE_END
