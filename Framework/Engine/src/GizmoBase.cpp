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

void GizmoBase::PostUpdate()
{
	if (!m_handlingTransform)
	{
		enable = false;
		return;
	}

	GizmoUpdate();

	m_onEnabled = false;
}

void GizmoBase::OnEnable()
{
	m_onEnabled = true;

	NotifyChangedGizmoState();

	GizmoOnEnable();
}

void GizmoBase::OnDisable()
{
	GizmoOnDisable();
}
