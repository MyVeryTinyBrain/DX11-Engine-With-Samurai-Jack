#pragma once

#include "GizmoBase.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Texture2D;
class Mesh;
class MeshRenderer;
class Material;
class ENGINE_API GizmoTranslation : public GizmoBase
{
public:

	virtual GizmoBase::Axis PickTest() const override;

private:

	MeshRenderer* AxisRenderer(GizmoBase::Axis axis) const;

	bool ProjectOnAxis(GizmoBase::Axis axis, V3& inout_point) const;

	bool VirtualPlane(GizmoBase::Axis axis, const V3& look, Plane& out_plane) const;

	bool MouseOnVirtualPlane(GizmoBase::Axis axis, V3& out_point) const;

private:	// Component Routains

	virtual void Awake() override;

	virtual void GizmoUpdate() override;

	virtual void OnChangedGizmoState() override;

private:	// Setup Component Parameters

	void SetupResources();

	void SetupObjects();

private:

	ResourceRef<Mesh>			m_axisMesh;

	ResourceRef<Material>		m_rMat;
	ResourceRef<Material>		m_gMat;
	ResourceRef<Material>		m_bMat;

	MeshRenderer*				m_xRenderer = nullptr;
	MeshRenderer*				m_yRenderer = nullptr;
	MeshRenderer*				m_zRenderer = nullptr;

	GizmoBase::Axis				m_hitAxis = GizmoBase::Axis::None;
	V3							m_hitPointOnAxis;
	V3							m_hitGizmoPosition;
};

ENGINE_END
