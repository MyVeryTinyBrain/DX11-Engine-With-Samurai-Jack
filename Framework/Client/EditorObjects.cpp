#include "stdafx.h"
#include "EditorObjects.h"
#include "Editor.h"
#include "FreeCamera.h"
#include "Inspector.h"
#include "IEditorObject.h"

EDITOR_USE

void EditorObjects::Update()
{
	Editor* editor = Editor::GetInstance();
	if (!editor)
		return;

	FreeCamera* freeCamera = editor->GetFreeCamera();
	if (!freeCamera)
		return;

	Inspector* inspector = editor->GetInspector();
	if (!inspector)
		return;

	Gizmo* gizmo = editor->GetGizmo();
	if (!gizmo)
		return;

	if (system->input->GetKeyDown(Key::LeftMouse))
	{
		bool gizmoClicked = gizmo->show && gizmo->PickTest() != GizmoBase::Axis::None;

		if (!gizmoClicked)
		{
			float minDist = FLT_MAX;
			IEditorObject* minDistSelected = nullptr;

			for (auto& object : m_objects)
			{
				V3 hit;
				if (object->Raycast(freeCamera->camera, hit))
				{
					float dist = V3::Distance(freeCamera->transform->position, hit);
					if (dist < minDist)
					{
						minDist = dist;
						minDistSelected = object;
					}
				}
			}

			inspector->Select(minDistSelected);
		}
	}
}

void EditorObjects::RegistObject(IEditorObject* object)
{
	auto find_it = std::find(m_objects.begin(), m_objects.end(), object);
	if (find_it != m_objects.end())
		return;

	m_objects.push_back(object);
}

void EditorObjects::UnregistObject(IEditorObject* object)
{
	auto find_it = std::find(m_objects.begin(), m_objects.end(), object);
	if (find_it != m_objects.end())
	{
		m_objects.erase(find_it);
	}
}

void EditorObjects::ClearObjects()
{
	while (!m_objects.empty())
	{
		IEditorObject* object = m_objects.front();
		object->gameObject->Destroy();
	}
}

void EditorObjects::Serialize(Json::Value& root) const
{
	Json::Value objects;

	uint numObjects = (uint)m_objects.size();
	for (uint i = 0; i < numObjects; ++i)
	{
		Json::Value value = m_objects[i]->Serialize();
		objects[i] = value;
	}

	root["EditorObjects"] = objects;
}

#include "EditorDeserialize.h"
void EditorObjects::Deserialize(const Json::Value& root)
{
	ClearObjects();

	const Json::Value& objects = root["EditorObjects"];

	uint numObjects = objects.size();

	for (uint i = 0; i < numObjects; ++i)
	{
		const Json::Value& value = objects[i];

		IEditorObject* object = EditorDeserialize::NewEditorObjectByType(gameObject->regionScene, value);
		if (!object) continue;

		object->Deserialize(value);

		RegistObject(object);
	}
}

string EditorObjects::GetNumberedName(const string& name) const
{
	uint number = 0;
	tstring tstrName = string_to_tstring(name);

	uint numObjects = (uint)m_objects.size();
	while (1)
	{
		bool findSameName = false;
		for (uint i = 0; i < numObjects; ++i)
		{
			IEditorObject* object = m_objects[i];

			string strNumer = std::to_string(number);
			tstring tstrNumber = string_to_tstring(strNumer);
			tstring tstrNumberedName = tstrName + tstrNumber;
			if (object->gameObject->name == tstrNumberedName)
			{
				findSameName = true;
				++number;
				break;
			}
		}
		if (!findSameName)
			break;
	}

	return name + std::to_string(number);
}
