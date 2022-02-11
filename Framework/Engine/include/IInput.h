#pragma once

ENGINE_BEGIN
class System;
class ENGINE_API IInput abstract
{
public:

	virtual ~IInput() = default;

	virtual bool Initialize(System* system) = 0;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	virtual void End() = 0;
	
	virtual void Fetch() = 0;

	virtual void SetEnable(bool value) = 0;
};

ENGINE_END

