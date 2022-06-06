#include "stdafx.h"
#include "PrefabBeetleDrone.h"
#include "EditorConfig.h"
#include "Config.h"

EDITOR_USE

void PrefabBeetleDrone::Awake()
{
	IEditorObject::Awake();

    GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
	goMeshRenderer->transform->localPosition = ADJUST_BEETLEDRONE_LOCALPOSITION;
	goMeshRenderer->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
    m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
    m_meshRenderer->mesh = system->resource->Find(MESH_BEETLE_DRONE);
    m_meshRenderer->SetupStandardMaterials();
}

void PrefabBeetleDrone::OnImGui()
{
}

bool PrefabBeetleDrone::Raycast(ICamera* camera, V3& out_hit) const
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

string PrefabBeetleDrone::GetType() const
{
	return EDITOR_OBJECT_TYPE_PREFAB_BEETLEDRONE;
}

void PrefabBeetleDrone::OnSerialize(Json::Value& json) const
{
}

void PrefabBeetleDrone::OnDeserialize(const Json::Value& json)
{
}
