#pragma once

#include "GizmoBase.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Texture2D;
class Mesh;
class MeshRenderer;
class MaterialGizmoTranslation;
class ENGINE_API GizmoScaling : public GizmoBase
{
private:

	GizmoBase::Axis PickTest() const;

	MeshRenderer* AxisRenderer(GizmoBase::Axis axis) const;

	MeshRenderer* AxisPivotRenderer(GizmoBase::Axis axis) const;

	bool ProjectOnAxis(GizmoBase::Axis axis, V3& inout_point) const;

	bool VirtualPlane(GizmoBase::Axis axis, const V3& look, Plane& out_plane) const;

	bool MouseOnVirtualHandle(GizmoBase::Axis axis, V3& out_point) const;

	void RollbackTransformationSelf();

	void ApplyTransformationSelf(GizmoBase::Axis axis, float signedMagDelta);

private:	// Component Routains

	virtual void Awake() override;

	virtual void GizmoUpdate() override;

	virtual void OnChangedGizmoState() override;

private:	// Setup Component Parameters

	void SetupResources();

	void SetupObjects();

private:

	// For Pivot and Axis

	ResourceRef<Mesh>							m_axisMesh;

	ResourceRef<Texture2D>						m_rTexture;
	ResourceRef<Texture2D>						m_gTexture;
	ResourceRef<Texture2D>						m_bTexture;
	ResourceRef<Texture2D>						m_pTexture; // Pivot Texture

	ResourceRef<MaterialGizmoTranslation>		m_rMat;
	ResourceRef<MaterialGizmoTranslation>		m_gMat;
	ResourceRef<MaterialGizmoTranslation>		m_bMat;
	ResourceRef<MaterialGizmoTranslation>		m_pMat; // Pivot Material

	MeshRenderer*								m_xRenderer = nullptr;
	MeshRenderer*								m_xpRenderer = nullptr;
	MeshRenderer*								m_yRenderer = nullptr;
	MeshRenderer*								m_ypRenderer = nullptr;
	MeshRenderer*								m_zRenderer = nullptr;
	MeshRenderer*								m_zpRenderer = nullptr;
	MeshRenderer*								m_pRenderer = nullptr;

	float										m_axisWidth = 0.05f;
	float										m_pivotWidth = 0.1f;

	GizmoBase::Axis								m_hitAxis = GizmoBase::Axis::None;
	V3											m_hitPointOnAxis;
	V3											m_hitHandleTransformLocalScale;
};

ENGINE_END
