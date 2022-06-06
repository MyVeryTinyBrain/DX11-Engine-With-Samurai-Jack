#include "stdafx.h"
#include "SerializableSphereCollider.h"
#include "EditorConfig.h"

EDITOR_USE

void SerializableSphereCollider::Awake()
{
	ISerializableCollider::Awake();

	m_sphereCollider = gameObject->AddComponent<SphereCollider>();
	m_sphereCollider->debugRender = true;
}

Collider* SerializableSphereCollider::GetCollider() const
{
	return m_sphereCollider;
}

bool SerializableSphereCollider::Raycast(ICamera* camera, V3& out_hit) const
{
	ResourceRef<Mesh> debugMesh = m_sphereCollider->GetDebugMesh();

	if (!debugMesh)
		return false;

	M4 localToWorldMatrix = m_sphereCollider->GetDebugRenderWorldMatrix();
	Ray ray = system->input->GetRayInWorldSpace(camera);
	if (debugMesh->RaycastInWorldSpace(out_hit, ray, localToWorldMatrix, true))
	{
		return true;
	}
	else
	{
		return false;
	}
}

string SerializableSphereCollider::GetType() const
{
	return EDITOR_OBJECT_TYPE_SPHERE_COLLIDER;
}

void SerializableSphereCollider::OnImGui()
{
	ISerializableCollider::OnImGui();

	if (ImGui::TreeNode("SphereCollider"))
	{
		float radius = m_sphereCollider->radius;
		if (ImGui::DragFloat("radius", &radius, 0.1f, 0.01f, 100.0f))
		{
			m_sphereCollider->radius = radius;
		}

		ImGui::TreePop();
	}
}

void SerializableSphereCollider::OnSerialize(Json::Value& json) const
{
	ISerializableCollider::OnSerialize(json);

	json["radius"] = m_sphereCollider->radius;
}

void SerializableSphereCollider::OnDeserialize(const Json::Value& json)
{
	ISerializableCollider::OnDeserialize(json);

	m_sphereCollider->radius = json["radius"].asFloat();
}
