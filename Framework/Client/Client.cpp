// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client.h"

#include "TestScene.h"
#include "PlayerTestScene.h"

EngineWorld world;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MakeWindowDesc makeWindowDesc;
    makeWindowDesc.hInstance = hInstance;
    _tcscpy_s(makeWindowDesc.Title, TEXT("Test"));
    makeWindowDesc.WinCX = 1280;
    makeWindowDesc.WinCY = 720;
    makeWindowDesc.WndProc = WndProc;
    makeWindowDesc.fullScreen = false;

    UpdateDesc updateDesc;
    updateDesc.UpdateFPS = 14400.0f;
    updateDesc.FixedUpdateFPS = 60.0f;
    updateDesc.PhysSubStepLimit = 3;

    EngineWorldDesc desc;
    desc.makeWindowDesc = &makeWindowDesc;
    desc.alreadedWindowDesc = nullptr;
    desc.updateDesc = &updateDesc;

    if (!world.Initialize(&desc))
    {
        world.Release();
        return -1;
    }

    world.GetSystem()->GetSceneManagement()->ChangeScene(new PlayerTestScene);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        world.Step();
    }

    world.Release();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    world.WndProc(hWnd, message, wParam, lParam);

    switch (message)
    {
        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
                DestroyWindow(hWnd);
        }
        break;
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}