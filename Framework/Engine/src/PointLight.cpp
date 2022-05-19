#include "EnginePCH.h"
#include "PointLight.h"
#include "Transform.h"
#include "ICamera.h"

void PointLight::Awake()
{
    Light::Awake();
    SetupRotations();
}

void PointLight::GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const
{
    float Far = m_range - m_near;
    const M4& matrix = DirectX::XMMatrixPerspectiveFovLH(90.0f * Deg2Rad, 1.0f, m_near, 1000);
    const V3& viewPosition = transform->position;

    Frustum frustums[6] = {};

    frustums[0] = Frustum(matrix);
    frustums[1] = Frustum(matrix);
    frustums[2] = Frustum(matrix);
    frustums[3] = Frustum(matrix);
    frustums[4] = Frustum(matrix);
    frustums[5] = Frustum(matrix);

    frustums[0].Transform(M4::SRT(V3::one(), m_arrRotations[0], viewPosition));
    frustums[1].Transform(M4::SRT(V3::one(), m_arrRotations[1], viewPosition));
    frustums[2].Transform(M4::SRT(V3::one(), m_arrRotations[2], viewPosition));
    frustums[3].Transform(M4::SRT(V3::one(), m_arrRotations[3], viewPosition));
    frustums[4].Transform(M4::SRT(V3::one(), m_arrRotations[4], viewPosition));
    frustums[5].Transform(M4::SRT(V3::one(), m_arrRotations[5], viewPosition));

    out_arrBoundingHolder[0] = frustums[0];
    out_arrBoundingHolder[1] = frustums[1];
    out_arrBoundingHolder[2] = frustums[2];
    out_arrBoundingHolder[3] = frustums[3];
    out_arrBoundingHolder[4] = frustums[4];
    out_arrBoundingHolder[5] = frustums[5];
}

LightDesc PointLight::GetLightDesc(ICamera* camera) const
{
    float Far = m_range - m_near;
    const V3& viewPosition = transform->position;

    LightDesc desc = {};

    desc.DrawShadow = drawShadow;
    desc.Type = LightType::Point;
    desc.DepthSize = depthSize;
    desc.ShadowWhiteness = shadowWhiteness;
    desc.Intensity = intensity;
    desc.Position = transform->position;
    desc.Direction = transform->forward;
    desc.Range = m_range;
    desc.Angle = 0;
    desc.ShadowBias = shadowBias;
    desc.ShadowFadeDistance = shadowFadeDistance;
    desc.ShadowFadeBeginPercent = shadowFadeBeginPercent;
    desc.Diffuse = diffuse;
    desc.Ambient = ambient;
    desc.Near = m_near;
    desc.Far = Far;
    desc.ViewMatrix[0] = DirectX::XMMatrixLookAtLH(viewPosition, viewPosition + m_arrDirection[0], m_arrUp[0]);
    desc.ViewMatrix[1] = DirectX::XMMatrixLookAtLH(viewPosition, viewPosition + m_arrDirection[1], m_arrUp[1]);
    desc.ViewMatrix[2] = DirectX::XMMatrixLookAtLH(viewPosition, viewPosition + m_arrDirection[2], m_arrUp[2]);
    desc.ViewMatrix[3] = DirectX::XMMatrixLookAtLH(viewPosition, viewPosition + m_arrDirection[3], m_arrUp[3]);
    desc.ViewMatrix[4] = DirectX::XMMatrixLookAtLH(viewPosition, viewPosition + m_arrDirection[4], m_arrUp[4]);
    desc.ViewMatrix[5] = DirectX::XMMatrixLookAtLH(viewPosition, viewPosition + m_arrDirection[5], m_arrUp[5]);
    desc.ProjectionMatrix[0] = 
    desc.ProjectionMatrix[1] = 
    desc.ProjectionMatrix[2] = 
    desc.ProjectionMatrix[3] = 
    desc.ProjectionMatrix[4] = 
    desc.ProjectionMatrix[5] = DirectX::XMMatrixPerspectiveFovLH(90.1f * Deg2Rad, 1.0f, m_near, Far);
    // 90.0f 사용시 부동 소수점 오차로 인해 경계가 제대로 그려지지 않습니다.

    return desc;
}

bool PointLight::ContainsInCamera(ICamera* camera) const
{
    Sphere sphere(transform->position, m_range);
    return camera->Intersects(sphere);
}

FRect PointLight::GetDeferredScreenQuad(ICamera* camera) const
{
    const M4& V = camera->GetViewMatrix();
    const M4& P = camera->GetProjectionMatrix();

    OrientedBounds obb(transform->position, m_range * V3::one(), camera->GetRotation());
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

VolumetricDesc PointLight::GetVolumetricDesc() const
{
    VolumetricDesc desc;
    desc.DrawVolumetric = volumetricLightState;
    desc.NumSamples = volumetricLightNumSamples;
    desc.Intensity = volumetricLightIntensity;
    desc.Power = volumetricLightPower;

    return desc;
}

bool PointLight::IsWorking() const
{
    if (!Light::IsWorking())
        return false;

    return m_range > 0.0f;
}

void PointLight::SetupRotations()
{
    // const V3	m_arrDirection[6] = { V3::up(),V3::left(),V3::forward(),V3::right(),V3::back(),V3::down(), };

    m_arrRotations[4] = Q::Euler(-90, 0, 0);
    m_arrRotations[1] = Q::Euler(0, -90, 0);
    m_arrRotations[2] = Q::Euler(0, +000, 0);
    m_arrRotations[0] = Q::Euler(0, +90, 0);
    m_arrRotations[3] = Q::Euler(0, +180, 0);
    m_arrRotations[5] = Q::Euler(+90, 0, 0);
}
