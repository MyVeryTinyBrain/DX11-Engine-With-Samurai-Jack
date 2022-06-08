#include "stdafx.h"
#include "MeshSelector.h"
#include "Editor.h"
#include "EditorEventSystem.h"
#include "EditorConfig.h"

EDITOR_USE

void MeshSelector::Awake()
{
	this->enable = false;
}

void MeshSelector::Update()
{
	ImGui::Begin("Mesh selector", NULL);

	char cstrSearch[MAX_PATH];
	strcpy_s(cstrSearch, m_strSearch.c_str());
	if (ImGui::InputText("search", cstrSearch, MAX_PATH))
	{
		m_strSearch = cstrSearch;
	}
	string strLowerSearch = m_strSearch;
	std::transform(strLowerSearch.begin(), strLowerSearch.end(), strLowerSearch.begin(), ::tolower);

	ImGui::BeginChild("Meshes", ImVec2(0, 400), true);
	for (auto& mesh : m_meshes)
	{
		string strName = tstring_to_string(mesh->name);
		bool isSelected = m_selected == mesh;

		string strLowerName = strName;
		std::transform(strLowerName.begin(), strLowerName.end(), strLowerName.begin(), ::tolower);
		bool strMatch = true;
		if (m_strSearch != "")
			strMatch = strLowerName.find(strLowerSearch) != string::npos;

		if (strMatch && ImGui::Selectable(strName.c_str(), &isSelected))
		{
			if (isSelected)
			{
				m_selected = mesh;
				NotifySelected(false);
			}
		}
	}
	ImGui::EndChild();

	ImGui::BeginChild("Selected", ImVec2(0, 0), false);
	if (m_selected)
	{
		string strPath = tstring_to_string(m_selected->path);
		ImGui::Text(strPath.c_str());
	}
	else
	{
		ImGui::Text("No selected mesh");
	}
	if (ImGui::Button("OK", ImVec2(ImGui::GetWindowSize().x, 20)))
	{
		NotifySelected(true);
	}
	ImGui::EndChild();

	ImGui::End();
}

void MeshSelector::OnEnable()
{
	Reset();
}

void MeshSelector::Show()
{
	this->enable = true;
}

void MeshSelector::Reset()
{
	m_selected = nullptr;
	FindAllMeshes();
}

ResourceRef<Mesh> MeshSelector::GetSelected()
{
	return m_selected;
}

void MeshSelector::FindAllMeshes()
{
	m_meshes.clear();

	uint numResources = system->resource->GetNumResources();
	for (uint i = 0; i < numResources; ++i)
	{
		ResourceRef<Mesh> mesh = system->resource->GetResourceByIndex(i);
		if (mesh)
		{
			m_meshes.push_back(mesh);
		}
	}
}

void MeshSelector::NotifySelected(bool close)
{
	Editor* editor = Editor::GetInstance();
	EditorEventSystem* editorEventSystem = nullptr;
	if (editor) editorEventSystem = editor->GetEditorEventSystem();
	if (editorEventSystem)
	{
		TCHAR selectedPath[MAX_PATH]; _tcscpy_s(selectedPath, m_selected->path.c_str());
		editorEventSystem->Notify(EDITOR_EVENT_MESH_SELECTED, selectedPath);
		this->enable = !close;
	}
}
