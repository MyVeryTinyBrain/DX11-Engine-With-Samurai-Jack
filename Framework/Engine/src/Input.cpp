#include "EnginePCH.h"
#include "Input.h"
#include "System.h"
#include "GraphicSystem.h"
#include "CameraManager.h"
#include "Camera.h"

Input::Input()
{
}

bool Input::Initialize(System* system)
{
	if (!system)
		return false;

	m_system = system;

	ZeroMemory(&m_pressingState, sizeof(m_pressingState));

	ZeroMemory(&m_capture, sizeof(Buffer));

	ZeroMemory(&m_frame, sizeof(Buffer));

	return true;
}

void Input::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// while (m_lock);

	switch (message)
	{
		case WM_LBUTTONDOWN:
			SetKeyDown((unsigned char)Key::LeftMouse);
			break;
		case WM_RBUTTONDOWN:
			SetKeyDown((unsigned char)Key::RightMouse);
			break;
		case WM_MBUTTONDOWN:
			SetKeyDown((unsigned char)Key::WhileMouse);
			break;
		case WM_LBUTTONUP:
			SetKeyUp((unsigned char)Key::LeftMouse);
			break;
		case WM_RBUTTONUP:
			SetKeyUp((unsigned char)Key::RightMouse);
			break;
		case WM_MBUTTONUP:
			SetKeyUp((unsigned char)Key::WhileMouse);
			break;
		case WM_MOUSEWHEEL:
			SetWheelDelta(HIWORD(wParam));
			break;
		case WM_KEYDOWN:
			SetKeyDown((unsigned char)wParam);
			break;
		case WM_KEYUP:
			SetKeyUp((unsigned char)wParam);
			break;
	}
}

void Input::End()
{
	ZeroMemory(&m_capture, sizeof(Buffer));
}

void Input::Fetch()
{
	HWND hWnd = m_system->graphic->GetWindowHandle();
	if (!hWnd)
		return;

	memcpy(&m_frame, &m_capture, sizeof(Buffer));
}

void Input::SetEnable(bool value)
{
	m_enable = value;
}

bool Input::GetKey(const Key& key)
{
	if (!m_enable)
		return false;

	SHORT state = GetAsyncKeyState((int)key) & 0x8000;
	return state != 0;
}

const bool& Input::GetKeyDown(const Key& key)
{
	return m_frame.DownState[(unsigned char)key];
}

const bool& Input::GetKeyUp(const Key& key)
{
	return m_frame.UpState[(unsigned char)key];
}

const float& Input::GetMouseWheelDelta()
{
	return m_frame.WheelDelta;
}

V3 Input::GetMousePositionInScreen() const
{
	HWND hWnd = m_system->graphic->GetWindowHandle();
	if (!hWnd)
		return V3::zero();

	POINT cursor = {};
	GetCursorPos(&cursor);
	V3 screen = V3(float(cursor.x), float(cursor.y), 0);

	return screen;
}

V3 Input::GetMousePositionInViewport() const
{
	HWND hWnd = m_system->graphic->GetWindowHandle();
	if (!hWnd)
		return V3::zero();

	POINT cursor = {};
	GetCursorPos(&cursor);
	ScreenToClient(hWnd, &cursor);
	V3 client = V3(float(cursor.x), float(cursor.y), 0);

	return client;
}

V3 Input::GetMousePositionInNDC() const
{
	HWND hWnd = m_system->graphic->GetWindowHandle();
	if (!hWnd)
		return V3::zero();

	V3 viewMouse = GetMousePositionInViewport();

	RECT rect = {};
	GetClientRect(hWnd, &rect);

	if (rect.right * rect.bottom == 0)
		return V3::zero();

	float x = viewMouse.x / float(rect.right) * 2.0f - 1.0f;
	float y = (viewMouse.y / float(rect.bottom) * 2.0f - 1.0f) * -1.0f;

	return V3(x, y, 0);
}

V3 Input::GetMousePositionInViewSpace(ICamera* camera) const
{
	if (!camera)
	{
		return V3::zero();
	}

	const M4& projectionMatrix = camera->GetProjectionMatrix();
	const M4& projectionToViewSpaceMatrix = projectionMatrix.inversed;
	const V3& ndcMouse = GetMousePositionInNDC();
	const V3& viewSpaceMouse = projectionToViewSpaceMatrix.MultiplyPoint(ndcMouse);

	return viewSpaceMouse;
}

Ray Input::GetRayInWorldSpace(ICamera* camera) const
{
	if (!camera)
	{
		return Ray(V3::zero(), V3::zero());
	}

	const M4& worldToViewMatrix = camera->GetViewMatrix();
	const M4& viewToWorldMatrix = worldToViewMatrix.inversed;
	const V3& viewSpaceMouse = GetMousePositionInViewSpace(camera);

	// x = 0, y = 0: Center of NDC
	// z = 0: Point of camera
	V3 viewOrigin = V3::zero();
	V3 viewDirection = viewSpaceMouse.normalized /* viewSpaceMouse.normalized - 0*/;

	V3 worldOrigin = viewToWorldMatrix.MultiplyPoint(viewOrigin);
	V3 worldDirection = viewToWorldMatrix.MultiplyVector(viewDirection);;

	return Ray(worldOrigin, worldDirection);
}

void Input::Used(Key key)
{
	m_frame.DownState[(unsigned char)key] = false;
	m_frame.UpState[(unsigned char)key] = false;
}

void Input::UsedWheelDelta()
{
	m_frame.WheelDelta = 0;
}

void Input::SetKeyDown(unsigned char key)
{
	if (!m_enable)
		return;

	if (m_pressingState[key])
		return;

	m_capture.DownState[key] = true;
	m_pressingState[key] = true;
}

void Input::SetKeyUp(unsigned char key)
{
	if (!m_pressingState[key])
		return;

	m_capture.UpState[key] = true;
	m_pressingState[key] = false;
}

void Input::SetWheelDelta(short wParamHiword)
{
	if (!m_enable)
		return;

	if (wParamHiword > 0) ++m_capture.WheelDelta;
	else --m_capture.WheelDelta;
}
