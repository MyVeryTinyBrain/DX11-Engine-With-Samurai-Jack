#pragma once

ENGINE_BEGIN
class GameObject;
class Transform;
class System;
class ENGINE_API IComponent abstract
{
public:

	virtual ~IComponent() = default;

	// 이 컴포넌트를 생성하고 이 함수를 호출해야 합니다.
	// Awake() 및 OnEnable() 함수를 호출합니다.
	virtual void Initialize(GameObject* gameObject) = 0;

	// 컴포넌트 생성 이후로 이 함수를 실행하지 않았다면 Start() 함수를 호출합니다.
	virtual void CallStartOnce() = 0;

	// 이 함수를 호출했을때와 이전에 이 함수를 호출했을때의 활성화 상태가 다른 경우에는 
	// OnEnable() 혹은 OnDisable() 함수를 호출합니다.
	virtual void ApplyEnableChange() = 0;

public:

	// 컴포넌트가 생성된 직후 호출됩니다.
	virtual void Awake() = 0;

	// 컴포넌트가 생성된 다음 프레임에 호출됩니다.
	virtual void Start() = 0;

	// 고정 업데이트입니다.
	virtual void FixedUpdate() = 0;

	// 고정 업데이트 이후에 호출됩니다.
	virtual void PostFixedUpdate() = 0;

	// 업데이트 이전에 호출됩니다.
	virtual inline void PreUpdate() = 0;

	// 애니메이션 상태를 설정하는 업데이트입니다.
	virtual void AnimationUpdate() = 0;

	// 업데이트입니다.
	virtual void Update() = 0;

	// 업데이트 이후에 호출됩니다.
	virtual void LateUpdate() = 0;

	// 마지막 처리를 위한 업데이트입니다.
	virtual void PostUpdate() = 0;

	// 렌더 관련 처리를 합니다.
	virtual void Render() = 0;

	// 컴포넌트의 상태가 비활성화 상태에서 활성화 상태로 변경된 경우에 호출됩니다.
	virtual void OnEnable() = 0;

	// 컴포넌트의 상태가 활성화 상태에서 비활성화 상태로 변경된 경우에 호출됩니다.
	virtual void OnDisable() = 0;

	// 컴포넌트에 파괴 요청이 전달되면 호출됩니다.
	// OnDestroyed() 및 SetEnable(false) 함수를 호출합니다.
	// 파괴되기 이전에 활성화 상태였다면 SetEnable(false) 동작에 의해 OnDisable() 함수도 호출됩니다.
	virtual void OnDestroyed() = 0;

	// 씬이 변경된 경우 호출됩니다.
	virtual void OnSceneChanged() = 0;

	// 메세지를 송신한 경우 호출됩니다.
	virtual void OnMessage(const tstring& message, void* args = nullptr) = 0;
};

ENGINE_END