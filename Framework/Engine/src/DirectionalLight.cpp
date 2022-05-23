#include "EnginePCH.h"
#include "DirectionalLight.h"
#include "Transform.h"
#include "ICamera.h"

void DirectionalLight::Awake()
{
	Light::Awake();
	SetDrawShadowMode(true);

	shadowFadeDistance = 25.0f;
	shadowFadeBeginPercent = 0.8f;
}

V3 DirectionalLight::CalculateViewPosition(ICamera* camera, float camNear, float camFar) const
{
	float deltaDist = camFar - camNear;
	V3 start = camera->GetPosition() + camera->GetDirection() * camNear;
	V3 end = start + camera->GetDirection() * deltaDist;
	V3 center = (start + end) * 0.5f;

	return center - transform->forward * (m_far - m_near) * 0.5f;
}

void DirectionalLight::GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const
{
	V3 viewPositions[3] = {};
	viewPositions[0] = CalculateViewPosition(camera, m_camNear[0], m_camFar[0]);
	viewPositions[1] = CalculateViewPosition(camera, m_camNear[1], m_camFar[1]);
	viewPositions[2] = CalculateViewPosition(camera, m_camNear[2], m_camFar[2]);

	Frustum camFrustums[3] = {}; 
	camFrustums[0] = camera->GetFrustum(m_camNear[0], m_camFar[0]);
	camFrustums[1] = camera->GetFrustum(m_camNear[1], m_camFar[1]);
	camFrustums[2] = camera->GetFrustum(m_camNear[2], m_camFar[2]);

	V3 camFrustumCorners[3][8] = {};
	camFrustums[0].GetCorners(camFrustumCorners[0]);
	camFrustums[1].GetCorners(camFrustumCorners[1]);
	camFrustums[2].GetCorners(camFrustumCorners[2]);

	float camFrustumWidthes[3] = {};
	float camFrustumHeightes[3] = {};

	for (uint i = 0; i < 3; ++i)
	{
		V3 min = V3(FLT_MAX, FLT_MAX, FLT_MAX);
		V3 max = V3(FLT_MIN, FLT_MIN, FLT_MIN);

		for (uint j = 0; j < 8; ++j)
		{
			V3 localCorner = camera->GetViewMatrix().MultiplyPoint(camFrustumCorners[i][j]);
			min = V3::Min(min, localCorner);
			max = V3::Max(max, localCorner);
		}

		camFrustumWidthes[i] = max.x - min.x;
		camFrustumHeightes[i] = max.y - min.y;
	}

	for (uint i = 0; i < 3; ++i)
	{
		V3 startPos = viewPositions[i];
		float deltaDist = m_far - m_near;

		V3 center = startPos + transform->forward * deltaDist * 0.5f;
		V3 extents = V3(camFrustumWidthes[i], camFrustumHeightes[i], deltaDist);
		Q  orientation = transform->rotation;

		out_arrBoundingHolder[i] = OrientedBounds(center, extents, orientation);
	}
}

LightDesc DirectionalLight::GetLightDesc(ICamera* camera) const
{
	V3 viewPositions[3] = {};
	viewPositions[0] = CalculateViewPosition(camera, m_camNear[0], m_camFar[0]);
	viewPositions[1] = CalculateViewPosition(camera, m_camNear[1], m_camFar[1]);
	viewPositions[2] = CalculateViewPosition(camera, m_camNear[2], m_camFar[2]);

	Frustum camFrustums[3] = {};
	camFrustums[0] = camera->GetFrustum(m_camNear[0], m_camFar[0]);
	camFrustums[1] = camera->GetFrustum(m_camNear[1], m_camFar[1]);
	camFrustums[2] = camera->GetFrustum(m_camNear[2], m_camFar[2]);

	V3 camFrustumCorners[3][8] = {};
	camFrustums[0].GetCorners(camFrustumCorners[0]);
	camFrustums[1].GetCorners(camFrustumCorners[1]);
	camFrustums[2].GetCorners(camFrustumCorners[2]);

	float camFrustumWidthes[3] = {};
	float camFrustumHeightes[3] = {};

	for (uint i = 0; i < 3; ++i)
	{
		V3 min = V3(FLT_MAX, FLT_MAX, FLT_MAX);
		V3 max = V3(FLT_MIN, FLT_MIN, FLT_MIN);

		for (uint j = 0; j < 8; ++j)
		{
			V3 localCorner = camera->GetViewMatrix().MultiplyPoint(camFrustumCorners[i][j]);
			min = V3::Min(min, localCorner);
			max = V3::Max(max, localCorner);
		}

		camFrustumWidthes[i] = max.x - min.x;
		camFrustumHeightes[i] = max.y - min.y;
	}

	LightDesc desc = {};

	desc.DrawShadow = drawShadow;
	desc.Type = LightType::Directional;
	desc.DepthSize = depthSize;
	desc.ShadowWhiteness = shadowWhiteness;
	desc.Intensity = intensity;
	desc.Position = {};
	desc.Direction = transform->forward;
	desc.Range = 0;
	desc.Angle = 0;
	desc.ShadowBias = shadowBias;
	desc.ShadowFadeDistance = shadowFadeDistance;
	desc.ShadowFadeBeginPercent = shadowFadeBeginPercent;
	desc.Diffuse = diffuse;
	desc.Ambient = ambient;
	desc.Near = m_near;
	desc.Far = m_far;
	desc.ViewMatrix[0] = DirectX::XMMatrixLookAtLH(viewPositions[0], viewPositions[0] + transform->forward, transform->up);;
	desc.ViewMatrix[1] = DirectX::XMMatrixLookAtLH(viewPositions[1], viewPositions[1] + transform->forward, transform->up);;
	desc.ViewMatrix[2] = DirectX::XMMatrixLookAtLH(viewPositions[2], viewPositions[2] + transform->forward, transform->up);;
	desc.ProjectionMatrix[0] = XMMatrixOrthographicLH(camFrustumWidthes[0], camFrustumHeightes[0], m_near, m_far);
	desc.ProjectionMatrix[1] = XMMatrixOrthographicLH(camFrustumWidthes[1], camFrustumHeightes[1], m_near, m_far);
	desc.ProjectionMatrix[2] = XMMatrixOrthographicLH(camFrustumWidthes[2], camFrustumHeightes[2], m_near, m_far);

	return desc;
}

bool DirectionalLight::ContainsInCamera(ICamera* camera) const
{
	return true;
}

FRect DirectionalLight::GetDeferredScreenQuad(ICamera* camera) const
{
	return FRect(-1.0f, +1.0f, +1.0f, -1.0f);
}

VolumetricDesc DirectionalLight::GetVolumetricDesc() const
{
	VolumetricDesc desc;
	desc.DrawVolumetric = false;
	return desc;
}
