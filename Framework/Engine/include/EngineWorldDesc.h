#pragma once

ENGINE_BEGIN

struct MakeWindowDesc
{
	HINSTANCE hInstance = NULL;
	unsigned int WinCX = 1280;
	unsigned int WinCY = 720;
	WNDPROC WndProc = nullptr;
	TCHAR Title[256] = {};
	bool fullScreen = false;
};

struct AlreadedWindowDesc
{
	HWND hWnd = NULL;
};

struct UpdateDesc
{
	float UpdateFPS = 144.0f;
	float FixedUpdateFPS = 60.0f;
	unsigned int PhysSubStepLimit = 3;
	bool vsync = false;
};

struct EngineWorldDesc
{
	// 이 값이 null이 아니라면 AlreadedWindowDesc는 무시됩니다.
	// 윈도우를 생성에 필요한 정보를 담습니다.
	MakeWindowDesc* makeWindowDesc = nullptr;
	
	// 이미 생성된 윈도우를 사용하기 위한 정보를 담습니다.
	AlreadedWindowDesc* alreadedWindowDesc = nullptr;

	// 업데이트에 필요한 정보를 설정하는 정보를 담습니다.
	UpdateDesc* updateDesc = nullptr;
};

ENGINE_END