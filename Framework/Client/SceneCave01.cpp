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
	fogDesc.MinZ = 40.0f;
	fogDesc.RangeZ = 30.0f;
	fogDesc.Color = Color::black();
	camera->SetFogDesc(fogDesc);
}

void SceneCave01::OnUnload()
{
	EventSystem::UnregistListener(this);
}
#include "SceneCave02.h"
void SceneCave01::OnUpdate()
{
	for (auto& nextSceneTrigger : m_nextSceneTriggers)
	{
		if (system->physics->query->OverlapTest(nextSceneTrigger, 1 << PhysicsLayer_Player))
		{
			system->sceneManagement->ChangeScene(new SceneCave02);
		}
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