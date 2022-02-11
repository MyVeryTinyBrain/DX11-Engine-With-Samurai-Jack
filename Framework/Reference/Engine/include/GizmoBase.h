#pragma once

#include "Component.h"
#include "Ref.h"

ENGINE_BEGIN

class ENGINE_API GizmoBase abstract : public Component
{
public:

	enum class Axis
	{
		None,
		X,
		Y,
		Z,
		Pivot,
	};

public:

	inline bool IsLocalGizmo() const { return m_localGizmo; }

	void SetGizmoSpace(bool localSpace);

	Ref<Transform> GetHandlingTransform() const;

	void SetHandlingTransform(const Ref<Transform>& handlingTransform);

	_declspec(property(get = IsLocalGizmo, put = SetGizmoSpace)) bool isLocalGizmo;
	_declspec(property(get = GetHandlingTransform, put = SetHandlingTransform)) Ref<Transform> handlingTransform;

protected:

	// 조작대상 트랜스폼이 설정되면 호출됩니다.
	// 기즈모의 기준 공간이 변경되면 호출됩니다.
	// 기즈모가 활성화되면 호출됩니다.
	// 조작대상 트랜스폼이 존재할 때만 호출됩니다.
	virtual void OnChangedGizmoState() = 0;

	// 필요하다면 Update 대신 재설정하여 사용합니다.
	// HandlingTransform이 비어있다면 호출되지 않습니다.
	inline virtual void GizmoUpdate() {}

	// 필요하다면 OnEnable 대신 재설정하여 사용합니다.
	inline virtual void GizmoOnEnable() {}

	// 필요하다면 OnDisable 대신 재설정하여 사용합니다.
	inline virtual void GizmoOnDisable() {}

private:

	void NotifyChangedGizmoState();

	virtual void Update() final override;

	virtual void OnEnable() final override;

	virtual void OnDisable() final override;

private:

	bool				m_localGizmo = false;

	// 조작하게 되는 트랜스폼입니다.
	// 이 기즈모가 활성화되었을때 업데이트 함수에서 검사하여 이 변수가 비어있다면 이 기즈모를 비활성화합니다.
	Ref<Transform>		m_handlingTransform;
};

ENGINE_END
