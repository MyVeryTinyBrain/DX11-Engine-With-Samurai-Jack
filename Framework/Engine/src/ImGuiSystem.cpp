#include "EnginePCH.h"
#include "ImGuiSystem.h"
#include "System.h"
#include "GraphicSystem.h"
#include "Input.h"

ImGuiSystem::ImGuiSystem()
{
}

ImGuiSystem::~ImGuiSystem()
{
	Release();
}

bool ImGuiSystem::Initialize(System* system, GraphicSystem* graphicSystem)
{
	if (!system)
		return false;

	if (!graphicSystem)
		return false;

	m_system = system;

	m_graphicSystem = graphicSystem;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;

	io.Fonts->AddFontFromFileTTF("../Resource/Font/Arialuni.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
	//unsigned char* pixels;
	//int width, height;
	//io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_system->graphicSystem->windowHandle);
	ImGui_ImplDX11_Init(m_system->graphicSystem->device.Get(), m_system->graphicSystem->deviceContext.Get());

	return true;
}

bool ImGuiSystem::Release()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ImGuiSystem::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	switch (message)
	{
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
				break;;

			RECT rect;
			::GetClientRect(hWnd, &rect);

			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
			ImGui_ImplDX11_InvalidateDeviceObjects();
			ImGui_ImplDX11_CreateDeviceObjects();
		}
		break;
	}
}

bool ImGuiSystem::BeginRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;
}

bool ImGuiSystem::EndRender()
{
	if (!m_graphicSystem->ClearDepthStencilBuffer(1.0f, 0))
		return false;

	ImGui::Render();

	ImDrawData* drawData = ImGui::GetDrawData();
	if (!drawData || !drawData->Valid)
		return false;

	ImGui_ImplDX11_RenderDrawData(drawData);

	return true;
}
