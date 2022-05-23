#pragma once

ENGINE_BEGIN

class Animator;
class AnimatorLayer;
class AnimatorNode;
class AnimatorTransition;

class ENGINE_API AnimatorTransitionCallback abstract
{
public:

	// ��Ƽ������ �����߿� ȣ��Ǵ� �ݹ��Լ��Դϴ�.
	// true: Ʈ������ �˻縦 �����մϴ�.
	// false: Ʈ������ �˻縦 �������� �ʰ� Ʈ�������� ������� �ʽ��ϴ�.
	virtual bool Transferable(
		Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition, 
		AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const = 0;
};

ENGINE_END
