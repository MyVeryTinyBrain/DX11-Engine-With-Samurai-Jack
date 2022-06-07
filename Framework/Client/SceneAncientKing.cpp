#include "stdafx.h"
#include "SceneAncientKing.h"

#include "Config.h"
#include "EditorDeserialize.h"
#include "EventSystem.h"

Scene* SceneAncientKing::Clone()
{
    return new SceneAncientKing;
}

void SceneAncientKing::OnLoad()
{
    INIT_PHYSICS_LAYER(system->physics->layerManager);
    LOAD_CONFIGFILES(system);

	GameObject* goEventSystem = CreateGameObject();
	goEventSystem->AddComponent<EventSystem>();

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
}

void SceneAncientKing::OnUnload()
{
	EventSystem::UnregistListener(this);
}

void SceneAncientKing::OnUpdate()
{
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
