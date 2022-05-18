#include "EnginePCH.h"
#include "Camera.h"
#include "System.h"
#include "GraphicSystem.h"
#include "Transform.h"
#include "CameraManager.h"
#include "RenderTexture2D.h"
#include "DepthStencil.h"
#include "DeferredRenderTarget.h"

void Camera::Awake()
{
	Reset();

	ICameraManager* iCameraManager = system->graphic->cameraManager;
	iCameraManager->AddCamera(this);

	m_deferredRenderTarget =
		DeferredRenderTarget::Create(
			system->graphic->device,
			uint(system->graphic->width),
			uint(system->graphic->height));

	m_ssaoDesc.Enable = true;
	m_ssaoDesc.NumSamples = 4;
	m_ssaoDesc.BlurNumSamples = 8;
	m_ssaoDesc.Transparency = 0.225f;
	m_ssaoDesc.MinZ = 0.003f;
	m_ssaoDesc.Radius = 0.28f;
	m_ssaoDesc.Power = 2.5f;
	m_ssaoDesc.BlurPixelDistance = 1100.0f;

	m_ssrDesc.Enable = true;
	m_ssrDesc.BlurEnable = true;
	m_ssrDesc.BlurType = BlurType::InvDepth;
	m_ssrDesc.NumSamples = 50;
	m_ssrDesc.BlurNumSamples = 4;
	m_ssrDesc.Step = 0.2f;
	m_ssrDesc.Thickness = 0.5f;
	m_ssrDesc.Bias = 0.04f;
	m_ssrDesc.BlurPixelDistance = 1000.0f;
	m_ssrDesc.ResolutionScale = 0.5f;

	m_dofDesc.Enable = true;
	m_dofDesc.BlurNumSamples = 8;
	m_dofDesc.MinZ = 15.0f;
	m_dofDesc.RangeZ = 5.0f;
	m_dofDesc.Power = 1.0f;
	m_dofDesc.BlurPixelDistance = 10.0f;

	m_fogDesc.Enable = false;
	m_fogDesc.Type = FogType::Distance;
	m_fogDesc.MinZ = 30.0f;
	m_fogDesc.RangeZ = 40.0f;
	m_fogDesc.Color = Color::white();

	m_bloomDesc.Enable = true;
	m_bloomDesc.Type = BloomType::Add;
	m_bloomDesc.BlurNumSamples = 8;
	m_bloomDesc.Intensity = 1.5f;
	m_bloomDesc.Threshold = 0.1f;
	m_bloomDesc.BlurPixelDistance = 50.0f;

	m_chromaticAberrationDesc.Enable = true;
	m_chromaticAberrationDesc.Blend = V4::one();
	m_chromaticAberrationDesc.Offset = V4::one() * 2.5f;
	m_chromaticAberrationDesc.Angle = V4(0, 120, 240, 0);
}

void Camera::Render()
{
	if (!m_renderTexture)
	{
		FitDeferredRenderTargetToResolution(uint(system->graphic->width), uint(system->graphic->height));
	}
	else
	{
		FitDeferredRenderTargetToRenderTexture();
	}

	// Update Bounding holder
	m_boundingHolder = GetBoundingHolder();
}

inline void Camera::OnDestroyed()
{
	ICameraManager* iCameraManager = system->graphic->cameraManager;
	iCameraManager->RemoveCamera(this);

	SafeDelete(m_deferredRenderTarget);
}

void Camera::OnResolutionChanged(float width, float height, float aspect)
{
	if (!m_renderTexture)
	{
		FitDeferredRenderTargetToResolution(uint(system->graphic->width), uint(system->graphic->height));
	}
	else
	{
		FitDeferredRenderTargetToRenderTexture();
	}
}

V3 Camera::GetPosition() const
{
	return transform->position;
}

void Camera::SetPosition(const V3& position)
{
	transform->position = position;
}

V3 Camera::GetDirection() const
{
	return transform->forward;
}

void Camera::SetDirection(const V3& direction)
{
	transform->forward = direction;
}

Q Camera::GetRotation() const
{
	return transform->rotation;
}

M4 Camera::GetWorldMatrix() const
{
	M4 localToWorld = transform->localToWorldMatrix;
	V3 right = localToWorld.GetRow(0);
	V3 up = localToWorld.GetRow(1);
	V3 forward = localToWorld.GetRow(2);

	right.Normalize();
	up.Normalize();
	forward.Normalize();

	M4 unscaledLocalToWorld;
	unscaledLocalToWorld.SetRow(0, V4(right.x, right.y, right.z, 0.0f));
	unscaledLocalToWorld.SetRow(1, V4(up.x, up.y, up.z, 0.0f));
	unscaledLocalToWorld.SetRow(2, V4(forward.x, forward.y, forward.z, 0.0f));
	unscaledLocalToWorld.SetRow(3, localToWorld.GetRow(3));

	return unscaledLocalToWorld;
}

M4 Camera::GetViewMatrix() const
{
	return GetWorldMatrix().inversed;
}

M4 Camera::GetProjectionMatrix(float Near, float Far) const
{
	float asp = nullptr != m_renderTexture ? m_renderTexture->aspect : system->graphic->aspect;

	switch (m_projection)
	{
		case Projection::Perspective:
			return XMMatrixPerspectiveFovLH(m_fov, asp, Clamp(Near, 0.01f, Far), Far);
		case Projection::Orthographic:
			return XMMatrixOrthographicLH(asp * m_orthographicSize, 1.0f * m_orthographicSize, Clamp(Near, 0.01f, Far), Far);
	}
	return m_projectionMatrix;

	//float asp = nullptr != m_renderTexture ? m_renderTexture->aspect : system->graphic->aspect;

	//float deltaPercent = farPercent - nearPercent;
	//float deltaDist = m_far - m_near;
	//float nearByPercent = m_near + deltaDist * nearPercent;
	//float farByPercent = nearByPercent + deltaDist * deltaPercent;

	//switch (m_projection)
	//{
	//	case Projection::Perspective:
	//		return XMMatrixPerspectiveFovLH(m_fov, asp, Clamp(nearByPercent, 0.01f, farByPercent), farByPercent);
	//	case Projection::Orthographic:
	//		return XMMatrixOrthographicLH(asp * m_orthographicSize, 1.0f * m_orthographicSize, Clamp(nearByPercent, 0.01f, farByPercent), farByPercent);
	//}
	//return m_projectionMatrix;
}

M4 Camera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

Frustum Camera::GetFrustum(float Near, float Far) const
{
	Frustum frustum(GetProjectionMatrix(Near, Far));
	frustum.Transform(GetWorldMatrix());
	return frustum;
}

Frustum Camera::GetFrustum() const
{
	Frustum frustum(m_projectionMatrix);
	frustum.Transform(GetWorldMatrix());
	return frustum;
}

OrientedBounds Camera::GetOBB(float Near, float Far) const
{
	OrientedBounds obb(GetProjectionMatrix(Near, Far));
	obb.Transform(GetWorldMatrix());
	return obb;
}

OrientedBounds Camera::GetOBB() const
{
	OrientedBounds obb(m_projectionMatrix);
	obb.Transform(GetWorldMatrix());
	return obb;
}

BoundingHolder Camera::GetBoundingHolder(float Near, float Far) const
{
	switch (m_projection)
	{
		default:
		case Camera::Projection::Perspective:
			return GetFrustum(Near, Far);
		case Camera::Projection::Orthographic:
			return GetOBB(Near, Far);
	}
}

BoundingHolder Camera::GetBoundingHolder() const
{
	switch (m_projection)
	{
		default:
		case Camera::Projection::Perspective:
			return GetFrustum();
		case Camera::Projection::Orthographic:
			return GetOBB();
	}
}

uint32_t Camera::GetAllowedLayers() const
{
	return m_allowedLayers;
}

void Camera::SetAllowedLayers(uint32_t value)
{
	m_allowedLayers = value;
}

int Camera::GetOrder() const
{
	return m_order;
}

void Camera::SetOrder(int value)
{
	m_order = value;

	ICameraManager* iCameraManager = system->graphic->cameraManager;
	iCameraManager->SortByOrder();
}

bool Camera::IsWorking() const
{
	return IsActive();
}

bool Camera::HasRenderTexture2D() const
{
	return !m_renderTexture.IsNull();
}

RenderTarget* Camera::GetRenderTarget() const
{
	if (!m_renderTexture)
	{
		return m_deferredRenderTarget->result;
	}
	else
	{
		return m_renderTexture->renderTarget;
	}
}

DepthStencil* Camera::GetDepthStencil() const
{
	return m_deferredRenderTarget->depthStencil;
}

DeferredRenderTarget* Camera::GetDeferredRenderTarget() const
{
	return m_deferredRenderTarget;
}

uint2 Camera::GetSize() const
{
	return uint2(m_deferredRenderTarget->width, m_deferredRenderTarget->height);
}

float Camera::GetAspect() const
{
	return m_aspect;
}

void Camera::SetAspect(float value)
{
	if (m_renderTexture)
	{
		m_aspect = m_renderTexture->aspect;
	}
	else
	{
		m_aspect = Abs(value);
	}

	ResetProjectionMatrix();
}

void Camera::ResetAspect()
{
	float asp = nullptr != m_renderTexture ? m_renderTexture->aspect : system->graphic->aspect;
	SetAspect(asp);
}

void Camera::ResetProjectionMatrix()
{
	float asp = nullptr != m_renderTexture ? m_renderTexture->aspect : system->graphic->aspect;

	switch (m_projection)
	{
		case Projection::Perspective:
			m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fov, asp, m_near, m_far);
			break;
		case Projection::Orthographic:
			m_projectionMatrix = XMMatrixOrthographicLH(asp * m_orthographicSize, 1.0f * m_orthographicSize, 0, m_far);
			break;
	}
}

void Camera::Reset()
{
	ResetAspect();
	ResetProjectionMatrix();
}

float Camera::GetNear() const
{
	return m_near;
}

void Camera::SetNear(float value)
{
	m_near = Clamp(value, 0.01f, m_far);
	ResetProjectionMatrix();
}

float Camera::GetFar() const
{
	return m_far;
}

void Camera::SetFar(float value)
{
	m_far = Clamp(value, 0.0f, FLT_MAX);
	ResetProjectionMatrix();
}

float Camera::GetFOV() const
{
	return m_fov * Rad2Deg;
}

void Camera::SetFOV(float angle)
{
	m_fov = angle * Deg2Rad;
	ResetProjectionMatrix();
}

float Camera::GetOrthographicSize() const
{
	return m_orthographicSize;
}

void Camera::SetOrthographicSize(float value)
{
	m_orthographicSize = value;
	ResetProjectionMatrix();
}

Camera::Projection Camera::GetProjection() const
{
	return m_projection;
}

void Camera::SetProjection(Projection mode)
{
	m_projection = mode;
	ResetProjectionMatrix();
}

const ResourceRef<RenderTexture2D>& Camera::GetRenderTexture() const
{
	return m_renderTexture;
}

void Camera::SetRenderTexture(const ResourceRef<RenderTexture2D>& value)
{
	if (value == m_renderTexture)
		return;

	m_renderTexture = value;

	if (!m_renderTexture)
	{
		FitDeferredRenderTargetToResolution(uint(system->graphic->width), uint(system->graphic->height));
	}
	else
	{
		FitDeferredRenderTargetToRenderTexture();
	}

	Reset();
}

bool Camera::Contains(const V3& point) const
{
	return m_boundingHolder.ContainsPoint(point);
}

bool Camera::Intersects(const BoundingHolder& boundingHolder) const
{
	return m_boundingHolder.IntersectsBoundingHolder(boundingHolder);
}

void Camera::FitDeferredRenderTargetToRenderTexture()
{
	if (!m_renderTexture)
		return;

	if (m_deferredRenderTarget->Resize(system->graphic->device, m_renderTexture->width, m_renderTexture->height))
	{
		Reset();
	}
}

void Camera::FitDeferredRenderTargetToResolution(uint width, uint height)
{
	if (m_deferredRenderTarget->Resize(system->graphic->device, width, height))
	{
		Reset();
	}
}
