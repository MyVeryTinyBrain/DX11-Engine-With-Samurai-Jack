#include "stdafx.h"
#include "LoadingScene.h"
#include "Config.h"
#include "GameSystem.h"

Scene* LoadingScene::Clone()
{
	return new LoadingScene;
}

void LoadingScene::OnLoad()
{
	GameObject* goCamera = CreateGameObject();
	goCamera->AddComponent<Camera>();
}

void LoadingScene::OnUnload()
{
}

void LoadingScene::OnUpdate()
{
}
#include "SceneCave01.h"
void LoadingScene::OnLateUpdate()
{
	if(!m_resourceLoaded)
	{
		INIT_PHYSICS_LAYER(system->physics->layerManager);
		LOAD_CONFIGFILES(system);

		system->sceneManagement->ChangeScene(new SceneCave01);

		m_resourceLoaded = true;
	}
}
