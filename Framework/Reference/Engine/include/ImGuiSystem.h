#pragma once

#include "IImGuiSystem.h"

ENGINE_BEGIN

class ENGINE_API ImGuiSystem : public IImGuiSystem
{
public:

	ImGuiSystem();

	virtual ~ImGuiSystem();

private:

	virtual bool Initialize(System * system, GraphicSystem* graphicSystem) final override;

	virtual bool Release() final override;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final override;

	virtual bool BeginRender() final override;

	virtual bool EndRender() final override;

private:

	System* m_system = nullptr;

	GraphicSystem* m_graphicSystem = nullptr;
};

ENGINE_END
