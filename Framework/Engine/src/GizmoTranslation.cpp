#include "EnginePCH.h"
#include "GizmoTranslation.h"
#include "System.h"
#include "Input.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "GraphicSystem.h"
#include "CameraManager.h"
#include "ResourceManagement.h"
#include "ResourceFactory.h"
#include "BuiltInResources.h"
#include "GameObject.h"
#include "Texture2D.h"

GizmoBase::Axis GizmoTranslation::PickTest() const
{
	Ray ray = system->input->GetRayInWorldSpace();
	V3 hitOnAxisBox;

	GizmoBase::Axis minDistAxis = GizmoBase::Axis::None;
	float minDist = FLT_MAX;

	if (m_xRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_xRenderer->transform->localToWorldMatrix))
	{
		float dist = V3::Distance(ray.origin, hitOnAxisBox);
		if (dist < minDist)
		{
			minDistAxis = GizmoBase::Axis::X;
			minDist = dist;
		}
	}
	if (m_yRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_yRenderer->transform->localToWorldMatrix))
	{
		float dist = V3::Distance(ray.origin, hitOnAxisBox);
		if (dist < minDist)
		{
			minDistAxis = GizmoBase::Axis::Y;
			minDist = dist;
		}
	}
	if (m_zRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_zRenderer->transform->localToWorldMatrix))
	{
		float dist = V3::Distance(ray.origin, hitOnAxisBox);
		if (dist < minDist)
		{
			minDistAxis = GizmoBase::Axis::Z;
			minDist = dist;
		}
	}

	return minDistAxis;
}

MeshRenderer* GizmoTranslation::AxisRenderer(GizmoBase::Axis axis) const
{
	switch (axis)
	{
		case GizmoBase::Axis::X:
			return m_xRenderer;
		case GizmoBase::Axis::Y:
			return m_yRenderer;
		case GizmoBase::Axis::Z:
			return m_zRenderer;
	}

	return nullptr;
}

bool GizmoTranslation::ProjectOnAxis(GizmoBase::Axis axis, V3& inout_point) const
{
	MeshRenderer* axisRenderer = AxisRenderer(axis);
	if (!axisRenderer)
		return false;

	V3 localPoint = inout_point - axisRenderer->transform->position;
	inout_point = V3::Project(localPoint, axisRenderer->transform->forward) + axisRenderer->transform->position;
	return true;
}

bool GizmoTranslation::VirtualPlane(GizmoBase::Axis axis, const V3& look, Plane& out_plane) const
{
	MeshRenderer* axisRenderer = AxisRenderer(axis);
	if (!axisRenderer)
		return false;

	V3 vertical = V3::Cross(look, axisRenderer->transform->forward);
	if (vertical.magnitude == 0)
		return false;
	out_plane = Plane(
		axisRenderer->transform->position,
		axisRenderer->transform->position + vertical,
		axisRenderer->transform->position + axisRenderer->transform->forward
	);
	return true;
}

bool GizmoTranslation::MouseOnVirtualPlane(GizmoBase::Axis axis, V3& out_point) const
{
	if (Camera* mainCamera = dynamic_cast<Camera*>(system->graphic->cameraManager->mainCamera))
	{
		Plane virtualPlane;
		if (VirtualPlane(axis, mainCamera->transform->forward, virtualPlane))
		{
			Ray ray = system->input->GetRayInWorldSpace();
			float distance;

			if (virtualPlane.Raycast(ray, distance))
			{
				out_point = ray.GetPoint(Abs(distance));
				ProjectOnAxis(axis, out_point);

				return true;
			}
		}
	}
	return false;
}

void GizmoTranslation::Awake()
{
	executionOrder = INT_MAX;

	SetupResources();
	SetupObjects();
}

void GizmoTranslation::GizmoUpdate()
{
	// Sync to handling transform
	transform->position = handlingTransform->position;
	if (isLocalGizmo)
		transform->rotation = handlingTransform->rotation;
	else
		transform->rotation = Q::identity();

	// Drag Axis
	if (m_hitAxis != GizmoBase::Axis::None)
	{
		V3 pointOnPlane;
		MouseOnVirtualPlane(m_hitAxis, pointOnPlane);

		V3 delta = pointOnPlane - m_hitPointOnAxis;
		transform->position = m_hitGizmoPosition + delta;

		// Apply Position
		handlingTransform->transform->position = transform->position;
	}

	// Select Axis
	if (system->input->GetKeyDown(Key::LeftMouse))
	{
		m_hitAxis = PickTest();

		MouseOnVirtualPlane(m_hitAxis, m_hitPointOnAxis);

		m_hitGizmoPosition = transform->position;
	}

	// Unselect
	if (system->input->GetKeyUp(Key::LeftMouse))
	{
		m_hitAxis = GizmoBase::Axis::None;
	}
}

void GizmoTranslation::OnChangedGizmoState()
{
}

void GizmoTranslation::SetupResources()
{
	if (!m_axisMesh)
	{
		m_axisMesh = system->resource->builtIn->boxMesh;
	}
	//if (!m_rTexture)
	//{
	//	m_rTexture = system->resource->factory->CreateUnmanagedTexture2D(Color::red(), 16, 16);
	//}
	//if (!m_gTexture)
	//{
	//	m_gTexture = system->resource->factory->CreateUnmanagedTexture2D(Color::green(), 16, 16);
	//}
	//if (!m_bTexture)
	//{
	//	m_bTexture = system->resource->factory->CreateUnmanagedTexture2D(Color::blue(), 16, 16);
	//}
	//if (!m_rMat)
	//{
	//	m_rMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoTranslation>();
	//	m_rMat->diffuseTexture = m_rTexture;
	//}
	//if (!m_gMat)
	//{
	//	m_gMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoTranslation>();
	//	m_gMat->diffuseTexture = m_gTexture;
	//}
	//if (!m_bMat)
	//{
	//	m_bMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoTranslation>();
	//	m_bMat->diffuseTexture = m_bTexture;
	//}
}

void GizmoTranslation::SetupObjects()
{
	GameObject* goX = CreateGameObjectToChild(transform);
	GameObject* goY = CreateGameObjectToChild(transform);
	GameObject* goZ = CreateGameObjectToChild(transform);

	goX->transform->localScale = V3(0.05f, 0.05f, 1.00f);
	goY->transform->localScale = V3(0.05f, 0.05f, 1.00f);
	goZ->transform->localScale = V3(0.05f, 0.05f, 1.00f);

	goX->transform->localPosition = V3(0.5f, 0.0f, 0.0f);
	goY->transform->localPosition = V3(0.0f, 0.5f, 0.0f);
	goZ->transform->localPosition = V3(0.0f, 0.0f, 0.5f);

	goX->transform->localEulerAngles = V3(+00.0f, +90.0f, +00.0f); // forward: right
	goY->transform->localEulerAngles = V3(-90.0f, +00.0f, +00.0f); // forward: up
	goZ->transform->localEulerAngles = V3(+00.0f, +00.0f, +00.0f); // forward: forward

	m_xRenderer = goX->AddComponent<MeshRenderer>();
	m_yRenderer = goY->AddComponent<MeshRenderer>();
	m_zRenderer = goZ->AddComponent<MeshRenderer>();

	m_xRenderer->mesh = m_axisMesh;
	m_yRenderer->mesh = m_axisMesh;
	m_zRenderer->mesh = m_axisMesh;

	m_xRenderer->material = m_rMat;
	m_yRenderer->material = m_gMat;
	m_zRenderer->material = m_bMat;
}
