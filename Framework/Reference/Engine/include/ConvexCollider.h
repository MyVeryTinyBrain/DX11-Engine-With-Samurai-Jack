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
private:

	virtual PxGeometryHolder CreatePxGeometry(bool& out_invalid) const override;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDestroyed() override;
	virtual void DebugRender() override;

private:

	void UpdatePxConvexMesh();
	void FindMeshRenderer();
	void OnMeshRendererDestroyed(SceneObject* meshRenderer);
	void OnMeshChanged(ResourceRef<Mesh> before, ResourceRef<Mesh> after);
	virtual Bounds GetBounds() const override;

public:

	virtual ResourceRef<Mesh> GetDebugMesh() const override;
	virtual M4 GetDebugRenderWorldMatrix() const override;

private:

	Ref<MeshRenderer>		m_meshRenderer;
	PxConvexMesh*			m_pxConvexMesh = nullptr;

	ResourceRef<Mesh>		m_dbgMesh;
};

ENGINE_END
