#include "stdafx.h"
#include "SceneCave01.h"
#include "Config.h"
#include "EventSystem.h"
#include "EditorDeserialize.h"

Scene* SceneCave01::Clone()
{
    return new SceneCave01;
}

void SceneCave01::OnLoad()
{
	INIT_PHYSICS_LAYER(system->physics->layerManager);
	LOAD_CONFIGFILES(system);

	GameObject* goEventSystem = CreateGameObject();
	goEventSystem->AddComponent<EventSystem>();

	EventSystem::RegistListener(this);

	Json::Value root;
	std::ifstream in(TEXT("../Scene/Cave01.txt"));

	if (!in.is_open())
	{
		in.close();
		return;
	}

	in >> root;
	in.close();

	EDITOR::EditorDeserialize::DeserializeAllInRuntimeScene(this, root);

	vector<GameObject*> goFightColliders = FindGameObjectsWithTag(TAG_FIGHT_COLLIDER);
	for (auto& go : goFightColliders)
	{
		Collider* collider = go->GetComponent<Collider>();
		if (collider)
			m_fightColliders.push_back(collider);
	}
}

void SceneCave01::OnUnload()
{
}
#include "SceneAncientKing.h"
void SceneCave01::OnUpdate()
{
	if (system->input->GetKeyDown(Key::Zero))
		system->sceneManagement->ChangeScene(new SceneAncientKing);

	{
		ImGui::Begin("Info", 0, ImGuiWindowFlags_AlwaysAutoResize);

		tstring resolutionTxt = tstring_format(TEXT("resolution: %d x %d"), int(system->graphic->GetWidth()), int(system->graphic->GetHeight()));
		ImGui::Text(tstring_to_str_utf8(resolutionTxt).c_str());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		#ifdef _DEBUG
		ImGui::Text("Debug Mode");
		#else
		ImGui::Text("Release Mode");
		#endif

		ImGui::End();
	}
}

void SceneCave01::OnLateUpdate()
{
}

void SceneCave01::OnEvent(const string& msg, void* pContext)
{
	if (msg == EVENT_FIGHT_START)
	{
		for (auto& collider : m_fightColliders)
			collider->isTrigger = false;
	}
	if (msg == EVENT_FIGHT_END)
	{
		for (auto& collider : m_fightColliders)
			collider->isTrigger = true;
	}
}