#include "stdafx.h"
#include "SerializableBoxCollider.h"
#include "EditorConfig.h"

EDITOR_USE

void SerializableBoxCollider::Awake()
{
	ISerializableCollider::Awake();

	m_boxCollider = gameObject->AddComponent<BoxCollider>();
	m_boxCollider->debugRender = true;
}

Collider* SerializableBoxCollider::GetCollider() const
{
	return m_boxCollider;
}

bool SerializableBoxCollider::Raycast(ICamera* camera, V3& out_hit) const
{
	ResourceRef<Mesh> debugMesh = m_boxCollider->GetDebugMesh();

	if (!debugMesh)
		return false;

	M4 localToWorldMatrix = m_boxCollider->GetDebugRenderWorldMatrix();
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

string SerializableBoxCollider::GetType() const
{
	return EDITOR_OBJECT_TYPE_BOX_COLLIDER;
}

void SerializableBoxCollider::OnImGui()
{
	ISerializableCollider::OnImGui();

	if (ImGui::TreeNode("BoxCollider"))
	{
		V3 extents = m_boxCollider->extents;
		if (ImGui::DragFloat3("extents", (float*)&extents, 0.1f, 0.01f, 100.0f))
		{
			m_boxCollider->extents = extents;
		}

		ImGui::TreePop();
	}
}

void SerializableBoxCollider::OnSerialize(Json::Value& json) const
{
	ISerializableCollider::OnSerialize(json);

	json["extents"] = JsonUtility::Parse(m_boxCollider->extents);
}

void SerializableBoxCollider::OnDeserialize(const Json::Value& json)
{
	ISerializableCollider::OnDeserialize(json);

	V4 extents;
	JsonUtility::ToVector4(json["extents"].asString(), extents);
	m_boxCollider->extents = extents;
}
