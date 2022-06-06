#include "stdafx.h"
#include "PrefabPlayer.h"
#include "EditorConfig.h"
#include "Config.h"

EDITOR_USE

void PrefabPlayer::Awake()
{
	IEditorObject::Awake();

    GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
    goMeshRenderer->transform->localPosition = ADJUST_PLAYER_LOCALPOSITION;
    goMeshRenderer->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
    m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->Find(MESH_JACK);
	m_meshRenderer->SetupStandardMaterials();
}

void PrefabPlayer::OnImGui()
{
}

bool PrefabPlayer::Raycast(ICamera* camera, V3& out_hit) const
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

string PrefabPlayer::GetType() const
{
	return EDITOR_OBJECT_TYPE_PREFAB_PLAYER;
}

void PrefabPlayer::OnSerialize(Json::Value& json) const
{
}

void PrefabPlayer::OnDeserialize(const Json::Value& json)
{
}
