#include "EnginePCH.h"
#include "Gizmo.h"
#include "GizmoTranslation.h"
#include "GizmoRotation.h"
#include "GizmoScaling.h"
#include "GameObject.h"
#include "System.h"
#include "Input.h"
#include "GraphicSystem.h"
#include "CameraManager.h"
#include "Camera.h"

GizmoBase::Axis Gizmo::PickTest() const
{
	return m_gizmos[m_currentGizmo]->PickTest();
}

void Gizmo::ShowGizmo(bool value)
{
	m_showGizmo = value;

	SetGizmo(m_currentGizmo);
}

void Gizmo::NextGizmo()
{
	SetGizmo(m_currentGizmo + 1);
}

void Gizmo::PrevGizmo()
{
	uint index = m_currentGizmo;
	if (index == 0)
		index = 3;

	SetGizmo(index - 1);
}

void Gizmo::SetGizmo(uint index)
{
	for (auto& gizmo : m_gizmos)
		gizmo->gameObject->activeSelf = false;

	index = Repeat(index, 3u);
	m_currentGizmo = index;

	GizmoBase* currentGizmo = m_gizmos[m_currentGizmo];
	currentGizmo->gameObject->activeSelf = m_showGizmo;

	currentGizmo->handlingTransform = m_handlingTransform;
	currentGizmo->isLocalGizmo = m_localSpace;
}

void Gizmo::SetHandlingTransform(Ref<Transform> handlingTransform)
{
	m_handlingTransform = handlingTransform;

	for (auto& gizmo : m_gizmos)
	{
		gizmo->SetHandlingTransform(handlingTransform);
	}
}

Ref<Transform> Gizmo::GetHandlingTransform() const
{
	return m_handlingTransform;
}

void Gizmo::SetGizmoSpace(bool localSpace)
{
	m_localSpace = localSpace;

	for (auto& gizmo : m_gizmos)
	{
		gizmo->SetGizmoSpace(localSpace);
	}
}

void Gizmo::Awake()
{
	SetupObjects();
}

void Gizmo::PostUpdate()
{
	if (system->input->GetKeyDown(Key::F1))
		SetGizmo(0);
	if (system->input->GetKeyDown(Key::F2))
		SetGizmo(1);
	if (system->input->GetKeyDown(Key::F3))
		SetGizmo(2);
	if (system->input->GetKeyDown(Key::F4))
		SetGizmoSpace(!m_localSpace);

	AutoScaling();
}

void Gizmo::AutoScaling()
{
	GizmoBase* gizmo = m_gizmos[m_currentGizmo];

	if (Camera* mainCamera = dynamic_cast<Camera*>(system->graphic->cameraManager->mainCamera))
	{
		float distance = V3::Distance(gizmo->transform->position, mainCamera->position);
		XM_CONSTEXPR float fixedScalingFactor = 0.17f;
		float scale = distance / (1.0f / fixedScalingFactor);
		transform->localScale = V3::one() * Max(0.01f, scale);
	}
}

void Gizmo::SetupObjects()
{
	GameObject* goTranslation = CreateGameObjectToChild(transform);
	GameObject* goRotation = CreateGameObjectToChild(transform);
	GameObject* goScaling = CreateGameObjectToChild(transform);

	m_translation = goTranslation->AddComponent<GizmoTranslation>();
	m_rotation = goRotation->AddComponent<GizmoRotation>();
	m_scaling = goScaling->AddComponent<GizmoScaling>();

	m_gizmos[0] = m_translation;
	m_gizmos[1] = m_rotation;
	m_gizmos[2] = m_scaling;

	for (auto& gizmo : m_gizmos)
		gizmo->gameObject->activeSelf = false;
}
