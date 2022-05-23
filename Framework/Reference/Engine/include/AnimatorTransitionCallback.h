#pragma once

ENGINE_BEGIN

class Animator;
class AnimatorLayer;
class AnimatorNode;
class AnimatorTransition;

class ENGINE_API AnimatorTransitionCallback abstract
{
public:

	// 멀티스레드 동작중에 호출되는 콜백함수입니다.
	// true: 트랜지션 검사를 실행합니다.
	// false: 트랜지션 검사를 실행하지 않고 트랜지션을 사용하지 않습니다.
	virtual bool Transferable(
		Animator* animator, AnimatorLayer* layer, const AnimatorTransition* transition, 
		AnimatorNode* currentNode, AnimatorNode* blendingNode, AnimatorTransition* currentTransition) const = 0;
};

ENGINE_END
