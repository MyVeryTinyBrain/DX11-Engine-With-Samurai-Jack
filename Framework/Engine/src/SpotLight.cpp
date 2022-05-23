#include "EnginePCH.h"
#include "SpotLight.h"
#include "Transform.h"
#include "ICamera.h"

void SpotLight::Awake()
{
    Light::Awake();

    shadowFadeDistance = 50.0f;
    shadowFadeBeginPercent = 0.9f;
}

void SpotLight::GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const
{
    float Far = m_range - m_near;
    M4 matrix = DirectX::XMMatrixPerspectiveFovLH(Clamp(m_angle + 45.0f, 0.0f, 180.0f) * Deg2Rad, 1.0f, m_near, Far);
    Frustum frustum(matrix);
    frustum.Transform(transform->localToWorldMatrix);
    out_arrBoundingHolder[0] = frustum;
}

LightDesc SpotLight::GetLightDesc(ICamera* camera) const
{
    float Far = m_range - m_near;

    LightDesc desc = {};

    desc.DrawShadow = drawShadow;
    desc.Type = LightType::Spot;
    desc.DepthSize = depthSize;
    desc.ShadowWhiteness = shadowWhiteness;
    desc.Intensity = intensity;
    desc.Position = transform->position;
    desc.Direction = transform->forward;
    desc.Range = m_range;
    desc.Angle = m_angle;
    desc.ShadowBias = shadowBias;
    desc.ShadowFadeDistance = shadowFadeDistance;
    desc.ShadowFadeBeginPercent = shadowFadeBeginPercent;
    desc.Diffuse = diffuse;
    desc.Ambient = ambient;
    desc.Near = m_near;
    desc.Far = Far;
    desc.ViewMatrix[0] = DirectX::XMMatrixLookAtLH(desc.Position, desc.Position + desc.Direction, transform->up);
    desc.ProjectionMatrix[0] = DirectX::XMMatrixPerspectiveFovLH(Clamp(m_angle * 2.0f, 0.0f, 180.0f) * Deg2Rad, 1.0f, m_near, Far);

    return desc;
}

bool SpotLight::ContainsInCamera(ICamera* camera) const
{
    float Far = m_range - m_near;
    BoundingHolder frustum;
    GetBoundingHolders(camera, &frustum);
    return camera->Intersects(frustum);
}

FRect SpotLight::GetDeferredScreenQuad(ICamera* camera) const
{
    const M4& V = camera->GetViewMatrix();
    const M4& P = camera->GetProjectionMatrix();

    float radius = m_range / Cos(m_angle * Deg2Rad);
    OrientedBounds obb(transform->position, radius * V3::one(), camera->GetRotation());
    V3 corners[8];
    obb.GetCorners(corners);

    V2 min = V2(FLT_MAX, FLT_MAX);
    V2 max = V2(FLT_MIN, FLT_MIN);

    for (uint i = 0; i < 8; ++i)
    {
        V4 wCorner = V4(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        V4 vCorner = wCorner * V;
        V4 pCorner = vCorner * P;
        V4 ndcCorner = pCorner / pCorner.w;
        V2 sCorner = V2(ndcCorner.x, ndcCorner.y);
        min = V2::Min(min, sCorner);
        max = V2::Max(max, sCorner);
    }

    min.x = Clamp(min.x, -1.0f, 1.0f);
    min.y = Clamp(min.y, -1.0f, 1.0f);
    max.x = Clamp(max.x, -1.0f, 1.0f);
    max.y = Clamp(max.y, -1.0f, 1.0f);

    return FRect(min, max);
}

VolumetricDesc SpotLight::GetVolumetricDesc() const
{
    VolumetricDesc desc;
    desc.DrawVolumetric = volumetricLightState;
    desc.NumSamples = volumetricLightNumSamples;
    desc.Intensity = volumetricLightIntensity;
    desc.Power = volumetricLightPower;

    return desc;
}

bool SpotLight::IsWorking() const
{
    if (!Light::IsWorking())
        return false;

    return m_range > 0.0f && m_angle > 0.0f;
}
