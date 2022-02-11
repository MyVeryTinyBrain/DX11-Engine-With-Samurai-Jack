#include "EnginePCH.h"
#include "EngineWorld.h"

EngineWorld::EngineWorld() : EngineWorldBase()
{
}


EngineWorld::~EngineWorld()
{
}

bool EngineWorld::Initialize(EngineWorldDesc* desc)
{
	if (!EngineWorldBase::Initialize(desc))
		RETURN_FALSE_ERROR_MESSAGE("EngineWorldBase::Initialize");

	return true;
}

bool EngineWorld::Step()
{
	return EngineWorldBase::Step();
}

bool EngineWorld::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return EngineWorldBase::WndProc(hWnd, message, wParam, lParam);
}

void EngineWorld::Release()
{
	EngineWorldBase::Release();
}

System* EngineWorld::GetSystem() const
{
	return EngineWorldBase::GetSystem();
}
