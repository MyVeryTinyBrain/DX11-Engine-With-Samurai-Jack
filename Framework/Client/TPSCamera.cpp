#include "stdafx.h"
#include "TPSCamera.h"
#include "Config.h"
#include "Enemy.h"
#include "EventSystem.h"

void TPSCamera::Awake()
{
	m_goCamera = CreateGameObjectToChild(transform);
    m_camera = m_goCamera->AddComponent<Camera>();
    m_goCamera->AddComponent<AudioListener>();

    EventSystem::RegistListener(this);

    m_camera->postProcessingState = true;

    SSAODesc ssao = m_camera->ssaoDesc;
    ssao.Enable = true;
    ssao.NumSamples = 4;
    ssao.BlurNumSamples = 8;
    ssao.Transparency = 0.225f;
    ssao.MinZ = 0.1f;
    ssao.Radius = 0.335f;
    ssao.Power = 1.25f;
    ssao.BlurPixelDistance = 2000.0f;

    SSRDesc ssr = m_camera->ssrDesc;
    ssr.Enable = false;
    ssr.BlurEnable = true;
    ssr.BlurType = BlurType::InvDepth;
    ssr.NumSamples = 120;
    ssr.BlurNumSamples = 4;
    ssr.Step = 0.4f;
    ssr.Thickness = 0.7f;
    ssr.Bias = 0.04f;
    ssr.BlurPixelDistance = 3000.0f;
    ssr.ResolutionScale = 0.5f;

    DOFDesc dof = m_camera->dofDesc;
    dof.Enable = true;
    dof.BlurNumSamples = 8;
    dof.MinZ = 20.0f;
    dof.RangeZ = 30.0f;
    dof.Power = 1.0f;
    dof.BlurPixelDistance = 10.0f;

    FogDesc fog = m_camera->fogDesc;
    fog.Enable = false;
    fog.Type = FogType::Distance;
    fog.MinZ = 30.0f;
    fog.RangeZ = 40.0f;
    fog.Color = Color::white();

    BloomDesc bloom = m_camera->bloomDesc;
    bloom.Enable = true;
    bloom.BlurNumSamples = 16;
    bloom.Intensity = 1.834f;
    bloom.Threshold = 0.324f;
    bloom.BlurPixelDistance = 13.312f;

    ChromaticAberrationDesc ca = m_camera->chromaticAberrationDesc;
    ca.Enable = true;
    ca.StartSeperate = 0.0f;
    ca.MaxSeperate = 1.0f;
    ca.Blend = V4::one();
    ca.Offset = V4::one() * 2.5f;
    ca.Angle = V4(0, 120, 240, 0);

    m_camera->ssaoDesc = ssao;
    m_camera->ssrDesc = ssr;
    m_camera->dofDesc = dof;
    m_camera->fogDesc = fog;
    m_camera->bloomDesc = bloom;
    m_camera->chromaticAberrationDesc = ca;
}

void TPSCamera::Start()
{
    SetCursorToCenter();
    ShowCursor(false);
}

void TPSCamera::Update()
{
    if (m_imgui) {
        {
            ImGui::Begin("Info", 0, ImGuiWindowFlags_AlwaysAutoResize);

            tstring resolutionTxt = tstring_format(TEXT("resolution: %d x %d"), int(system->graphic->GetWidth()), int(system->graphic->GetHeight()));
            ImGui::Text(tstring_to_str_utf8(resolutionTxt).c_str());

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

#ifdef _DEBUG
            ImGui::Text("Debug Mode");
#else
            ImGui::Text("Release Mode");
#endif

            ImGui::End();
        }

        Camera* camera = (Camera*)system->graphic->cameraManager->mainCamera;
        ImGui::Begin("Camera");
        if (ImGui::CollapsingHeader("Camera")) {
            ImGui::PushID("Camera");

            bool drawGBuffer = camera->drawGBuffer;
            ImGui::Checkbox("Draw G Buffer", &drawGBuffer);
            camera->drawGBuffer = drawGBuffer;

            float Near = camera->Near;
            ImGui::SliderFloat("Near", &Near, 0.0f, camera->Far);
            camera->Near = Near;

            float Far = camera->Far;
            ImGui::SliderFloat("Far", &Far, 0.0f, 1000.0f);
            camera->Far = Far;

            float FOV = camera->fov;
            ImGui::SliderFloat("FOV", &FOV, 0.0f, 180.0f);
            camera->fov = FOV;

            bool postProcessing = camera->postProcessingState;
            ImGui::Checkbox("Enable", &postProcessing);
            camera->postProcessingState = postProcessing;

            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("SSAO")) {
            ImGui::PushID("SSAO");

            SSAODesc ssaoDesc = camera->ssaoDesc;

            bool enable = ssaoDesc.Enable;
            ImGui::Checkbox("Enable", &enable);
            ssaoDesc.Enable = enable;

            int numSamples = (int)ssaoDesc.NumSamples;
            ImGui::SliderInt("NumSamples", &numSamples, 0, 16);
            ssaoDesc.NumSamples = (uint)numSamples;

            float transparency = ssaoDesc.Transparency;
            ImGui::SliderFloat("Transparency", &transparency, 0.0f, 1.0f);
            ssaoDesc.Transparency = transparency;

            float minZ = ssaoDesc.MinZ;
            ImGui::SliderFloat("MinZ", &minZ, 0.0f, 0.1f);
            ssaoDesc.MinZ = minZ;

            float radius = ssaoDesc.Radius;
            ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
            ssaoDesc.Radius = radius;

            float power = ssaoDesc.Power;
            ImGui::SliderFloat("Power", &power, 0.0f, 10.0f);
            ssaoDesc.Power = power;

            int blurNumSamples = (int)ssaoDesc.BlurNumSamples;
            ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
            ssaoDesc.BlurNumSamples = (uint)blurNumSamples;

            float blurPixelDistance = ssaoDesc.BlurPixelDistance;
            ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 2000.0f);
            ssaoDesc.BlurPixelDistance = blurPixelDistance;

            camera->ssaoDesc = ssaoDesc;

            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("SSR")) {
            ImGui::PushID("SSR");

            SSRDesc ssrDesc = camera->ssrDesc;

            bool enable = ssrDesc.Enable;
            ImGui::Checkbox("Enable", &enable);
            ssrDesc.Enable = enable;

            bool blurEnable = ssrDesc.BlurEnable;
            ImGui::Checkbox("BlurEnable", &blurEnable);
            ssrDesc.BlurEnable = blurEnable;

            const char* items[] = { "Default", "InvDepth", "Depth" };
            int blurType = (int)ssrDesc.BlurType;
            ImGui::Combo("BlurType", &blurType, items, 3);
            ssrDesc.BlurType = (BlurType)blurType;

            int numSamples = (int)ssrDesc.NumSamples;
            ImGui::SliderInt("NumSamples", &numSamples, 0, 256);
            ssrDesc.NumSamples = (uint)numSamples;

            int blurNumSamples = (int)ssrDesc.BlurNumSamples;
            ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
            ssrDesc.BlurNumSamples = (uint)blurNumSamples;

            float step = ssrDesc.Step;
            ImGui::SliderFloat("Step", &step, 0.0f, 0.4f);
            ssrDesc.Step = step;

            float thickness = ssrDesc.Thickness;
            ImGui::SliderFloat("Thickness", &thickness, 0.0f, 1.0f);
            ssrDesc.Thickness = thickness;

            float bias = ssrDesc.Bias;
            ImGui::SliderFloat("Bias", &bias, 0.0f, 1.0f);
            ssrDesc.Bias = bias;

            float blurPixelDistance = ssrDesc.BlurPixelDistance;
            if (ssrDesc.BlurType == BlurType::InvDepth)
                ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 3000.0f);
            else
                ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 100.0f);
            ssrDesc.BlurPixelDistance = blurPixelDistance;

            float resolutionScale = ssrDesc.ResolutionScale;
            ImGui::SliderFloat("ResolutionScale", &resolutionScale, 0.0f, 1.0f);
            ssrDesc.ResolutionScale = resolutionScale;

            camera->ssrDesc = ssrDesc;

            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("DOF")) {
            ImGui::PushID("DOF");

            DOFDesc dofDesc = camera->dofDesc;

            bool enable = dofDesc.Enable;
            ImGui::Checkbox("Enable", &enable);
            dofDesc.Enable = enable;

            int blurNumSamples = (int)dofDesc.BlurNumSamples;
            ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
            dofDesc.BlurNumSamples = (uint)blurNumSamples;

            float minZ = dofDesc.MinZ;
            ImGui::SliderFloat("MinZ", &minZ, 0.0f, 100.0f);
            dofDesc.MinZ = minZ;

            float rangeZ = dofDesc.RangeZ;
            ImGui::SliderFloat("RangeZ", &rangeZ, 0.0f, 100.0f);
            dofDesc.RangeZ = rangeZ;

            float power = dofDesc.Power;
            ImGui::SliderFloat("Power", &power, 0.0f, 100.0f);
            dofDesc.Power = power;

            float blurPixelDistance = dofDesc.BlurPixelDistance;
            ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 1000.0f);
            dofDesc.BlurPixelDistance = blurPixelDistance;

            camera->dofDesc = dofDesc;

            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Fog")) {
            ImGui::PushID("Fog");

            FogDesc fogDesc = camera->fogDesc;

            bool enable = fogDesc.Enable;
            ImGui::Checkbox("Enable", &enable);
            fogDesc.Enable = enable;

            const char* items[] = { "Distance", "Z" };
            int type = (int)fogDesc.Type;
            ImGui::Combo("Type", &type, items, 2);
            fogDesc.Type = (FogType)type;

            float minZ = fogDesc.MinZ;
            ImGui::SliderFloat("MinZ", &minZ, 0.0f, 100.0f);
            fogDesc.MinZ = minZ;

            float rangeZ = fogDesc.RangeZ;
            ImGui::SliderFloat("RangeZ", &rangeZ, 0.0f, 100.0f);
            fogDesc.RangeZ = rangeZ;

            Color color = fogDesc.Color;
            ImGui::ColorPicker4("Color", (float*)&color);
            fogDesc.Color = color;

            camera->fogDesc = fogDesc;

            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Bloom")) {
            ImGui::PushID("Bloom");

            BloomDesc bloomDesc = camera->bloomDesc;

            bool enable = bloomDesc.Enable;
            ImGui::Checkbox("Enable", &enable);
            bloomDesc.Enable = enable;

            int blurNumSamples = (int)bloomDesc.BlurNumSamples;
            ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
            bloomDesc.BlurNumSamples = (uint)blurNumSamples;

            float intensity = bloomDesc.Intensity;
            ImGui::SliderFloat("Intensity", &intensity, 0.0f, 100.0f);
            bloomDesc.Intensity = intensity;

            float threshold = bloomDesc.Threshold;
            ImGui::SliderFloat("Threshold", &threshold, 0.0f, 1.0f);
            bloomDesc.Threshold = threshold;

            float blurPixelDistance = bloomDesc.BlurPixelDistance;
            ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 100.0f);
            bloomDesc.BlurPixelDistance = blurPixelDistance;

            camera->bloomDesc = bloomDesc;

            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("ChromaticAberration")) {
            ImGui::PushID("ChromaticAberration");

            ChromaticAberrationDesc chromaticAberrationDesc = camera->chromaticAberrationDesc;

            bool enable = chromaticAberrationDesc.Enable;
            ImGui::Checkbox("Enable", &enable);
            chromaticAberrationDesc.Enable = enable;

            ImGui::SliderFloat("StartSeperatestance", &chromaticAberrationDesc.StartSeperate, 0.0f, 1.0f);

            ImGui::SliderFloat("MaxSeperate", &chromaticAberrationDesc.MaxSeperate, 0.0f, 1.0f);

            ImGui::SliderFloat3("Blend", (float*)&chromaticAberrationDesc.Blend, 0.0f, 1.0f);

            ImGui::SliderFloat3("Offset", (float*)&chromaticAberrationDesc.Offset, 0.0f, 50.0f);

            ImGui::SliderFloat3("Angle", (float*)&chromaticAberrationDesc.Angle, 0.0f, 360.0f);

            camera->chromaticAberrationDesc = chromaticAberrationDesc;

            ImGui::PopID();
        }
        ImGui::End();
    }

    if (system->input->GetKeyDown(Key::Return))
    {
        m_rotate = !m_rotate;
        m_imgui = !m_imgui;
        if (m_rotate)
        {
            SetCursorToCenter();
            ShowCursor(false);
        }
        else
        {
            ShowCursor(true);
        }
    }

    if (m_rotate)
        UpdateRotation();
    UpdateShake();
    UpdatePosition();
    UpdateDistance();
}

void TPSCamera::OnDestroyed()
{
    EventSystem::UnregistListener(this);
}

void TPSCamera::OnEvent(const string& msg, void* pContext)
{
    if (msg == EVENT_ENEMY_DIE)
    {
        if (m_lookTarget)
            LookNearEnemy();
    }
}

void TPSCamera::SetLookTarget(const Ref<Transform>& target)
{
    m_lookTarget = target;
}

void TPSCamera::LookNearEnemy()
{
    Enemy* nearEnemy = FindNearEnemy();
    if (nearEnemy)
    {
        SetLookTarget(nearEnemy->transform);
    }
    else
    {
        Unlook();
    }
}

void TPSCamera::Unlook()
{
    SetLookTarget(nullptr);
}

void TPSCamera::Shake(const V3& direction, float power, float square, float duration, float shakePerSec)
{
    Shake_t shake;
    shake.direction = direction;
    shake.duration = duration;
    shake.power = power;
    shake.square = square;
    shake.shakePerSec = shakePerSec;
    shake.elapsed = 0.0f;
    m_shakes.push_back(shake);
}

Enemy* TPSCamera::FindNearEnemy() const
{
    // 카메라의 forward 방향과 가장 작은 각도차를 가지는 적을 찾습니다.

    float nearAngle = FLT_MAX;
    Enemy* nearEnemy = nullptr;

    uint numEnemies = Enemy::GetEnemyCount();
    for (uint i = 0; i < numEnemies; ++i)
    {
        Enemy* enemy = Enemy::GetEnemyByIndex(i);
        if (enemy->isDead) continue;

        V3 toEnemyVector = enemy->transform->position - m_goCamera->transform->position;
        float angle = V3::Angle(m_goCamera->transform->forward, toEnemyVector);

        if (angle < nearAngle)
        {
            nearAngle = angle;
            nearEnemy = enemy;
        }
    }

    return nearEnemy;
}

Enemy* TPSCamera::FindOtherNearEnemy(Transform* selected, const V3& deltaMouse) const
{
    // 현재 선택된 적과 마우스 방향으로 가장 가까운 다른 적을 찾습니다.

    V2 ndcDirection;
    ndcDirection.x = deltaMouse.x;
    ndcDirection.y = -deltaMouse.y;
    ndcDirection.Normalize();

    float enemyDistance;
    float nearDistance = FLT_MAX;
    Enemy* nearEnemy = nullptr;

    if (!selected)
    {
        return FindNearEnemy();
    }
    else
    {
        // 먼저 선택된 적의 마우스 방향으로의 투영거리를 구합니다.
        // 이 값은 음수일수 있습니다.
        // 이 값보다 작은 적들은 마우스 방향의 반대에 위치하고 있게 됩니다.
        // 이 값보다 큰 적들을 마우스 방향에 위치하고 있게 됩니다.
        V4 wp = selected->transform->position;
        wp.w = 1.0f;
        V4 vp = wp * m_camera->viewMatrix;
        V4 pp = vp * m_camera->projectionMatrix;
        V2 ndc = pp / pp.w;
        enemyDistance = V2::Dot(ndcDirection, ndc);
    }

    uint numEnemies = Enemy::GetEnemyCount();
    for (uint i = 0; i < numEnemies; ++i)
    {
        Enemy* enemy = Enemy::GetEnemyByIndex(i);

        tstring name = enemy->gameObject->name;

        if (enemy->transform == selected)
            continue;

        V4 wp = enemy->transform->position;
        wp.w = 1.0f;
        V4 vp = wp * m_camera->viewMatrix;
        V4 pp = vp * m_camera->projectionMatrix;
        V2 ndc = pp / pp.w;

        // NDC에서 마우스 방향으로 투영한 적의 거리가 최소인 것을 찾습니다.
        float projectionDistance = V2::Dot(ndcDirection, ndc);
        if (projectionDistance > enemyDistance && projectionDistance < nearDistance)
        {
            nearDistance = projectionDistance;
            nearEnemy = enemy;
        }
    }

    return nearEnemy;
}

void TPSCamera::UpdateRotation()
{
    if (!m_lookTarget)
    {
        V2 delta = DeltaCursor();

        V3 camEulerAngles = transform->localEulerAngles;
        camEulerAngles.y += delta.x * m_manualRotateSpeed;
        camEulerAngles.x += delta.y * m_manualRotateSpeed;

        camEulerAngles.x = Clamp(camEulerAngles.x, -89.0f, +89.0f);
        camEulerAngles.z = Clamp(camEulerAngles.z, -89.0f, +89.0f);

        transform->localEulerAngles = camEulerAngles;
    }
    else
    {
        V3 lookVector = m_lookTarget->position - m_goCamera->transform->position;
        V3 xzLookVector = lookVector;
        xzLookVector.y = 0;
        xzLookVector.Normalize();
        V3 up = V3::up();
        Q targetRotation = Q::LookRotation(lookVector.normalized, up);
        Q rotation = Q::SLerp(transform->rotation, targetRotation, m_xztracePower * system->time->deltaTime * m_lookRotateSpeed * 10.0f);

        transform->rotation = rotation;

        V3 camEulerAngles = transform->localEulerAngles;
        camEulerAngles.x = Clamp(camEulerAngles.x, -89.0f, +89.0f);
        camEulerAngles.z = Clamp(camEulerAngles.z, -89.0f, +89.0f);
        transform->localEulerAngles = camEulerAngles;
        
        m_lookChangeDelayCount -= system->time->deltaTime;

        V2 deltaCursor = DeltaCursor();
        if (m_lookChangeDelayCount <= 0.0f && deltaCursor.magnitude > m_minLookChangeDelta)
        {
            Enemy* otherEnemy = FindOtherNearEnemy(m_lookTarget, deltaCursor);
            if (otherEnemy)
            {
                SetLookTarget(otherEnemy->transform);
                m_lookChangeDelayCount = m_lookChangeDelay;
            }
        }
    }

    SetCursorToCenter();
}

void TPSCamera::UpdateShake()
{
    m_shakeTranslation = {};

    if (m_shakes.empty())
        return;

    float dt = system->time->deltaTime;
    for (auto it = m_shakes.begin(); it != m_shakes.end();)
    {
        Shake_t& shake = *it;
        shake.elapsed += dt;
        float f = Sin(shake.elapsed * PI * shake.shakePerSec);
        float p = (1.0f - pow(Max(0.0f, shake.elapsed / shake.duration), shake.square)) * shake.power;
        m_shakeTranslation += shake.direction * f * p;
        if (shake.elapsed > shake.duration)
            it = m_shakes.erase(it);
        else
            ++it;
    }
}

void TPSCamera::UpdatePosition()
{
    if (!m_traceTarget)
        return;

    V3 camPos = transform->position;
    V3 targetPos = CalcTargetPos();
    V3 newCamPos;
    newCamPos.x = Lerp(camPos.x, targetPos.x, m_xztracePower * system->time->deltaTime);
    newCamPos.z = Lerp(camPos.z, targetPos.z, m_xztracePower * system->time->deltaTime);
    newCamPos.y = Lerp(camPos.y, targetPos.y, m_ytracePower * system->time->deltaTime);
    newCamPos += m_shakeTranslation;

    transform->position = newCamPos;
}

void TPSCamera::UpdateDistance()
{
    if (!m_traceTarget)
        return;

    {   // Back distance
        float distance = m_distance;
        V3 targetPos = CalcTargetPos();

        vector<PhysicsHit> hits;
        PhysicsRay ray;
        ray.Direction = -m_goCamera->transform->forward;
        ray.Length = m_distance + 100.0f;
        ray.Point = targetPos;
        hits = system->physics->query->RaycastAll(ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider);
        if (!hits.empty())
        {
            for (auto& hit : hits)
            {
                if (hit.Collider->rigidbody->gameObject->tag != TAG_FIGHT_COLLIDER)
                {
                    float hitDistance = Max(m_camera->Near, hit.Distance - m_camera->Near - 1.0f);
                    distance = Min(distance, hitDistance);
                }
            }
        }

        m_goCamera->transform->localPosition = V3(0, 0, -distance);
    }
    //{   // Right distance
    //    float distance = 1.0f;
    //    vector<PhysicsHit> hits;
    //    PhysicsRay ray;
    //    ray.Direction = m_goCamera->transform->right;
    //    ray.Length = 100.0f;
    //    ray.Point = m_goCamera->transform->position;
    //    hits = system->physics->query->RaycastAll(ray, 1 << PhysicsLayer_Default, PhysicsQueryType::Collider);

    //    if (!hits.empty())
    //    {
    //        for (auto& hit : hits)
    //        {
    //            if (hit.Collider->rigidbody->gameObject->tag != TAG_FIGHT_COLLIDER)
    //            {
    //                distance = Min(distance, hit.Distance);
    //            }
    //        }
    //    }

    //    V3 localPos = m_goCamera->transform->localPosition;
    //    localPos.x = distance;
    //    m_goCamera->transform->localPosition = localPos;
    //}
}

V2 TPSCamera::DeltaCursor() const
{
    HWND hWnd = system->graphic->windowHandle;

    RECT rect = {};
    GetClientRect(hWnd, &rect);

    POINT point = {};
    GetCursorPos(&point);
    ScreenToClient(hWnd, &point);

    POINT center = { rect.right / 2, rect.bottom / 2 };

    V2 vPoint = V2(float(point.x), float(point.y));
    V2 vCenter = V2(float(center.x), float(center.y));
    V2 delta = vPoint - vCenter;

    return delta;
}

void TPSCamera::SetCursorToCenter()
{
    HWND hWnd = system->graphic->windowHandle;

    RECT rect = {};
    GetClientRect(hWnd, &rect);

    POINT point = {};
    GetCursorPos(&point);
    ScreenToClient(hWnd, &point);

    POINT screenCenter = { rect.right / 2, rect.bottom / 2 };
    ClientToScreen(hWnd, &screenCenter);
    SetCursorPos(int(screenCenter.x), int(screenCenter.y));
}

V3 TPSCamera::CalcTargetPos() const
{
    return m_traceTarget->position + V3::up() * m_height;
    //return m_traceTarget->position + V3::up() * m_height;
}
