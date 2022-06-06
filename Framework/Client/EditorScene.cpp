#include "stdafx.h"
#include "EditorScene.h"
#include "Editor.h"

EDITOR_USE

Scene* EditorScene::Clone()
{
	return new EditorScene;
}

void EditorScene::OnLoad()
{
	GameObject* goEditor = CreateGameObject();
	Editor* editor = goEditor->AddComponent<Editor>();
}

void EditorScene::OnUnload()
{
}

void EditorScene::OnUpdate()
{
}

void EditorScene::OnLateUpdate()
{
}
