#pragma once

#include "EngineWorldBase.h"

ENGINE_BEGIN
class ENGINE_API EngineWorld final : EngineWorldBase
{
public:

	EngineWorld();

	virtual ~EngineWorld();

	bool Initialize(EngineWorldDesc* desc);

	bool Step();

	bool WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Release();

public:

	System* GetSystem() const;
};
ENGINE_END
