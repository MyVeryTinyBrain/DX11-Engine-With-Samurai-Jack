#include "stdafx.h"
#include "IEditorObject.h"
#include "Editor.h"
#include "EditorObjects.h"

EDITOR_USE

void IEditorObject::Awake()
{
	if (editorObjects)
	{
		editorObjects->RegistObject(this);
	}
}

void IEditorObject::OnDestroyed()
{
	if (editorObjects)
	{
		editorObjects->UnregistObject(this);
	}
}

void IEditorObject::ImGuiRender()
{
	// type
	ImGui::Text(GetType().c_str());

	if (ImGui::TreeNode("GameObject"))
	{
		// name
		string strName = tstring_to_string(gameObject->name);
		char cstrName[256]; strcpy_s(cstrName, strName.c_str());
		ImGui::InputText("name", cstrName, sizeof(cstrName));
		strName = cstrName;
		gameObject->name = string_to_tstring(strName);

		// tag
		string strTag = tstring_to_string(gameObject->tag);
		char cstrTag[256]; strcpy_s(cstrTag, strTag.c_str());
		ImGui::InputText("tag", cstrTag, sizeof(cstrTag));
		strTag = cstrTag;
		gameObject->tag = string_to_tstring(strTag);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Transform"))
	{
		// local position
		V3 localPosition = transform->localPosition;
		ImGui::DragFloat3("local position", (float*)&localPosition, 0.1f);
		transform->localPosition = localPosition;

		// local euler angles
		V3 localEulerAngles = transform->localEulerAngles;
		ImGui::DragFloat3("local euler angles", (float*)&localEulerAngles, 0.1f);
		transform->localEulerAngles = localEulerAngles;

		// local scale
		V3 localScale = transform->localScale;
		ImGui::DragFloat3("localScale", (float*)&localScale, 0.1f);
		transform->localScale = localScale;

		ImGui::TreePop();
	}

	OnImGui();
}

Json::Value IEditorObject::Serialize() const
{
	Json::Value json;

	json["type"] = GetType();
	json["name"] = tstring_to_string(gameObject->name);
	json["tag"] = tstring_to_string(gameObject->tag);
	json["localPosition"] = JsonUtility::Parse(transform->localPosition);
	json["localRotation"] = JsonUtility::Parse(transform->localRotation);
	json["localScale"] = JsonUtility::Parse(transform->localScale);

	OnSerialize(json);

	return json;
}

void IEditorObject::Deserialize(const Json::Value& json)
{
	gameObject->name = string_to_tstring(json["name"].asString());
	gameObject->tag = string_to_tstring(json["tag"].asString());

	V4 localPosition;
	JsonUtility::ToVector4(json["localPosition"].asString(), localPosition);
	transform->localPosition = localPosition;

	Q localRotation;
	JsonUtility::ToQuaternion(json["localRotation"].asString(), localRotation);
	transform->localRotation = localRotation;

	V4 localScale;
	JsonUtility::ToVector4(json["localScale"].asString(), localScale);
	transform->localScale = localScale;

	OnDeserialize(json);
}

void IEditorObject::DeserializeInRuntime(const Json::Value& json)
{
	gameObject->name = string_to_tstring(json["name"].asString());
	gameObject->tag = string_to_tstring(json["tag"].asString());

	V4 localPosition;
	JsonUtility::ToVector4(json["localPosition"].asString(), localPosition);
	transform->localPosition = localPosition;

	Q localRotation;
	JsonUtility::ToQuaternion(json["localRotation"].asString(), localRotation);
	transform->localRotation = localRotation;

	V4 localScale;
	JsonUtility::ToVector4(json["localScale"].asString(), localScale);
	transform->localScale = localScale;

	OnDeserializeInRuntime(json);
}

Editor* IEditorObject::GetEditor() const
{
	Editor* editor = Editor::GetInstance();
	return editor;
}

EditorObjects* IEditorObject::GetEditorObjects() const
{
	Editor* editor = GetEditor();
	if (!editor)
		return nullptr;

	return editor->GetEditorObjects();
}
