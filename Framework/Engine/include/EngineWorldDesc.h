#pragma once

ENGINE_BEGIN

struct WindowDesc
{
	HINSTANCE hInstance = NULL;
	unsigned int WinCX = 1280;
	unsigned int WinCY = 720;
	WNDPROC WndProc = nullptr;
	TCHAR Title[256] = {};
	bool fullScreen = false;
	HWND hWnd = NULL;
};

struct UpdateDesc
{
	float UpdateFPS = 144.0f;
	float FixedUpdateFPS = 60.0f;
	unsigned int PhysSubStepLimit = 3;
	bool vsync = false;
};

struct SoundDesc
{
	int NumChannels = 128;
};

struct EngineWorldDesc
{
	// 이 값이 null이 아니라면 AlreadedWindowDesc는 무시됩니다.
	// 윈도우를 생성에 필요한 설정입니다.
	// hWnd를 전달하면 이미 생성된 윈도우를 사용합니다.
	WindowDesc windowCreateDesc;

	// 업데이트에 필요한 정보를 설정합니다.
	UpdateDesc updateDesc;

	// 사운드에 필요한 정보를 설정합니다.
	SoundDesc soundDesc;
};

ENGINE_END