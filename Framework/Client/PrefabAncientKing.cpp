#include "stdafx.h"
#include "PrefabAncientKing.h"
#include "EditorConfig.h"
#include "Config.h"
#include "BossAncientKing.h"
#include "Editor.h"

EDITOR_USE

void PrefabAncientKing::Awake()
{
	if (Editor::IsEditorMode())
	{
		IEditorObject::Awake();

		GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
		goMeshRenderer->transform->localPosition = ADJUST_ANCIENTKING_LOCALPOSITION;
		goMeshRenderer->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
		goMeshRenderer->transform->localScale = ADJUST_ANCIENTKING_LOCALSCALE;
		m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
		m_meshRenderer->mesh = system->resource->Find(MESH_ANCIENT_KING);
		m_meshRenderer->SetupStandardMaterials();
	}
}

void PrefabAncientKing::OnImGui()
{
}

bool PrefabAncientKing::Raycast(ICamera* camera, V3& out_hit) const
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

string PrefabAncientKing::GetType() const
{
	return EDITOR_OBJECT_TYPE_PREFAB_ANCIENTKING;
}

void PrefabAncientKing::OnSerialize(Json::Value& json) const
{
}

void PrefabAncientKing::OnDeserialize(const Json::Value& json)
{
}

void PrefabAncientKing::OnDeserializeInRuntime(const Json::Value& json)
{
	gameObject->AddComponent<BossAncientKing>();
}
