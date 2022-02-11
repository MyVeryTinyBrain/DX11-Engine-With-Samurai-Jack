#pragma once

#include "Collider.h"
#include "Ref.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class MeshRenderer;
class Mesh;
class Material;
class ENGINE_API ConvexCollider : public Collider
{

	virtual PxGeometryHolder CreatePxGeometry(bool& out_invalid) const override;

	virtual void Awake() override;

	virtual void Start() override;

	virtual void Update() override;

private:

	bool CheckMeshRenderer();
	
	void UpdatePxGeometry();

private:

	Ref<MeshRenderer>		m_meshRenderer;
	ResourceRef<Mesh>		m_mesh;

	PxConvexMesh*			m_pxMesh = nullptr;

	ResourceRef<Mesh>		m_dbgMesh;
	ResourceRef<Material>	m_dbgMaterial;
};

ENGINE_END
