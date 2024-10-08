#pragma once

#include "Collider.h"
#include "ResourceRef.h"
#include "IRendererCullOp.h"

ENGINE_BEGIN
class Mesh;
class Material;
class ENGINE_API CapsuleCollider final : public Collider, public IRendererCullOp
{
	virtual PxGeometryHolder CreatePxGeometry(bool& out_invalid) const override;

	virtual void Awake() override;
	virtual void Awake(void* arg /* PxShape */) override;
	virtual void Start() override;

	virtual void DebugRender() override;

	virtual bool CullTest(ICamera* camera) const override;

public:

	virtual Bounds GetBounds() const override;

	inline float GetRadius() const { return m_radius; }

	void SetRadius(float value);

	inline float GetHalfHeight() const { return m_halfHeight; }

	void SetHalfHeight(float value);

	_declspec(property(get = GetRadius, put = SetRadius)) float radius;
	_declspec(property(get = GetHalfHeight, put = SetHalfHeight)) float halfHeight;

private:

	void CreateDebugShape();
	void ResetDebugShape();

public:

	virtual ResourceRef<Mesh> GetDebugMesh() const override;
	virtual M4 GetDebugRenderWorldMatrix() const override;

private:

	float					m_radius = 0.5f;
	float					m_halfHeight = 0.5f;

	ResourceRef<Mesh>		m_dbgMesh;
};

ENGINE_END
