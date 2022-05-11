#pragma once

#include "EngineWorldDesc.h"

ENGINE_BEGIN
class System;
class ENGINE_API EngineWorldBase abstract
{
public:

	EngineWorldBase();

	virtual ~EngineWorldBase();

public:

	virtual System* GetSystem() const = 0;

protected:

	virtual bool Initialize(EngineWorldDesc* desc);

	virtual bool Step();

	virtual bool WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Release();

private:

	HWND MakeWindow(WindowDesc* desc) const;

	HINSTANCE GetHInstance(WindowDesc* desc) const;

private:

	HINSTANCE			m_hInstance = NULL;

	HWND				m_hWnd = NULL;

	System*				m_system = nullptr;

	class ThreadPool*	m_threadPool = nullptr;
};
ENGINE_END

