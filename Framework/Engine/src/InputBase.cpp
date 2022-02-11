#include "EnginePCH.h"
#include "InputBase.h"

InputBase::InputBase()
{
	Initialize();
}

bool InputBase::GetKey(const Key& key)
{
	SHORT state = GetAsyncKeyState((int)key) & 0x8000;
	return state != 0;
}

const bool& InputBase::GetKeyDown(const Key& key)
{
	return m_frame.DownState[(unsigned char)key];
}

const bool& InputBase::GetKeyUp(const Key& key)
{
	return m_frame.UpState[(unsigned char)key];
}

const float& InputBase::GetMouseWheelDelta()
{
	return m_frame.WheelDelta;
}

bool InputBase::Initialize()
{
	ZeroMemory(&m_pressingState, sizeof(m_pressingState));

	ZeroMemory(&m_capture, sizeof(Buffer));

	ZeroMemory(&m_frame, sizeof(Buffer));

	return true;
}

void InputBase::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void InputBase::Ready()
{
	ZeroMemory(&m_capture, sizeof(Buffer));
	m_lock = false;
}

void InputBase::Update()
{
	memcpy(&m_frame, &m_capture, sizeof(Buffer));
	m_lock = true;
}

void InputBase::SetKeyDown(unsigned char key)
{
	if (m_pressingState[key])
		return;

	m_capture.DownState[key] = true;
	m_pressingState[key] = true;
}

void InputBase::SetKeyUp(unsigned char key)
{
	if (!m_pressingState[key])
		return;

	m_capture.UpState[key] = true;
	m_pressingState[key] = false;
}

void InputBase::SetWheelDelta(short wParamHiword)
{
	if (wParamHiword > 0) ++m_capture.WheelDelta;
	else --m_capture.WheelDelta;
}
