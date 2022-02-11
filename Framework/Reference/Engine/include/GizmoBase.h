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

	// ���۴�� Ʈ�������� �����Ǹ� ȣ��˴ϴ�.
	// ������� ���� ������ ����Ǹ� ȣ��˴ϴ�.
	// ����� Ȱ��ȭ�Ǹ� ȣ��˴ϴ�.
	// ���۴�� Ʈ�������� ������ ���� ȣ��˴ϴ�.
	virtual void OnChangedGizmoState() = 0;

	// �ʿ��ϴٸ� Update ��� �缳���Ͽ� ����մϴ�.
	// HandlingTransform�� ����ִٸ� ȣ����� �ʽ��ϴ�.
	inline virtual void GizmoUpdate() {}

	// �ʿ��ϴٸ� OnEnable ��� �缳���Ͽ� ����մϴ�.
	inline virtual void GizmoOnEnable() {}

	// �ʿ��ϴٸ� OnDisable ��� �缳���Ͽ� ����մϴ�.
	inline virtual void GizmoOnDisable() {}

private:

	void NotifyChangedGizmoState();

	virtual void Update() final override;

	virtual void OnEnable() final override;

	virtual void OnDisable() final override;

private:

	bool				m_localGizmo = false;

	// �����ϰ� �Ǵ� Ʈ�������Դϴ�.
	// �� ����� Ȱ��ȭ�Ǿ����� ������Ʈ �Լ����� �˻��Ͽ� �� ������ ����ִٸ� �� ����� ��Ȱ��ȭ�մϴ�.
	Ref<Transform>		m_handlingTransform;
};

ENGINE_END
