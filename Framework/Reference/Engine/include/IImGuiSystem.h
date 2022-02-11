#pragma once

ENGINE_BEGIN
class System;
class GraphicSystem;
class Input;
class ENGINE_API IImGuiSystem
{
public:

	IImGuiSystem() = default;

	virtual ~IImGuiSystem() = default;

public:

	virtual bool Initialize(System* system, GraphicSystem* graphicSystem) = 0;

	virtual bool Release() = 0;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	virtual bool BeginRender() = 0;

	virtual bool EndRender() = 0;
};

ENGINE_END
