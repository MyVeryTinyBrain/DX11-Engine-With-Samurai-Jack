#include "stdafx.h"
#include "Lava.h"
#include "Config.h"
#include "EditorConfig.h"

void Lava::Awake()
{
	EDITOR::IEditorObject::Awake();

	ResourceRef<Shader> lavaShader = system->resource->factory->LoadShaderFromBinaryFolderM(TEXT("Lava.cso"));
	ResourceRef<Texture> noise03 = system->resource->Find(TEX_NOISE_03);
	ResourceRef<Texture> noise02 = system->resource->Find(TEX_NOISE_02);
	ResourceRef<Material> lavaMaterial = system->resource->factory->CreateMaterialByShaderUM(lavaShader);
	lavaMaterial->SetColor("_MinColor", Color::RGBA255(240, 60, 5, 255));
	lavaMaterial->SetColor("_MaxColor", Color::RGBA255(255, 240, 20, 255));
	lavaMaterial->SetTexture("_DistortionTexture", noise03);
	lavaMaterial->SetTexture("_BaseNoiseTexture", noise02);

	GameObject* goMesh = CreateGameObjectToChild(transform);
	goMesh->transform->eulerAngles = V3(90, 0, 0);
	m_meshRenderer = goMesh->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->builtIn->quadMesh;
	m_meshRenderer->material = lavaMaterial;
}

void Lava::OnImGui()
{
}

bool Lava::Raycast(ICamera* camera, V3& out_hit) const
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

string Lava::GetType() const
{
	return EDITOR_OBJECT_TYPE_EDITABLE_LAVA;
}

void Lava::OnSerialize(Json::Value& json) const
{
}

void Lava::OnDeserialize(const Json::Value& json)
{
}
