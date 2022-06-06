#include "stdafx.h"
#include "FreeCamera.h"

void FreeCamera::Awake()
{
	m_camera = gameObject->AddComponent<Camera>();
    m_camera->projection = Camera::Projection::Perspective;
    m_listener = gameObject->AddComponent<AudioListener>();
}

void FreeCamera::Start()
{
    SetCursorToCenter();
}

void FreeCamera::Update()
{
    UpdateTranslation();

    if (system->input->GetKeyDown(Key::RightMouse))
    {
        SetCursorToCenter();
        m_rotation = true;
        ShowCursor(FALSE);
    }
    if (system->input->GetKeyUp(Key::RightMouse))
    {
        m_rotation = false;
        ShowCursor(TRUE);
    }

    if (m_rotation)
        UpdateRotation();
    
    if (system->input->GetKeyDown(Key::Space) || system->input->GetKeyDown(Key::Q))
    {
        uint r = rand() % 4;

        GameObject* go = CreateGameObject();
        Rigidbody* rigidbody = go->AddComponent<Rigidbody>();
        go->transform->position = transform->position;
        go->transform->localScale = V3::one() * float(rand() % 100 + 50) * 0.01f;
        rigidbody->velocity = system->input->GetRayInWorldSpace(system->graphic->cameraManager->mainCamera).direction * 10.0f;
        rigidbody->linearDamping = 0.5f;
        rigidbody->angularDamping = 0.5f;
        rigidbody->SetInterpolateMode(Rigidbody::Interpolate::Interpolate);

        if (system->input->GetKey(Key::Q))
        {
            r = 4;
        }

        switch (r)
        {
            case 0:
            {
                BoxCollider* boxCollider = go->AddComponent<BoxCollider>();
                boxCollider->restitution = 0.1f;
                go->AddComponent<MeshRenderer>()->mesh = system->resource->builtIn->boxMesh;
            }
            break;
            case 1:
            {
                CapsuleCollider* capsuleCollider = go->AddComponent<CapsuleCollider>();
                capsuleCollider->restitution = 0.1f;
                go->AddComponent<MeshRenderer>()->mesh = system->resource->builtIn->capsuleMesh;
            }
            break;
            case 2:
            {
                SphereCollider* sphereCollider = go->AddComponent<SphereCollider>();
                sphereCollider->restitution = 0.1f;
                go->AddComponent<MeshRenderer>()->mesh = system->resource->builtIn->sphereMesh;
            }
            break;
            case 3:
            {
                ConvexCollider* convexCollider = go->AddComponent<ConvexCollider>();
                convexCollider->restitution = 0.1f;
                go->AddComponent<MeshRenderer>()->mesh = system->resource->builtIn->cylinderMesh;
            }
            break;
            case 4:
            {
                SphereCollider* sphereCollider = go->AddComponent<SphereCollider>();
                sphereCollider->restitution = 0.0f;

                float R = float(rand() % 255) / 255.0f;
                float G = float(rand() % 255) / 255.0f;
                float B = float(rand() % 255) / 255.0f;

                PointLight* pointLight = go->AddComponent<PointLight>();
                pointLight->range = 15.0f + float(rand() % 20 - 10);
                pointLight->enable = true;
                pointLight->diffuse = Color(R, G, B, 1.0f);
            }
            break;
        }
    }
}

void FreeCamera::UpdateTranslation()
{
    V3 camPos = transform->position;
    float speedFactor = 1.0f;
    if (system->input->GetKey(Key::LShift))
        speedFactor = m_translationFactor;
    if (system->input->GetKey(Key::A))
        camPos -= transform->right * system->time->deltaTime * speedFactor;
    if (system->input->GetKey(Key::D))
        camPos += transform->right * system->time->deltaTime * speedFactor;
    if (system->input->GetKey(Key::W))
        camPos += transform->forward * system->time->deltaTime * speedFactor;
    if (system->input->GetKey(Key::S))
        camPos -= transform->forward * system->time->deltaTime * speedFactor;
    if (system->input->GetMouseWheelDelta() > 0.0f)
        camPos += transform->forward * m_translationSpeed * m_translationFactor;
    if (system->input->GetMouseWheelDelta() < 0.0f)
        camPos -= transform->forward * m_translationSpeed * m_translationFactor;
    transform->position = camPos;
}

void FreeCamera::UpdateRotation()
{
    if (system->input->GetKey(Key::RightMouse))
    {
        V2 delta = DeltaCursor();

        V3 camEulerAngles = m_camera->transform->localEulerAngles;
        camEulerAngles.y += delta.x * m_rotationSpeed;
        camEulerAngles.x += delta.y * m_rotationSpeed;

        camEulerAngles.x = Clamp(camEulerAngles.x, -89.0f, +89.0f);
        camEulerAngles.z = Clamp(camEulerAngles.z, -89.0f, +89.0f);

        m_camera->transform->localEulerAngles = camEulerAngles;

        SetCursorToCenter();
    }
}

V2 FreeCamera::DeltaCursor() const
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

void FreeCamera::SetCursorToCenter()
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
