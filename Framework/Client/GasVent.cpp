#include "stdafx.h"
#include "GasVent.h"
#include "Config.h"
#include "ParticleDust01.h"
#include "EditorConfig.h"
#include "Editor.h"

void GasVent::Awake()
{
	IEditorObject::Awake();

	GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
	goMeshRenderer->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
	m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->Find(MESH_ENV_CAVE_VENT);
	m_meshRenderer->SetupStandardMaterials();
}

void GasVent::Update()
{
	IEditorObject::Update();

	if (EDITOR::Editor::IsEditorMode())
	{
		return;
	}

	int r = rand() % 4;

	if (r > 0)
		return;

	ParticleDust01::CreateWithNormal(
		gameObject->regionScene,
		transform->position,
		transform->up,
		0.0f, 20.0f, 0.5f, 0.3f, 5.0f,
		0.5f, 3.0f,
		0.2f, 4.0f, 1.5f,
		Color(0.5f, 0.5f, 0.5f, 1.0f), Color(0.5f, 0.5f, 0.5f, 0.0f), 1.0f,
		1
	);
}

void GasVent::OnImGui()
{
}

bool GasVent::Raycast(ICamera* camera, V3& out_hit) const
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

string GasVent::GetType() const
{
	return EDITOR_OBJECT_TYPE_GAS_VENT;
}

void GasVent::OnSerialize(Json::Value& json) const
{
}

void GasVent::OnDeserialize(const Json::Value& json)
{
}

void GasVent::OnDeserializeInRuntime(const Json::Value& json)
{
}
