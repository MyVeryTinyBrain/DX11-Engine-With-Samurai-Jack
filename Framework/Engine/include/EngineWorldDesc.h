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
	// �� ���� null�� �ƴ϶�� AlreadedWindowDesc�� ���õ˴ϴ�.
	// �����츦 ������ �ʿ��� �����Դϴ�.
	// hWnd�� �����ϸ� �̹� ������ �����츦 ����մϴ�.
	WindowDesc windowCreateDesc;

	// ������Ʈ�� �ʿ��� ������ �����մϴ�.
	UpdateDesc updateDesc;

	// ���忡 �ʿ��� ������ �����մϴ�.
	SoundDesc soundDesc;
};

ENGINE_END