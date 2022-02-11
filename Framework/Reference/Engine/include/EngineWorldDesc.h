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
	// �� ���� null�� �ƴ϶�� AlreadedWindowDesc�� ���õ˴ϴ�.
	// �����츦 ������ �ʿ��� ������ ����ϴ�.
	MakeWindowDesc* makeWindowDesc = nullptr;
	
	// �̹� ������ �����츦 ����ϱ� ���� ������ ����ϴ�.
	AlreadedWindowDesc* alreadedWindowDesc = nullptr;

	// ������Ʈ�� �ʿ��� ������ �����ϴ� ������ ����ϴ�.
	UpdateDesc* updateDesc = nullptr;
};

ENGINE_END