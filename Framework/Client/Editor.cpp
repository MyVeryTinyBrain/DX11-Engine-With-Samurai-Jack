#include "stdafx.h"
#include "Editor.h"
#include "Config.h"
#include "FreeCamera.h"
#include "EditorObjects.h"
#include "Spawner.h"
#include "Inspector.h"
#include "MeshSelector.h"
#include "EditorEventSystem.h"
#include "IEditorObject.h"
#include "Hierarchy.h"

#include <commdlg.h>

EDITOR_USE

Editor* Editor::g_editor = nullptr;

void Editor::Awake()
{
	if (g_editor)
	{
		g_editor->Destroy();
	}
	g_editor = this;

	Init();
}

void Editor::OnDestroyed()
{
	if (g_editor == this)
	{
		g_editor = nullptr;
	}
}

void Editor::Update()
{
	// 다른 윈도우를 선택하면 오브젝트 선택이 해제되는 현상 방지를 위한 코드입니다.
	if (system->input->GetKeyDown(Key::LeftMouse))
	{
		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
		{
			system->input->Used(Key::LeftMouse);
		}
	}

	// 휠 드래그가 카메라와 윈도우에 동시에 일어나는 현상 방지를 위한 코드입니다.
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
	{
		system->input->UsedWheelDelta();
	}

	if (system->input->GetKey(Key::LCtrl) && system->input->GetKeyDown(Key::S))
	{
		if (system->input->GetKey(Key::LShift)) SaveAs();
		else Save();
	}

	// 메뉴바
	ImGui::Begin("Editor", 0, ImGuiWindowFlags_MenuBar);
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) New();
				if (ImGui::MenuItem("Load")) Load();
				if (ImGui::MenuItem("Save (Ctrl + S)")) Save();
				if (ImGui::MenuItem("Save as (Ctrl + Shift + S)")) SaveAs();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		m_spawner->OnImGuiRender();
	}
	ImGui::End();

	m_inspector->OnImGuiRender();
	m_hierarchy->OnImGuiRender();
}

void Editor::Init()
{
	INIT_PHYSICS_LAYER(system->physics->layerManager);

	LOAD_CONFIGFILES(system);

	{
		GameObject* goCamera = CreateGameObject(TEXT("Camera"));
		goCamera->transform->position = V3(0, 0, 0);
		goCamera->transform->forward = V3::forward();
		m_freeCamera = goCamera->AddComponent<FreeCamera>();
		m_freeCamera->camera->fov = 45;
		//m_freeCamera->camera->postProcessingState = false;
		m_freeCamera->camera->postProcessingState = true;

		SSAODesc ssaoDesc = m_freeCamera->camera->ssaoDesc;
		ssaoDesc.Enable = false;
		m_freeCamera->camera->ssaoDesc = ssaoDesc;

		SSRDesc ssrDesc = m_freeCamera->camera->ssrDesc;
		ssrDesc.Enable = false;
		m_freeCamera->camera->ssrDesc = ssrDesc;

		DOFDesc dofDesc = m_freeCamera->camera->dofDesc;
		dofDesc.Enable = false;
		m_freeCamera->camera->dofDesc = dofDesc;

		FogDesc fogDesc = m_freeCamera->camera->fogDesc;
		fogDesc.Enable = false;
		m_freeCamera->camera->fogDesc = fogDesc;

		BloomDesc bloomDesc = m_freeCamera->camera->bloomDesc;
		bloomDesc.Enable = true;
		m_freeCamera->camera->bloomDesc = bloomDesc;

		ChromaticAberrationDesc chromaticAberrationDesc = m_freeCamera->camera->chromaticAberrationDesc;
		chromaticAberrationDesc.Enable = false;
		m_freeCamera->camera->chromaticAberrationDesc = chromaticAberrationDesc;
	}

	{
		GameObject* goGizmo = CreateGameObject();
		m_gizmo = goGizmo->AddComponent<Gizmo>();
		m_gizmo->show = false;
	}

	{
		m_editorObjects = gameObject->AddComponent<EditorObjects>();
		m_spawner = gameObject->AddComponent<Spawner>();
		m_inspector = gameObject->AddComponent<Inspector>();
		m_meshSelector = gameObject->AddComponent<MeshSelector>();
		m_editorEventSystem = gameObject->AddComponent<EditorEventSystem>();
		m_hierarchy = gameObject->AddComponent<Hierarchy>();
	}

	{
		m_spawner->SpawnDirectionalLight();
	}
}

void Editor::Serialize(const tstring& path)
{
	Json::Value root;
	m_editorObjects->Serialize(root);

	Json::StyledWriter styledWritter;
	std::ofstream out;
	out.open(path);
	out << styledWritter.write(root);
	out.close();
}

void Editor::Deserialize(const tstring& path)
{
	Json::Value root;
	std::ifstream in(path);

	if (!in.is_open())
	{
		in.close();
		return;
	}

	in >> root;
	in.close();

	New();
	m_editorObjects->Deserialize(root);
}

bool Editor::OpenFileDialog(tstring& out_path)
{
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH] = TEXT("");
	static TCHAR filter[] = TEXT("텍스트 파일\0*.txt");

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = 0;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrInitialDir = TEXT(".");

	if (GetOpenFileName(&OFN) != 0)
	{
		out_path = OFN.lpstrFile;	
		string str = tstring_to_string(out_path);
		cout << str << endl;
		return true;
	}
	else
	{
		return false;
	}
}

bool Editor::SaveFileDialog(tstring& out_path)
{
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH] = TEXT("");
	static TCHAR filter[] = TEXT("텍스트 파일\0*.txt");

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = 0;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrInitialDir = TEXT(".");

	if (GetSaveFileName(&OFN) != 0)
	{
		out_path = OFN.lpstrFile;
		string str = tstring_to_string(out_path);
		cout << str << endl;
		return true;
	}
	else
	{
		return false;
	}
}

void Editor::New()
{
	m_editorObjects->ClearObjects();
	m_inspector->Select(nullptr);
	m_freeCamera->transform->forward = V3::forward();
	m_freeCamera->transform->position = V3::zero();
	m_path = TEXT("");
}

void Editor::Load()
{
	tstring path;
	if (OpenFileDialog(path))
	{
		Deserialize(path);
		m_path = path;
	}
}

void Editor::Save()
{
	if (m_path != TEXT(""))
	{
		Serialize(m_path);
	}
	else
	{
		SaveAs();
	}
}

void Editor::SaveAs()
{
	tstring path;
	if (SaveFileDialog(path))
	{
		Serialize(path);
		m_path = path;
	}
}
