#pragma once

#include "Renderer.h"
#include "IRendererCullOp.h"
#include "IOnCamera.h"

ENGINE_BEGIN

class ENGINE_API BillboardRenderer : public Renderer, public IRendererCullOp, public IOnCamera
{
public:

	enum LockFlag { X = 0x01, Y = 0x2, Z = 0x4, ALL = X | Y | Z, };
	using LockFlags = uint;

private:

	virtual void Awake() override;
	virtual void Render() override;
	virtual void OnCamera(ICamera* camera, RenderRequest* inout_pinput) override;

public:

	void SetLockFlags(BillboardRenderer::LockFlags lockFlags);
	void SetLock(BillboardRenderer::LockFlag lockFlag, bool value);
	BillboardRenderer::LockFlags GetLockFlags() const;
	bool IsLock(BillboardRenderer::LockFlag lockFlag) const;

	void SetSelfLockFlags(BillboardRenderer::LockFlags lockFlags);
	void SetSelfLock(BillboardRenderer::LockFlag lockFlag, bool value);
	BillboardRenderer::LockFlags GetSelfLockFlags() const;
	bool IsSelfLock(BillboardRenderer::LockFlag lockFlag) const;

	const V3& GetAdjustLocalPosition() const { return m_adjustLocalPosition; }
	void SetAdjustLocalPosition(const V3& value) { m_adjustLocalPosition = value; }
	const V3& GetAdjustLocalEulerAngles() const { return m_adjustLocalEulerAngles; }
	void SetAdjustLocalEulerAngles(const V3& value) { m_adjustLocalEulerAngles = value; }

	_declspec(property(get = GetAdjustLocalPosition, put = SetAdjustLocalPosition)) const V3& adjustLocalPosition;
	_declspec(property(get = GetAdjustLocalEulerAngles, put = SetAdjustLocalEulerAngles)) const V3& adjustLocalEulerAngles;

public:

	virtual bool CullTest(ICamera* camera) const override;
	virtual Bounds GetBounds() const override;
	virtual bool IsValid() const override;

	const ResourceRef<Mesh>& GetMesh() const { return m_mesh; }
	void SetMesh(const ResourceRef<Mesh>& mesh);

	_declspec(property(get = GetMesh, put = SetMesh)) const ResourceRef<Mesh>& mesh;

private:

	BillboardRenderer::LockFlags m_lockFlags = 0;
	BillboardRenderer::LockFlags m_selfRotateLockFlags = LockFlag::ALL;

	V3 m_adjustLocalPosition = V3::zero();
	V3 m_adjustLocalEulerAngles = V3::zero();
};

ENGINE_END
