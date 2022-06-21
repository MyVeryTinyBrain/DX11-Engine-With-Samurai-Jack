#include "stdafx.h"
#include "DirtGround.h"
#include "EditorConfig.h"
#include "Config.h"

EDITOR_USE

void DirtGround::Awake()
{
	IEditorObject::Awake();

	GameObject* goMesh = CreateGameObjectToChild(transform);
	m_meshRenderer = goMesh->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->builtIn->boxMesh;
	
	ResourceRef<Shader> shader = system->resource->builtIn->standardShader;
	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);

	ResourceRef<Texture> albedo = system->resource->Find(TEX_LANDSCAPE_MINE_GROUND_BC);
	ResourceRef<Texture> normalMap = system->resource->Find(TEX_LANDSCAPE_MINE_GROUND_N);
	ResourceRef<Texture> roughness = system->resource->Find(TEX_LANDSCAPE_MINE_GROUND_MR);
	ResourceRef<Texture> metallic = system->resource->Find(TEX_LANDSCAPE_MINE_GROUND_MR);

	material->SetTexture("_AlbedoTexture", albedo);
	material->SetTexture("_NormalMapTexture", normalMap);
	material->SetTexture("_RoughnessTexture", roughness);
	material->SetTexture("_MetallicTexture", metallic);
	material->SetTexture("_OcclusionTexture", system->resource->builtIn->grayTexture);

	m_meshRenderer->material = material;

	Rigidbody* rigidbody = gameObject->AddComponent<Rigidbody>();
	rigidbody->kinematic = true;
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->layerIndex = PhysicsLayer_Default;
}

void DirtGround::Update()
{
	IEditorObject::Update();

	V2 uvScale;
	uvScale.x = transform->lossyScale.x * 0.1f;
	uvScale.y = transform->lossyScale.z * 0.1f;
	m_meshRenderer->material->SetVector("_UVScale", uvScale);
}

void DirtGround::OnImGui()
{
}

bool DirtGround::Raycast(ICamera* camera, V3& out_hit) const
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

string DirtGround::GetType() const
{
	return EDITOR_OBJECT_TYPE_DIRTGROUND;
}

void DirtGround::OnSerialize(Json::Value& json) const
{
}

void DirtGround::OnDeserialize(const Json::Value& json)
{
}

void DirtGround::OnDeserializeInRuntime(const Json::Value& json)
{
}
