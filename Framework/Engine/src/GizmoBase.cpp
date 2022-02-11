#include "EnginePCH.h"
#include "GizmoBase.h"

void GizmoBase::SetGizmoSpace(bool localSpace)
{
	if (m_localGizmo == localSpace)
		return;

	m_localGizmo = localSpace;

	NotifyChangedGizmoState();
}

Ref<Transform> GizmoBase::GetHandlingTransform() const
{
	return m_handlingTransform;
}

void GizmoBase::SetHandlingTransform(const Ref<Transform>& handlingTransform)
{
	if (m_handlingTransform == handlingTransform)
		return;

	m_handlingTransform = handlingTransform;

	NotifyChangedGizmoState();
}

void GizmoBase::NotifyChangedGizmoState()
{
	if (!m_handlingTransform)
		return;

	OnChangedGizmoState();
}

void GizmoBase::Update()
{
	if (!m_handlingTransform)
	{
		enable = false;
		return;
	}

	GizmoUpdate();
}

void GizmoBase::OnEnable()
{
	NotifyChangedGizmoState();

	GizmoOnEnable();
}

void GizmoBase::OnDisable()
{
	GizmoOnDisable();
}
