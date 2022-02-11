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

    desc.Type = LightType::Point;
    desc.DrawShadow = m_drawShadow;
    desc.ShadowWhiteness = m_shadowWhiteness;
    desc.Intensity = m_intensity;
    desc.Position = transform->position;
    desc.Direction = transform->forward;
    desc.Range = m_range;
    desc.Angle = 0;
    desc.Diffuse = m_diffuse;
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
    desc.ProjectionMatrix[5] = DirectX::XMMatrixPerspectiveFovLH(90.0f * Deg2Rad, 1.0f, m_near, Far);

    return desc;
}

bool PointLight::ContainsInCamera(ICamera* camera) const
{
    Sphere sphere(transform->position, m_range);
    return camera->Intersects(sphere);
}

bool PointLight::IsWorking() const
{
    if (!Light::IsWorking())
        return false;

    return m_range > 0.0f;
}

void PointLight::SetupRotations()
{
    // const V3	m_arrDirection[6] = { V3::right(),V3::left(),V3::forward(),V3::back(),V3::up(),V3::down(), };

    m_arrRotations[0] = Q::Euler(0, +90, 0);
    m_arrRotations[1] = Q::Euler(0, -90, 0);
    m_arrRotations[2] = Q::Euler(0, +000, 0);
    m_arrRotations[3] = Q::Euler(0, +180, 0);
    m_arrRotations[4] = Q::Euler(-90, 0, 0);
    m_arrRotations[5] = Q::Euler(+90, 0, 0);
}
