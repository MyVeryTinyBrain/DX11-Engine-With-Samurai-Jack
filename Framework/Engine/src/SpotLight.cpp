#include "EnginePCH.h"
#include "SpotLight.h"
#include "Transform.h"
#include "ICamera.h"

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

    desc.Type = LightType::Spot;
    desc.DrawShadow = m_drawShadow;
    desc.ShadowWhiteness = m_shadowWhiteness;
    desc.Intensity = m_intensity;
    desc.Position = transform->position;
    desc.Direction = transform->forward;
    desc.Range = m_range;
    desc.Angle = m_angle;
    desc.Diffuse = m_diffuse;
    desc.Near = m_near;
    desc.Far = Far;
    desc.ViewMatrix[0] = DirectX::XMMatrixLookAtLH(desc.Position, desc.Position + desc.Direction, transform->up);
    desc.ProjectionMatrix[0] = DirectX::XMMatrixPerspectiveFovLH(Clamp(m_angle + 45.0f, 0.0f, 180.0f) * Deg2Rad, 1.0f, m_near, Far);

    return desc;
}

bool SpotLight::ContainsInCamera(ICamera* camera) const
{
    float Far = m_range - m_near;
    BoundingHolder frustum;
    GetBoundingHolders(camera, &frustum);
    return camera->Intersects(frustum);
}

bool SpotLight::IsWorking() const
{
    if (!Light::IsWorking())
        return false;

    return m_range > 0.0f && m_angle > 0.0f;
}
