#include "stdafx.h"
#include "PrefabAshi.h"
#include "EditorConfig.h"
#include "Config.h"
#include "BossAshi.h"
#include "Editor.h"

EDITOR_USE

void PrefabAshi::Awake()
{
	if (Editor::IsEditorMode())
	{
		IEditorObject::Awake();

		GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
		goMeshRenderer->transform->localPosition = ADJUST_ASHI_LOCALPOSITION;
		goMeshRenderer->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
		goMeshRenderer->transform->localScale = ADJUST_ASHI_LOCALSCALE;
		m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
		m_meshRenderer->mesh = system->resource->Find(MESH_ASHI);
		m_meshRenderer->SetupStandardMaterials();
	}
}

void PrefabAshi::OnImGui()
{
}

bool PrefabAshi::Raycast(ICamera* camera, V3& out_hit) const
{
	if (!m_meshRenderer->mesh)
		return false;

	M4 localToWorldMatrix = m_meshRenderer->transform->localToWorldMatrix;
	Ray ray = system->input->GetRayInWorldSpace(camera);
	if (m_meshRenderer->mesh->RaycastInWorldSpace(out_hit, ray, localToWorldMatrix, true))
	{
		return true;
	}
	else
	{
		return false;
	}
}

string PrefabAshi::GetType() const
{
	return EDITOR_OBJECT_TYPE_PREFAB_ASHI;
}

void PrefabAshi::OnSerialize(Json::Value& json) const
{
}

void PrefabAshi::OnDeserialize(const Json::Value& json)
{
}

void PrefabAshi::OnDeserializeInRuntime(const Json::Value& json)
{
	gameObject->AddComponent<BossAshi>();
}