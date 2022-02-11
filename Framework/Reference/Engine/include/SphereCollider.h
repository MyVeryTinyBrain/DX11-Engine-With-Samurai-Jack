#pragma once

#include "Collider.h"
#include "ResourceRef.h"
#include "IRendererCullOp.h"

ENGINE_BEGIN
class Mesh;
class Material;
class ENGINE_API SphereCollider final : public Collider, public IRendererCullOp
{

	virtual PxGeometryHolder CreatePxGeometry(bool& out_invalid) const override;

	virtual void Awake() override;
	
	virtual void Render() override;

	virtual bool CullTest(ICamera* camera) const override;

public:

	virtual Bounds GetBounds() const override;

	inline float GetRadius() const { return m_radius; }

	void SetRadius(float value);

	_declspec(property(get = GetRadius, put = SetRadius)) float radius;

private:

	float					m_radius = 0.5f;

	ResourceRef<Mesh>		m_dbgMesh;
	ResourceRef<Material>	m_dbgMaterial;
};

ENGINE_END
