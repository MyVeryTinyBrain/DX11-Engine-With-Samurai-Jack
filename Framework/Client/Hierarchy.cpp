#include "stdafx.h"
#include "Hierarchy.h"
#include "Editor.h"
#include "EditorObjects.h"
#include "IEditorObject.h"
#include "Inspector.h"
#include "FreeCamera.h"

EDITOR_USE

void Hierarchy::OnImGuiRender()
{
	Editor* editor = Editor::GetInstance();
	if (!editor) return;

	EditorObjects* editorObjects = editor->GetEditorObjects();
	if (!editorObjects) return;

	Inspector* inspector = editor->GetInspector();
	if (!inspector) return;

	FreeCamera* freeCamera = editor->GetFreeCamera();
	if (!freeCamera) return;

	if (!ImGui::CollapsingHeader("Hierarchy"))
		return;

	if (ImGui::BeginListBox("GameObjects", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		IEditorObject* selectedObject = inspector->GetSelected().GetPointer();
		uint numObjects = editorObjects->GetNumObjects();
		for (uint i = 0; i < numObjects; ++i)
		{
			IEditorObject* object = editorObjects->GetObjectByIndex(i);
			string strName = tstring_to_string(object->gameObject->name);

			bool isSelected = selectedObject == object;
			if (ImGui::Selectable(strName.c_str(), &isSelected, ImGuiSelectableFlags_AllowDoubleClick))
			{
				if (selectedObject != object)
				{
					if (isSelected)
						inspector->Select(object);
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					freeCamera->gameObject->transform->position = object->transform->position - freeCamera->transform->forward * 5.0f;
				}
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
}
