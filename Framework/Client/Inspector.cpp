#include "stdafx.h"
#include "Inspector.h"
#include "IEditorObject.h"
#include "Editor.h"

EDITOR_USE

void Inspector::OnImGuiRender()
{
	if (!ImGui::CollapsingHeader("Inspector"))
		return;
	
	if (m_object && system->input->GetKeyDown(Key::Delete))
	{
		Editor* editor = Editor::GetInstance();
		if (!editor)
			return;

		Gizmo* gizmo = editor->GetGizmo();
		if (!gizmo)
			return;

		gizmo->show = false;
		gizmo->handlingTransform = nullptr;

		m_object->gameObject->Destroy();
		m_object = nullptr;
	}

	if (m_object)
	{
		m_object->ImGuiRender();
	}
}

void Inspector::Select(Ref<IEditorObject> object)
{
	Editor* editor = Editor::GetInstance();
	if (!editor)
		return;

	Gizmo* gizmo = editor->GetGizmo();
	if (!gizmo)
		return;

	m_object = object;

	if (m_object)
	{
		gizmo->show = true;
		gizmo->handlingTransform = object->transform;
	}
	else
	{
		gizmo->show = false;
		gizmo->handlingTransform = nullptr;
	}
}
