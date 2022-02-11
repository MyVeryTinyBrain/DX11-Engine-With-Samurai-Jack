#pragma once

#include "Component.h"
#include "Ref.h"

ENGINE_BEGIN
class GizmoBase;
class ENGINE_API Gizmo : public Component
{
public:

	void ShowGizmo(bool value);

	inline bool IsShowed() const { return m_showGizmo; }

	void NextGizmo();

	void PrevGizmo();

	void SetGizmo(uint index);

	void SetHandlingTransform(Ref<Transform> handlingTransform);

	Ref<Transform> GetHandlingTransform() const;

	void SetGizmoSpace(bool localSpace);

	inline bool IsLocalGizmo() const { return m_localSpace; }

	_declspec(property(get = IsShowed, put = ShowGizmo)) bool show;
	_declspec(property(get = GetHandlingTransform, put = SetHandlingTransform)) Ref<Transform> handlingTransform;
	_declspec(property(get = IsLocalGizmo, put = SetGizmoSpace)) bool isLocalSpace;

private:

	virtual void Awake() override;

	virtual void Update() override;

	void AutoScaling();

private:

	void SetupObjects();

private:

	GizmoBase*					m_translation = nullptr;
	GizmoBase*					m_rotation = nullptr;
	GizmoBase*					m_scaling = nullptr;
	GizmoBase*					m_gizmos[3] = {};

	uint						m_currentGizmo = 0;
	bool						m_showGizmo = false;
	bool						m_localSpace = false;

	Ref<Transform>				m_handlingTransform = nullptr;
};

ENGINE_END
