#include "stdafx.h"
#include "ISerializableLight.h"
#include "EditorConfig.h"

EDITOR_USE

void ISerializableLight::Awake()
{
	IEditorObject::Awake();

	GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
	goMeshRenderer->transform->localScale = V3::one() * 0.25f;
	goMeshRenderer->transform->localEulerAngles = V3(90.0f, 0.0f, 0.0f);
	m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->builtIn->cylinderMesh;
	m_meshRenderer->material = system->resource->builtIn->whiteUnlitMaterial;

	m_light = InitLight();
}

void ISerializableLight::OnImGui()
{
	if (ImGui::TreeNode("Light"))
	{
		float intensity = m_light->intensity;
		ImGui::SliderFloat("intensity", &intensity, 0, 60);
		m_light->intensity = intensity;

		Color color = m_light->diffuse;
		ImGui::ColorPicker3("diffuse", (float*)&color);
		m_light->diffuse = color;

		Color ambient = m_light->ambient;
		ImGui::ColorPicker3("ambient", (float*)&ambient);
		m_light->ambient = ambient;

		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Shadow"))
	{
		bool drawShadow = m_light->drawShadow;
		ImGui::Checkbox("drawShadow", &drawShadow);
		m_light->drawShadow = drawShadow;

		float shadowWhiteness = m_light->shadowWhiteness;
		ImGui::SliderFloat("shadowWhiteness", &shadowWhiteness, 0.0f, 1.0f);
		m_light->shadowWhiteness = shadowWhiteness;

		float shadowBias = m_light->shadowBias;
		ImGui::SliderFloat("shadowBias", &shadowBias, 0.0f, 1.0f);
		m_light->shadowBias = shadowBias;

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Volumetric"))
	{
		bool drawVolumetric = m_light->volumetricLightState;
		ImGui::Checkbox("drawVolumetric", &drawVolumetric);
		m_light->volumetricLightState = drawVolumetric;

		int volumetricLightNumSamples = m_light->volumetricLightNumSamples;
		ImGui::SliderInt("volumetricLightNumSamples", &volumetricLightNumSamples, 0, 200);
		m_light->volumetricLightNumSamples = volumetricLightNumSamples;

		float volumetricLightIntensity = m_light->volumetricLightIntensity;
		ImGui::SliderFloat("volumetricLightIntensity", &volumetricLightIntensity, 0.0f, 5.0f);
		m_light->volumetricLightIntensity = volumetricLightIntensity;

		float volumetricLightPower = m_light->volumetricLightPower;
		ImGui::SliderFloat("volumetricLightPower", &volumetricLightPower, 0.0f, 5.0f);
		m_light->volumetricLightPower = volumetricLightPower;

		ImGui::TreePop();
	}
}

bool ISerializableLight::Raycast(ICamera* camera, V3& out_hit) const
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

void ISerializableLight::OnSerialize(Json::Value& json) const
{
	json["intensity"] = m_light->intensity;
	json["diffuse"] = JsonUtility::Parse(m_light->diffuse);
	json["ambient"] = JsonUtility::Parse(m_light->ambient);
	json["drawShadow"] = m_light->drawShadow;
	json["shadowWhiteness"] = m_light->shadowWhiteness;
	json["shadowBias"] = m_light->shadowBias;
	json["volumetricLightState"] = m_light->volumetricLightState;
	json["volumetricLightNumSamples"] = m_light->volumetricLightNumSamples;
	json["volumetricLightIntensity"] = m_light->volumetricLightIntensity;
	json["volumetricLightPower"] = m_light->volumetricLightPower;
}

void ISerializableLight::OnDeserialize(const Json::Value& json)
{
	m_light->intensity = json["intensity"].asFloat();

	Color diffuse;
	JsonUtility::ToColor(json["diffuse"].asString(), diffuse);
	m_light->diffuse = diffuse;

	Color ambient;
	JsonUtility::ToColor(json["ambient"].asString(), ambient);
	m_light->ambient = ambient;

	m_light->drawShadow = json["drawShadow"].asBool();
	m_light->shadowWhiteness = json["shadowWhiteness"].asFloat();
	m_light->shadowBias = json["shadowBias"].asFloat();
	m_light->volumetricLightState = json["volumetricLightState"].asBool();
	m_light->volumetricLightNumSamples = json["volumetricLightNumSamples"].asUInt();
	m_light->volumetricLightIntensity = json["volumetricLightIntensity"].asFloat();
	m_light->volumetricLightPower = json["volumetricLightPower"].asFloat();
}
