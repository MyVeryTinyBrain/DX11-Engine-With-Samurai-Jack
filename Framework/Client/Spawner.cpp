#include "stdafx.h"
#include "Spawner.h"
#include "Editor.h"
#include "FreeCamera.h"
#include "Editor.h"
#include "Inspector.h"
#include "EditorObjects.h"

#include "Prop.h"

#include "SerializableDirectionalLight.h"
#include "SerializablePointLight.h"
#include "SerializableSpotLight.h"

#include "SerializableBoxCollider.h"
#include "SerializableSphereCollider.h"
#include "SerializableCapsuleCollider.h"

#include "PrefabPlayer.h"
#include "PrefabBeetleDrone.h"
#include "PrefabAncientKing.h"

#include "Lava.h"

EDITOR_USE

void Spawner::OnImGuiRender()
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

	EditorObjects* editorObjects = editor->GetEditorObjects();
	if (!editorObjects)
		return;

	if (!ImGui::CollapsingHeader("Spawner"))
		return;

	IEditorObject* o = nullptr;

	if (ImGui::TreeNode("Mesh"))
	{
		if (ImGui::Selectable("Mesh"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<Prop>();

			inspector->Select(o);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Light"))
	{
		if (ImGui::Selectable("Direction Light"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<SerializableDirectionalLight>();

			inspector->Select(o);
		}
		if (ImGui::Selectable("Point Light"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<SerializablePointLight>();

			inspector->Select(o);
		}
		if (ImGui::Selectable("Spot Light"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<SerializableSpotLight>();

			inspector->Select(o);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Collider"))
	{
		if (ImGui::Selectable("Box Collider"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<SerializableBoxCollider>();

			inspector->Select(o);
		}
		if (ImGui::Selectable("Sphere Collider"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<SerializableSphereCollider>();

			inspector->Select(o);
		}
		if (ImGui::Selectable("Capsule Collider"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<SerializableCapsuleCollider>();

			inspector->Select(o);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Prefab"))
	{
		if (ImGui::Selectable("Player"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<PrefabPlayer>();

			inspector->Select(o);
		}
		if (ImGui::Selectable("BeetleDrone"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<PrefabBeetleDrone>();

			inspector->Select(o);
		}
		if (ImGui::Selectable("AncientKing"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<PrefabAncientKing>();

			inspector->Select(o);
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Editable"))
	{
		if (ImGui::Selectable("Lava"))
		{
			GameObject* go = CreateGameObject();
			go->transform->position = freeCamera->transform->position + freeCamera->transform->forward * 5.0f;
			o = go->AddComponent<Lava>();

			inspector->Select(o);
		}

		ImGui::TreePop();
	}

	if (o)
	{
		string strNumberedName = editorObjects->GetNumberedName(o->GetType());
		o->gameObject->name = string_to_tstring(strNumberedName);
	}
}

void Spawner::SpawnDirectionalLight()
{
	Editor* editor = Editor::GetInstance();
	if (!editor)
		return;

	Inspector* inspector = editor->GetInspector();
	if (!inspector)
		return;

	GameObject* go = CreateGameObject();
	go->name = TEXT("DirectionalLight");
	go->transform->position = V3(0, 5, 0);
	SerializableDirectionalLight* o = go->AddComponent<SerializableDirectionalLight>();

	go->transform->forward = -V3(9.0f, 8.0f, -0.1f).normalized;
	o->GetDirectionalLight()->enable = true;
	o->GetDirectionalLight()->drawShadow = true;
	o->GetDirectionalLight()->diffuse = Color(1, 1, 1, 1);
	o->GetDirectionalLight()->ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);

	inspector->Select(o);
}
