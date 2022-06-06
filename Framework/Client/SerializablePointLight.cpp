#include "stdafx.h"
#include "SerializablePointLight.h"
#include "EditorConfig.h"

EDITOR_USE

Light* SerializablePointLight::InitLight()
{
	m_pointLight = gameObject->AddComponent<PointLight>();
	return m_pointLight;
}

string SerializablePointLight::GetType() const
{
	return EDITOR_OBJECT_TYPE_POINT_LIGHT;
}

void SerializablePointLight::OnImGui()
{
	if (ImGui::TreeNode("Point Light"))
	{
		float range = m_pointLight->range;
		ImGui::SliderFloat("range", &range, 0, 60);
		m_pointLight->range = range;

		ImGui::TreePop();
	}

	ISerializableLight::OnImGui();
}

void SerializablePointLight::OnSerialize(Json::Value& json) const
{
	json["range"] = m_pointLight->range;

	ISerializableLight::OnSerialize(json);
}

void SerializablePointLight::OnDeserialize(const Json::Value& json)
{
	m_pointLight->range = json["range"].asFloat();

	ISerializableLight::OnDeserialize(json);
}
