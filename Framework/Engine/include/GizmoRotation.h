#pragma once

#include "GizmoBase.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Texture2D;
class Mesh;
class MeshRenderer;
class MaterialGizmoRotationNoHighlight;
class MaterialGizmoRotationHighlight;
class MaterialGizmoRotationLine;
class ENGINE_API GizmoRotation : public GizmoBase
{
private:

	GizmoBase::Axis PickTest() const;

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
	ResourceRef<Mesh>									m_axisMesh;
	
	// Line mesh
	ResourceRef<Mesh>									m_axisLineMesh;

	// Slice line mesh
	ResourceRef<Mesh>									m_sliceLineMesh;

	// Transparent Textures
	// For Rendering to highlighted mesh

	ResourceRef<Texture2D>								m_rTexture;
	ResourceRef<Texture2D>								m_gTexture;
	ResourceRef<Texture2D>								m_bTexture;

	// Regular Textures
	// For Rendering to lines

	ResourceRef<Texture2D>								m_lrTexture;
	ResourceRef<Texture2D>								m_lgTexture;
	ResourceRef<Texture2D>								m_lbTexture;

	// For Rendering to mesh

	ResourceRef<MaterialGizmoRotationNoHighlight>		m_rMat;
	ResourceRef<MaterialGizmoRotationNoHighlight>		m_gMat;
	ResourceRef<MaterialGizmoRotationNoHighlight>		m_bMat;

	ResourceRef<MaterialGizmoRotationHighlight>			m_hrMat;
	ResourceRef<MaterialGizmoRotationHighlight>			m_hgMat;
	ResourceRef<MaterialGizmoRotationHighlight>			m_hbMat;

	// For Rendering to line

	ResourceRef<MaterialGizmoRotationLine>				m_lrMat;
	ResourceRef<MaterialGizmoRotationLine>				m_lgMat;
	ResourceRef<MaterialGizmoRotationLine>				m_lbMat;

	// For Rendering to slice line

	ResourceRef<MaterialGizmoRotationLine>				m_slMat;

	// For rendering to mesh

	MeshRenderer*										m_xRenderer = nullptr;
	MeshRenderer*										m_yRenderer = nullptr;
	MeshRenderer*										m_zRenderer = nullptr;

	// For rendering to line

	MeshRenderer*										m_lxRenderer = nullptr;
	MeshRenderer*										m_lyRenderer = nullptr;
	MeshRenderer*										m_lzRenderer = nullptr;

	// For rendering to slice line

	MeshRenderer*										m_slxRenderer = nullptr;
	MeshRenderer*										m_slyRenderer = nullptr;
	MeshRenderer*										m_slzRenderer = nullptr;

	GizmoBase::Axis										m_highlightedAxis = GizmoBase::Axis::None;
	GizmoBase::Axis										m_hitAxis = GizmoBase::Axis::None;
	V3													m_hitNormal;
	V3													m_hitCenter;
	V3													m_hitPointOnAxis;
	Q													m_hitGizmoRotation;
	Q													m_hitHandleTransformRotation;
};

ENGINE_END
