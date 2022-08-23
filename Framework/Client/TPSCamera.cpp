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
    //m_camera->drawGBuffer = true;

    EventSystem::RegistListener(this);
}

void TPSCamera::Start()
{
    SetCursorToCenter();
    ShowCursor(false);
}

void TPSCamera::Update()
{
    if (ImGui::Begin("Post Process"))
    {
        BloomDesc bloomDesc = m_camera->GetBloomDesc();

        ImGui::SliderFloat("Intensity", &bloomDesc.Intensity, 0.0f, 5.0f);
        ImGui::SliderFloat("Threshold", &bloomDesc.Threshold, 0.0f, 0.5f);
        ImGui::SliderFloat("BlurPixelDistance", &bloomDesc.BlurPixelDistance, 0.0f, 50.0f);

        m_camera->SetBloomDesc(bloomDesc);

        ImGui::End();
    }

    if (system->input->GetKeyDown(Key::Return))
    {
        m_rotate = !m_rotate;
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
