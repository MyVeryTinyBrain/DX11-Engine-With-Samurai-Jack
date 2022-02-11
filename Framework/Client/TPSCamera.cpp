#include "stdafx.h"
#include "TPSCamera.h"

void TPSCamera::Awake()
{
	m_goCamera = CreateGameObjectToChild(transform);
    m_camera = m_goCamera->AddComponent<Camera>();
}

void TPSCamera::Start()
{
    SetCursorToCenter();
    ShowCursor(false);
}

void TPSCamera::Update()
{
    if (system->input->GetKeyDown(Key::Return))
    {
        m_rotation = !m_rotation;
        if (m_rotation)
        {
            SetCursorToCenter();
            ShowCursor(false);
        }
        else
        {
            ShowCursor(true);
        }
    }

    if (m_rotation)
        UpdateRotation();
    UpdateDistance();
    UpdatePosition();
}

void TPSCamera::UpdateRotation()
{
    V2 delta = DeltaCursor();

    V3 camEulerAngles = transform->localEulerAngles;
    camEulerAngles.y += delta.x * m_rotationSpeed;
    camEulerAngles.x += delta.y * m_rotationSpeed;

    camEulerAngles.x = Clamp(camEulerAngles.x, -89.0f, +89.0f);
    camEulerAngles.z = Clamp(camEulerAngles.z, -89.0f, +89.0f);

    transform->localEulerAngles = camEulerAngles;

    SetCursorToCenter();
}

void TPSCamera::UpdateDistance()
{
    m_goCamera->transform->localPosition = V3(0, 0, -m_distance);
}

void TPSCamera::UpdatePosition()
{
    if (!m_target)
        return;

    V3 camPos = transform->position;
    V3 targetPos = m_target->position + V3::up() * m_height;
    V3 newCamPos = V3::Lerp(camPos, targetPos, m_tracePower * system->time->deltaTime);

    transform->position = newCamPos;
}

V2 TPSCamera::DeltaCursor() const
{
    HWND hWnd = system->graphicSystem->windowHandle;

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
    HWND hWnd = system->graphicSystem->windowHandle;

    RECT rect = {};
    GetClientRect(hWnd, &rect);

    POINT point = {};
    GetCursorPos(&point);
    ScreenToClient(hWnd, &point);

    POINT screenCenter = { rect.right / 2, rect.bottom / 2 };
    ClientToScreen(hWnd, &screenCenter);
    SetCursorPos(int(screenCenter.x), int(screenCenter.y));
}
