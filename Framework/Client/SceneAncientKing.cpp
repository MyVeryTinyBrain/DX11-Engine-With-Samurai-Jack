#include "stdafx.h"
#include "SceneAncientKing.h"

#include "Config.h"
#include "EditorDeserialize.h"
#include "GameSystem.h"
#include "EventSystem.h"

Scene* SceneAncientKing::Clone()
{
    return new SceneAncientKing;
}

void SceneAncientKing::OnLoad()
{
    INIT_PHYSICS_LAYER(system->physics->layerManager);
    LOAD_CONFIGFILES(system);

	GameObject* goGameSystem = CreateGameObject();
	goGameSystem->AddComponent<GameSystem>();

	EventSystem::RegistListener(this);

	Json::Value root;
	std::ifstream in(TEXT("../Scene/AncientKing.txt"));

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
	for (auto& go : goNextSceneTriggers) {
		Collider* collider = go->GetComponent<Collider>();
		if (collider)
			m_nextSceneTriggers.push_back(collider);
	}

	vector<GameObject*> goFallingTriggers = FindGameObjectsWithTag(TAG_FALLING_TRIGGER);
	for (auto& go : goFallingTriggers) {
		Collider* collider = go->GetComponent<Collider>();
		if (collider)
			m_fallingTriggers.push_back(collider);
	}

	Camera* camera = (Camera*)system->graphic->cameraManager->mainCamera;
	FogDesc fogDesc = camera->GetFogDesc();
	fogDesc.Enable = true;
	fogDesc.Type = FogType::Z;
	fogDesc.MinZ = 60.0f;
	fogDesc.RangeZ = 30.0f;
	fogDesc.Color = Color::black();
	camera->SetFogDesc(fogDesc);
}

void SceneAncientKing::OnUnload()
{
	EventSystem::UnregistListener(this);
}
#include "SceneAshi.h"
void SceneAncientKing::OnUpdate()
{
	for (auto& nextSceneTrigger : m_nextSceneTriggers) {
		if (system->physics->query->OverlapTest(nextSceneTrigger, 1 << PhysicsLayer_Player)) {
			GameSystem::GetInstance()->ChangeScene(new SceneAshi);
		}
	}
	for (auto& fallingTrigger : m_fallingTriggers) {
		if (system->physics->query->OverlapTest(fallingTrigger, 1 << PhysicsLayer_Player)) {
			GameSystem::GetInstance()->ChangeScene(new SceneAncientKing, 0.2f);
		}
	}
}

void SceneAncientKing::OnLateUpdate()
{
}

void SceneAncientKing::OnEvent(const string& msg, void* pContext)
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
