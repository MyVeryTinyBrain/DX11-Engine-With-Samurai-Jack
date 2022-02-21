#pragma once

#include "Component.h"
#include "ICamera.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class RenderTexture2D;
class ENGINE_API Camera : public Component, public ICamera
{
public:

	enum class Projection { Perspective, Orthographic };

protected:

	virtual void Awake() override;
	virtual void Render() override;
	virtual inline void OnDestroyed() override;
	virtual void OnResolutionChanged(float width, float height, float aspect) override;

public:

	virtual V3 GetPosition() const override;
	void SetPosition(const V3& position);

	virtual V3 GetDirection() const override;
	void SetDirection(const V3& direction);

	M4 GetWorldMatrix() const;
	virtual M4 GetViewMatrix() const override;
	virtual M4 GetProjectionMatrix(float nearPercent, float farPercent) const override; 
	virtual M4 GetProjectionMatrix() const override;

	virtual Frustum GetFrustum(float nearPercent, float farPercent) const override;
	virtual Frustum GetFrustum() const override;
	virtual OrientedBounds GetOBB(float nearPercent, float farPercent) const override;
	virtual OrientedBounds GetOBB() const override;

	virtual BoundingHolder GetBoundingHolder(float nearPercent, float farPercent) const override;
	virtual BoundingHolder GetBoundingHolder() const override;
	 
	virtual uint32_t GetAllowedLayers() const override;
	void SetAllowedLayers(uint32_t value);

	virtual int GetOrder() const override;
	void SetOrder(int value);

	_declspec(property(get = GetPosition, put = SetPosition)) V3 position;
	_declspec(property(get = GetDirection, put = SetDirection)) V3 direction;
	_declspec(property(get = GetViewMatrix)) M4 viewMatrix;
	_declspec(property(get = GetProjectionMatrix)) M4 projectionMatrix;
	_declspec(property(get = GetAllowedLayers, put = SetAllowedLayers)) uint32_t allowedLayers;
	_declspec(property(get = GetOrder, put = SetOrder)) int order;

private:

	virtual bool IsWorking() const override;
	virtual class RenderTarget* GetRenderTarget() const override;
	virtual class DepthStencil* GetDepthStencil() const override;

public:

	virtual class DeferredRenderTarget* GetDeferredRenderTarget() const override;

public:

	bool IsAutoFitToResolutionMode() const;
	void SetAutoFitToResolutionMode(bool value);

	float GetAspect() const;
	void SetAspect(float value);
	void ResetAspect();

	float GetNear() const override;
	void SetNear(float value);
	float GetFar() const override;
	void SetFar(float value);

	float GetFOV() const;
	void SetFOV(float angle);

	float GetOrthographicSize() const;
	void SetOrthographicSize(float value);

	Projection GetProjection() const;
	void SetProjection(Projection mode);

	const ResourceRef<RenderTexture2D>& GetRenderTexture() const;
	void SetRenderTexture(const ResourceRef<RenderTexture2D>& value);

	_declspec(property(get = IsAutoFitToResolutionMode, put = SetAutoFitToResolutionMode)) bool autoFitToResolution;
	_declspec(property(get = GetAspect, put = SetAspect)) float aspect;
	_declspec(property(get = GetNear, put = SetNear)) float Near;
	_declspec(property(get = GetFar, put = SetFar)) float Far;
	_declspec(property(get = GetFOV, put = SetFOV)) float fov;
	_declspec(property(get = GetOrthographicSize, put = SetOrthographicSize)) float orthographicSize;
	_declspec(property(get = GetProjection, put = SetProjection)) Projection projection;

public:

	virtual bool Contains(const V3& point) const override;
	virtual bool Intersects(const BoundingHolder& boundingHolder) const override;

private:

	void ResetProjectionMatrix();
	void FitDepthStencilToRenderTexture();
	void FitDeferredRenderTargetToRenderTexture();
	void FitDeferredRenderTargetToResolution(uint width, uint height);

private:

	bool								m_autoFitToResolution = false;

	float								m_aspect = 1.0f;
	float								m_near = 0.1f;
	float								m_far = 250.0f;
	float								m_fov = 60.0f * Deg2Rad;
	float								m_orthographicSize = 1.0f;

	Projection							m_projection = Projection::Perspective;
	M4									m_projectionMatrix = M4::identity();
	BoundingHolder						m_boundingHolder;

	uint32_t							m_allowedLayers = 0xFFFFFFFF;
	int									m_order = 0;

	bool								m_hasRenderTexture = false;
	ResourceRef<RenderTexture2D>		m_renderTexture;
	class DepthStencil*					m_depthStencil = nullptr;

	class DeferredRenderTarget*			m_deferredRenderTarget = nullptr;
};

ENGINE_END
