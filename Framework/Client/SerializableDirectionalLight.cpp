#include "stdafx.h"
#include "SerializableDirectionalLight.h"
#include "EditorConfig.h"

EDITOR_USE

Light* SerializableDirectionalLight::InitLight()
{
	m_directionalLight = gameObject->AddComponent<DirectionalLight>();
	return m_directionalLight;
}

string SerializableDirectionalLight::GetType() const
{
	return EDITOR_OBJECT_TYPE_DIRECTIONAL_LIGHT;
}
