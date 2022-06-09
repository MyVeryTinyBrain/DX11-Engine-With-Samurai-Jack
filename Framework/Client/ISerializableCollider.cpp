#include "stdafx.h"
#include "ISerializableCollider.h"
#include "Editor.h"

EDITOR_USE

void ISerializableCollider::Awake()
{
	IEditorObject::Awake();

	if (Editor::IsEditorMode())
	{
		Collider* collider = GetCollider();
		collider->debugRenderMode = Collider::DebugRenderMode::Fill;
	}
	else
	{
		Rigidbody* rigidbody = gameObject->AddComponent<Rigidbody>();
		rigidbody->kinematic = true;
	}
}

void ISerializableCollider::OnImGui()
{
	if (ImGui::TreeNode("Collider"))
	{
		Collider* collider = GetCollider();

		bool isTrigger = collider->isTrigger;
		if (ImGui::Checkbox("isTrigger", &isTrigger))
		{
			collider->isTrigger = isTrigger;
		}

		int layerIndex = (int)collider->layerIndex;
		if (ImGui::InputInt("layerIndex", &layerIndex))
		{
			layerIndex = Clamp(layerIndex, 0, 7);
			collider->layerIndex = (uint)layerIndex;
		}

		ImGui::TreePop();
	}
}

void ISerializableCollider::OnSerialize(Json::Value& json) const
{
	Collider* collider = GetCollider();

	json["isTrigger"] = collider->isTrigger;
	json["layerIndex"] = collider->layerIndex;
}

void ISerializableCollider::OnDeserialize(const Json::Value& json)
{
	Collider* collider = GetCollider();

	collider->isTrigger = json["isTrigger"].asBool();
	collider->layerIndex = json["layerIndex"].asUInt();
}

void ISerializableCollider::OnDeserializeInRuntime(const Json::Value& json)
{
	OnDeserialize(json);
}
