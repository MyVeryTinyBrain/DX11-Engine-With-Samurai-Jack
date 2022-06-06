#include "stdafx.h"
#include "SerializableCapsuleCollider.h"
#include "EditorConfig.h"

EDITOR_USE

void SerializableCapsuleCollider::Awake()
{
	ISerializableCollider::Awake();

	m_capsuleCollider = gameObject->AddComponent<CapsuleCollider>();
	m_capsuleCollider->debugRender = true;
}

Collider* SerializableCapsuleCollider::GetCollider() const
{
	return m_capsuleCollider;
}

bool SerializableCapsuleCollider::Raycast(ICamera* camera, V3& out_hit) const
{
	ResourceRef<Mesh> debugMesh = m_capsuleCollider->GetDebugMesh();

	if (!debugMesh)
		return false;

	M4 localToWorldMatrix = m_capsuleCollider->GetDebugRenderWorldMatrix();
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

string SerializableCapsuleCollider::GetType() const
{
	return EDITOR_OBJECT_TYPE_CAPSULE_COLLIDER;
}

void SerializableCapsuleCollider::OnImGui()
{
	ISerializableCollider::OnImGui();

	if (ImGui::TreeNode("CapsuleCollider"))
	{
		float radius = m_capsuleCollider->radius;
		if (ImGui::DragFloat("radius", &radius, 0.1f, 0.01f, 100.0f))
		{
			m_capsuleCollider->radius = radius;
		}

		float halfHeight = m_capsuleCollider->halfHeight;
		if (ImGui::DragFloat("halfHeight", &halfHeight, 0.1f, 0.01f, 100.0f))
		{
			m_capsuleCollider->halfHeight = halfHeight;
		}

		ImGui::TreePop();
	}
}

void SerializableCapsuleCollider::OnSerialize(Json::Value& json) const
{
	ISerializableCollider::OnSerialize(json);

	json["radius"] = m_capsuleCollider->radius;
	json["halfHeight"] = m_capsuleCollider->halfHeight;
}

void SerializableCapsuleCollider::OnDeserialize(const Json::Value& json)
{
	ISerializableCollider::OnDeserialize(json);

	m_capsuleCollider->radius = json["radius"].asFloat();
	m_capsuleCollider->halfHeight = json["halfHeight"].asFloat();
}
