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
	ResetAspect();
	ResetProjectionMatrix();

	ICameraManager* iCameraManager = system->graphicSystem->cameraManager;
	iCameraManager->AddCamera(this);

	m_deferredRenderTarget =
		DeferredRenderTarget::Create(
			system->graphicSystem->device,
			uint(system->graphicSystem->width),
			uint(system->graphicSystem->height));

	m_ssaoDesc.Enable = true;
	m_ssaoDesc.NumSamples = 4;
	m_ssaoDesc.BlurNumSamples = 8;
	m_ssaoDesc.Transparency = 0.225f;
	m_ssaoDesc.MinZ = 0.003f;
	m_ssaoDesc.Radius = 0.2f;
	m_ssaoDesc.Power = 2.5f;
	m_ssaoDesc.BlurPixelDistance = 1100.0f;

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

	m_ssrDesc.Enable = true;
	m_ssrDesc.BlurEnable = true;
	m_ssrDesc.BlurType = BlurType::InvDepth;
	//m_ssrDesc.NumSamples = 100;
	//m_ssrDesc.BlurNumSamples = 4;
	//m_ssrDesc.Step = 0.1f;
	//m_ssrDesc.Thickness = 0.3f;
	//m_ssrDesc.Bias = 0.05f;
	//m_ssrDesc.BlurPixelDistance = 1000.0f;
	m_ssrDesc.NumSamples = 50;
	m_ssrDesc.BlurNumSamples = 4;
	m_ssrDesc.Step = 0.2f;
	m_ssrDesc.Thickness = 0.5f;
	m_ssrDesc.Bias = 0.04f;
	m_ssrDesc.BlurPixelDistance = 1000.0f;

	m_linearDofDesc.Enable = true;
	m_linearDofDesc.BlurNumSamples = 8;
	m_linearDofDesc.MinZ = 15.0f;
	m_linearDofDesc.RangeZ = 5.0f;
	m_linearDofDesc.Power = 1.0f;
	m_linearDofDesc.BlurPixelDistance = 10.0f;
}

void Camera::Render()
{
	if (m_renderTexture)
	{
		FitDepthStencilToRenderTexture();
		FitDeferredRenderTargetToRenderTexture();
	}

	if (m_hasRenderTexture && !m_renderTexture)
	{
		m_hasRenderTexture = false;
		m_renderTexture = nullptr;
		ResetAspect();
		ResetProjectionMatrix();
	}

	m_boundingHolder = GetBoundingHolder();
}

inline void Camera::OnDestroyed()
{
	ICameraManager* iCameraManager = system->graphicSystem->cameraManager;
	iCameraManager->RemoveCamera(this);

	SafeDelete(m_depthStencil);
	SafeDelete(m_deferredRenderTarget);
}

void Camera::OnResolutionChanged(float width, float height, float aspect)
{
	ResetAspect();
	ResetProjectionMatrix();

	if (!m_renderTexture)
	{
		FitDeferredRenderTargetToResolution(uint(width), uint(height));
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

M4 Camera::GetProjectionMatrix(float nearPercent, float farPercent) const
{
	float asp = nullptr != m_renderTexture ? m_renderTexture->aspect : system->graphicSystem->aspect;

	float deltaPercent = farPercent - nearPercent;
	float deltaDist = m_far - m_near;
	float nearByPercent = m_near + deltaDist * nearPercent;
	float farByPercent = nearByPercent + deltaDist * deltaPercent;

	switch (m_projection)
	{
		case Projection::Perspective:
			return XMMatrixPerspectiveFovLH(m_fov, asp, Clamp(nearByPercent, 0.01f, farByPercent), farByPercent);
		case Projection::Orthographic:
			return XMMatrixOrthographicLH(asp * m_orthographicSize, 1.0f * m_orthographicSize, Clamp(nearByPercent, 0.01f, farByPercent), farByPercent);
	}
	return m_projectionMatrix;
}

M4 Camera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

Frustum Camera::GetFrustum(float nearPercent, float farPercent) const
{
	Frustum frustum(GetProjectionMatrix(nearPercent, farPercent));
	frustum.Transform(GetWorldMatrix());
	return frustum;
}

Frustum Camera::GetFrustum() const
{
	Frustum frustum(m_projectionMatrix);
	frustum.Transform(GetWorldMatrix());
	return frustum;
}

OrientedBounds Camera::GetOBB(float nearPercent, float farPercent) const
{
	OrientedBounds obb(GetProjectionMatrix(nearPercent, farPercent));
	obb.Transform(GetWorldMatrix());
	return obb;
}

OrientedBounds Camera::GetOBB() const
{
	OrientedBounds obb(m_projectionMatrix);
	obb.Transform(GetWorldMatrix());
	return obb;
}

BoundingHolder Camera::GetBoundingHolder(float nearPercent, float farPercent) const
{
	switch (m_projection)
	{
		default:
		case Camera::Projection::Perspective:
			return GetFrustum(nearPercent, farPercent);
		case Camera::Projection::Orthographic:
			return GetOBB(nearPercent, farPercent);
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

	ICameraManager* iCameraManager = system->graphicSystem->cameraManager;
	iCameraManager->SortByOrder();
}

bool Camera::IsWorking() const
{
	return IsActive();
}

RenderTarget* Camera::GetRenderTarget() const
{
	if (!m_renderTexture)
		return nullptr;

	return m_renderTexture->renderTarget;
}

DepthStencil* Camera::GetDepthStencil() const
{
	if (!m_renderTexture)
		return nullptr;

	return m_depthStencil;
}

DeferredRenderTarget* Camera::GetDeferredRenderTarget() const
{
	return m_deferredRenderTarget;
}

bool Camera::IsAutoFitToResolutionMode() const
{
	return m_autoFitToResolution;
}

void Camera::SetAutoFitToResolutionMode(bool value)
{
	m_autoFitToResolution = value;
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
	float asp = nullptr != m_renderTexture ? m_renderTexture->aspect : system->graphicSystem->aspect;
	SetAspect(asp);
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
	bool prevHasRenderTexture = m_hasRenderTexture;

	if (!value)
	{
		m_hasRenderTexture = false;
		m_renderTexture = nullptr;
	}
	else
	{
		m_hasRenderTexture = true;
		m_renderTexture = value;
		FitDepthStencilToRenderTexture();
	}

	if (prevHasRenderTexture != m_hasRenderTexture)
	{
		ResetAspect();
		ResetProjectionMatrix();
	}
}

bool Camera::Contains(const V3& point) const
{
	return m_boundingHolder.ContainsPoint(point);
}

bool Camera::Intersects(const BoundingHolder& boundingHolder) const
{
	return m_boundingHolder.IntersectsBoundingHolder(boundingHolder);
}

void Camera::ResetProjectionMatrix()
{
	float asp = nullptr != m_renderTexture ? m_renderTexture->aspect : system->graphicSystem->aspect;

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

void Camera::FitDepthStencilToRenderTexture()
{
	if (!m_renderTexture)
		return;

	if (!m_depthStencil)
	{
		DepthStencil::Create(system->graphicSystem->device, m_renderTexture->width, m_renderTexture->height, false, DepthStencil::Type::WRITEONLY_DEPTH_STENCIL, &m_depthStencil);
	}
	
	bool diffWidth = m_depthStencil->width != m_renderTexture->width;
	bool diffHeight = m_depthStencil->height != m_renderTexture->height;
	if (diffWidth || diffHeight)
	{
		m_depthStencil->Resize(system->graphicSystem->device, m_renderTexture->width, m_renderTexture->height);
	}
}

void Camera::FitDeferredRenderTargetToRenderTexture()
{
	if (!m_renderTexture)
		return;

	bool diffWidth = m_depthStencil->width != m_renderTexture->width;
	bool diffHeight = m_depthStencil->height != m_renderTexture->height;

	if (diffWidth || diffHeight)
	{
		m_deferredRenderTarget->Resize(system->graphicSystem->device, m_renderTexture->width, m_renderTexture->height);
	}
}

void Camera::FitDeferredRenderTargetToResolution(uint width, uint height)
{
	m_deferredRenderTarget->Resize(system->graphicSystem->device, width, height);
}
