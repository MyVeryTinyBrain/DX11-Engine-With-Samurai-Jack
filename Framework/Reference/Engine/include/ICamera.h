#pragma once

#include "PostProcessingType.h"

ENGINE_BEGIN

class ENGINE_API ICamera abstract
{
public:

	virtual ~ICamera() = default;

	virtual void OnResolutionChanged(float width, float height, float aspect) = 0;

	virtual V3 GetPosition() const = 0;

	virtual V3 GetDirection() const = 0;

	virtual Q GetRotation() const = 0;

	virtual float GetNear() const = 0;

	virtual float GetFar() const = 0;

	virtual M4 GetViewMatrix() const = 0;

	virtual M4 GetProjectionMatrix(float Near, float Far) const = 0;

	virtual M4 GetProjectionMatrix() const = 0;

	virtual Frustum GetFrustum(float Near, float Far) const = 0;
	virtual Frustum GetFrustum() const = 0;
	virtual OrientedBounds GetOBB(float Near, float Far) const = 0;
	virtual OrientedBounds GetOBB() const = 0;

	virtual BoundingHolder GetBoundingHolder(float Near, float Far) const = 0;
	virtual BoundingHolder GetBoundingHolder() const = 0;

	virtual bool Contains(const V3& point) const = 0;
	virtual bool Intersects(const BoundingHolder& boundingHolder) const = 0;

	virtual uint32_t GetAllowedLayers() const = 0;

	virtual int GetOrder() const = 0;

	virtual bool IsWorking() const = 0;
	
	virtual bool HasRenderTexture2D() const = 0;
	virtual class RenderTarget* GetRenderTarget() const = 0;
	virtual class DepthStencil* GetDepthStencil() const = 0;
	virtual class DeferredRenderTarget* GetDeferredRenderTarget() const = 0;
	virtual uint2 GetSize() const = 0;

	virtual bool IsDrawingGBuffer() const = 0;

	virtual bool GetPostProcessingState() const = 0;
	virtual const SSAODesc& GetSSAODesc() const = 0;
	virtual const SSRDesc& GetSSRDesc() const = 0;
	virtual const DOFDesc& GetDOFDesc() const = 0;
	virtual const FogDesc& GetFogDesc() const = 0;
	virtual const BloomDesc& GetBloomDesc() const = 0;
	virtual const ChromaticAberrationDesc& GetChromaticAberrationDesc() const = 0;
};

ENGINE_END
