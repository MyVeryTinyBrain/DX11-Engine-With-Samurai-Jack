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

	HWND MakeWindow(MakeWindowDesc* desc) const;

	HINSTANCE GetHInstance(AlreadedWindowDesc* desc) const;

private:

	HINSTANCE		m_hInstance = NULL;

	HWND			m_hWnd = NULL;

	System*			m_system = nullptr;
};
ENGINE_END

