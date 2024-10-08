#pragma once

#include "GizmoBase.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Texture2D;
class Mesh;
class MeshRenderer;
class Material;
class ENGINE_API GizmoRotation : public GizmoBase
{
public:

	virtual GizmoBase::Axis PickTest() const override;

private:

	MeshRenderer* AxisRenderer(GizmoBase::Axis axis) const;

	void VirtualPlane(const V3& normal, const V3& center, Plane& out_plane) const;

	bool MouseOnVirtualPlane(const V3& normal, const V3& center, V3& out_point) const;

	void UpdateGizmoColors(GizmoBase::Axis highlightedAxis);

private:	// Component Routains

	virtual void Awake() override;

	virtual void GizmoUpdate() override;

	virtual void OnChangedGizmoState() override;

private:	// Setup Component Parameters

	void SetupAxisMesh(uint edge);

	void SetupAxisLineStripMesh(uint edge);

	void SetupSliceLineMesh();

	void SetupResources();

	void SetupObjects();

private:

	// Plane mesh
	ResourceRef<Mesh>						m_axisMesh;
	
	// Line mesh
	ResourceRef<Mesh>						m_axisLineMesh;

	// Slice line mesh
	ResourceRef<Mesh>						m_sliceLineMesh;

	// For Rendering to mesh

	ResourceRef<Material>					m_rMat;
	ResourceRef<Material>					m_gMat;
	ResourceRef<Material>					m_bMat;

	ResourceRef<Material>					m_hrMat;
	ResourceRef<Material>					m_hgMat;
	ResourceRef<Material>					m_hbMat;

	// For Rendering to line

	ResourceRef<Material>					m_lrMat;
	ResourceRef<Material>					m_lgMat;
	ResourceRef<Material>					m_lbMat;

	// For Rendering to slice line

	ResourceRef<Material>					m_slMat;

	// For rendering to mesh

	MeshRenderer*							m_xRenderer = nullptr;
	MeshRenderer*							m_yRenderer = nullptr;
	MeshRenderer*							m_zRenderer = nullptr;

	// For rendering to line

	MeshRenderer*							m_lxRenderer = nullptr;
	MeshRenderer*							m_lyRenderer = nullptr;
	MeshRenderer*							m_lzRenderer = nullptr;

	// For rendering to slice line

	MeshRenderer*							m_slxRenderer = nullptr;
	MeshRenderer*							m_slyRenderer = nullptr;
	MeshRenderer*							m_slzRenderer = nullptr;

	GizmoBase::Axis							m_highlightedAxis = GizmoBase::Axis::None;
	GizmoBase::Axis							m_hitAxis = GizmoBase::Axis::None;
	V3										m_hitNormal;
	V3										m_hitCenter;
	V3										m_hitPointOnAxis;
	Q										m_hitGizmoRotation;
	Q										m_hitHandleTransformRotation;
};

ENGINE_END
