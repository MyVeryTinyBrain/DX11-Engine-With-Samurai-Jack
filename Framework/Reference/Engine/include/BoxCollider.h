#pragma once

#include "Collider.h"
#include "ResourceRef.h"
#include "IRendererCullOp.h"

ENGINE_BEGIN
class Mesh;
class Material;
class ENGINE_API BoxCollider final : public Collider, public IRendererCullOp
{

	virtual PxGeometryHolder CreatePxGeometry(bool& out_invalid) const override;

	virtual void Awake() override;

	virtual void Render() override;

	virtual bool CullTest(ICamera* camera) const override;

public:

	virtual Bounds GetBounds() const override;

	inline const V3& GetExtents() const { return m_extents; }

	void SetExtents(const V3& extents);

	inline V3 GetExtentsAsSize() const { return m_extents * 2.0f; }

	inline void SetExtentsAsSize(const V3& size) { SetExtents(size * 0.5f); }

	_declspec(property(get = GetExtents, put = SetExtents)) const V3& extents;
	_declspec(property(get = GetExtentsAsSize, put = SetExtentsAsSize)) V3 extentsAsSize;

private:

	/*

	¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á    ¦³
	¡á         ¡á    | extents.y * 2.0 = size.y
	¡á         ¡á    |
	¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á    ¦º

	|---------|
	 extents.x * 2.0 = size.x

	 extents.x, extents.y, extents.z >= 0

	*/
	V3						m_extents = V3::one() * 0.5f;

	ResourceRef<Mesh>		m_dbgMesh;
	ResourceRef<Material>	m_dbgMaterial;
};

ENGINE_END
