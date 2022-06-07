#include "stdafx.h"
#include "SerializableSpotLight.h"
#include "EditorConfig.h"

EDITOR_USE

Light* SerializableSpotLight::InitLight()
{
	m_spotLight = gameObject->AddComponent<SpotLight>();
	return m_spotLight;
}

string SerializableSpotLight::GetType() const
{
	return EDITOR_OBJECT_TYPE_SPOT_LIGHT;
}

void SerializableSpotLight::OnImGui()
{
	if (ImGui::TreeNode("Spot Light"))
	{
		float range = m_spotLight->range;
		ImGui::SliderFloat("range", &range, 0, 60);
		m_spotLight->range = range;

		float angle = m_spotLight->angle;
		ImGui::SliderFloat("angle", &angle, 0, 90);
		m_spotLight->angle = angle;

		ImGui::TreePop();
	}

	ISerializableLight::OnImGui();
}

void SerializableSpotLight::OnSerialize(Json::Value& json) const
{
	json["range"] = m_spotLight->range;
	json["angle"] = m_spotLight->angle;

	ISerializableLight::OnSerialize(json);
}

void SerializableSpotLight::OnDeserialize(const Json::Value& json)
{
	m_spotLight->range = json["range"].asFloat();
	m_spotLight->angle = json["angle"].asFloat();

	ISerializableLight::OnDeserialize(json);
}

void SerializableSpotLight::OnDeserializeInRuntime(const Json::Value& json)
{
	OnDeserialize(json);
}
