#include "stdafx.h"
#include "SceneAshi.h"

#include "Config.h"
#include "EditorDeserialize.h"
#include "GameSystem.h"
#include "EventSystem.h"

Scene* SceneAshi::Clone()
{
	return new SceneAshi;
}

void SceneAshi::OnLoad()
{
	INIT_PHYSICS_LAYER(system->physics->layerManager);
	LOAD_CONFIGFILES(system);

	GameObject* goGameSystem = CreateGameObject();
	goGameSystem->AddComponent<GameSystem>();

	EventSystem::RegistListener(this);

	Json::Value root;
	std::ifstream in(TEXT("../Scene/Ashi.txt"));

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

	vector<GameObject*> goNextSceneTriggers = FindGameObjectsWithTag(TAG_NEXTSCENE_TRIGGER);
	for (auto& go : goNextSceneTriggers)
	{
		Collider* collider = go->GetComponent<Collider>();
		if (collider)
			m_nextSceneTriggers.push_back(collider);
	}

	Camera* camera = (Camera*)system->graphic->cameraManager->mainCamera;

	FogDesc fogDesc = camera->GetFogDesc();
	fogDesc.Enable = true;
	fogDesc.Type = FogType::Z;
	fogDesc.MinZ = 100.0f;
	fogDesc.RangeZ = 100.0f;
	fogDesc.Color = Color::RGBA255(145, 120, 100, 255);
	camera->SetFogDesc(fogDesc);

	DOFDesc dofDesc = camera->GetDOFDesc();
	dofDesc.MinZ = 30.0f;
	dofDesc.RangeZ = 20.0f;
	camera->SetDOFDesc(dofDesc);
}

void SceneAshi::OnUnload()
{
	EventSystem::UnregistListener(this);
}

void SceneAshi::OnUpdate()
{
}

void SceneAshi::OnLateUpdate()
{
}

void SceneAshi::OnEvent(const string& msg, void* pContext)
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
