#include "EnginePCH.h"
#include "GizmoScaling.h"
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
#include "Material.h"

GizmoBase::Axis GizmoScaling::PickTest() const
{
	Ray ray = system->input->GetRayInWorldSpace(system->graphic->cameraManager->mainCamera);
	V3 hitOnAxisBox;

	GizmoBase::Axis minDistAxis = GizmoBase::Axis::None;
	float minDist = FLT_MAX;

	if (m_pRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_pRenderer->transform->localToWorldMatrix))
	{
		float dist = V3::Distance(ray.origin, hitOnAxisBox);
		if (dist < minDist)
		{
			minDistAxis = GizmoBase::Axis::Pivot;
			minDist = dist;
		}
	}
	if (m_xRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_xRenderer->transform->localToWorldMatrix))
	{
		float dist = V3::Distance(ray.origin, hitOnAxisBox);
		if (dist < minDist)
		{
			minDistAxis = GizmoBase::Axis::X;
			minDist = dist;
		}
	}
	if (m_xpRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_xpRenderer->transform->localToWorldMatrix))
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
	if (m_ypRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_ypRenderer->transform->localToWorldMatrix))
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
	if (m_zpRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_zpRenderer->transform->localToWorldMatrix))
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

MeshRenderer* GizmoScaling::AxisRenderer(GizmoBase::Axis axis) const
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

MeshRenderer* GizmoScaling::AxisPivotRenderer(GizmoBase::Axis axis) const
{
	switch (axis)
	{
		case GizmoBase::Axis::X:
			return m_xpRenderer;
		case GizmoBase::Axis::Y:
			return m_ypRenderer;
		case GizmoBase::Axis::Z:
			return m_zpRenderer;
	}

	return nullptr;
}

bool GizmoScaling::ProjectOnAxis(GizmoBase::Axis axis, V3& inout_point) const
{
	MeshRenderer* axisRenderer = AxisRenderer(axis);
	if (!axisRenderer)
		return false;

	V3 localPoint = inout_point - axisRenderer->transform->position;
	inout_point = V3::Project(localPoint, axisRenderer->transform->forward) + axisRenderer->transform->position;
	return true;
}

bool GizmoScaling::VirtualPlane(GizmoBase::Axis axis, const V3& look, Plane& out_plane) const
{
	switch (axis)
	{
		case GizmoBase::Axis::X:
		case GizmoBase::Axis::Y:
		case GizmoBase::Axis::Z:
		{
			MeshRenderer* axisRenderer = AxisRenderer(axis);
			if (!axisRenderer)
				break;

			V3 vertical = V3::Cross(look, axisRenderer->transform->forward);
			if (vertical.magnitude == 0)
				break;

			out_plane = Plane(
				axisRenderer->transform->position,
				axisRenderer->transform->position + vertical,
				axisRenderer->transform->position + axisRenderer->transform->forward
			);
			return true;
		}
		break;
		case GizmoBase::Axis::Pivot:
		{
			out_plane = Plane(look, transform->position);
			return true;
		}
		break;
	}

	return false;
}

bool GizmoScaling::MouseOnVirtualHandle(GizmoBase::Axis axis, V3& out_point) const
{
	if (Camera* mainCamera = dynamic_cast<Camera*>(system->graphic->cameraManager->mainCamera))
	{
		Plane virtualPlane;
		if (VirtualPlane(axis, mainCamera->transform->forward, virtualPlane))
		{
			Ray ray = system->input->GetRayInWorldSpace(mainCamera);
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

void GizmoScaling::RollbackTransformationSelf()
{
	m_xRenderer->transform->localScale = V3(m_axisWidth, m_axisWidth, 1.00f);
	m_yRenderer->transform->localScale = V3(m_axisWidth, m_axisWidth, 1.00f);
	m_zRenderer->transform->localScale = V3(m_axisWidth, m_axisWidth, 1.00f);
	m_pRenderer->transform->localScale = V3::one() * m_pivotWidth;
	m_xpRenderer->transform->localScale = V3::one() * m_pivotWidth;
	m_ypRenderer->transform->localScale = V3::one() * m_pivotWidth;
	m_zpRenderer->transform->localScale = V3::one() * m_pivotWidth;

	m_xRenderer->transform->localPosition = V3(0.5f, 0.0f, 0.0f);
	m_yRenderer->transform->localPosition = V3(0.0f, 0.5f, 0.0f);
	m_zRenderer->transform->localPosition = V3(0.0f, 0.0f, 0.5f);
	m_xpRenderer->transform->localPosition = V3(1.0f, 0.0f, 0.0f);
	m_ypRenderer->transform->localPosition = V3(0.0f, 1.0f, 0.0f);
	m_zpRenderer->transform->localPosition = V3(0.0f, 0.0f, 1.0f);

	m_xRenderer->transform->localEulerAngles = V3(+00.0f, +90.0f, +00.0f); // forward: right
	m_yRenderer->transform->localEulerAngles = V3(-90.0f, +00.0f, +00.0f); // forward: up
	m_zRenderer->transform->localEulerAngles = V3(+00.0f, +00.0f, +00.0f); // forward: forward
}

void GizmoScaling::ApplyTransformationSelf(GizmoBase::Axis axis, float signedMagDelta)
{
	auto ApplyTransformationToAxis = [this](GizmoBase::Axis axis, float signedMagDelta)
	{
		MeshRenderer* axisRenderer = AxisRenderer(axis);
		MeshRenderer* axisPivotRenderer = AxisPivotRenderer(axis);
		switch (axis)
		{
			case GizmoBase::Axis::X:
				axisRenderer->transform->localScale = V3(m_axisWidth, m_axisWidth, 1.00f + signedMagDelta);
				axisRenderer->transform->localPosition = V3((1.00f + signedMagDelta) * 0.5f, 0.0f, 0.0f);
				axisPivotRenderer->transform->localPosition = V3(1.00f + signedMagDelta, 0.0f, 0.0f);
				break;
			case GizmoBase::Axis::Y:
				axisRenderer->transform->localScale = V3(m_axisWidth, m_axisWidth, 1.00f + signedMagDelta);
				axisRenderer->transform->localPosition = V3(0.0f, (1.00f + signedMagDelta) * 0.5f, 0.0f);
				axisPivotRenderer->transform->localPosition = V3(0.0f, 1.00f + signedMagDelta, 0.0f);
				break;
			case GizmoBase::Axis::Z:
				axisRenderer->transform->localScale = V3(m_axisWidth, m_axisWidth, 1.00f + signedMagDelta);
				axisRenderer->transform->localPosition = V3(0.0f, 0.0f, (1.00f + signedMagDelta) * 0.5f);
				axisPivotRenderer->transform->localPosition = V3(0.0f, 0.0f, 1.00f + signedMagDelta);
				break;
		}
	};

	switch (axis)
	{
		case GizmoBase::Axis::X:
		case GizmoBase::Axis::Y:
		case GizmoBase::Axis::Z:
			ApplyTransformationToAxis(axis, signedMagDelta);
			break;
		case GizmoBase::Axis::Pivot:
			ApplyTransformationToAxis(GizmoBase::Axis::X, signedMagDelta);
			ApplyTransformationToAxis(GizmoBase::Axis::Y, signedMagDelta);
			ApplyTransformationToAxis(GizmoBase::Axis::Z, signedMagDelta);
			break;
	}
}

void GizmoScaling::Awake()
{
	executionOrder = INT_MAX;

	SetupResources();
	SetupObjects();
}

void GizmoScaling::GizmoUpdate()
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
		V3 pointOnHandle;
		MouseOnVirtualHandle(m_hitAxis, pointOnHandle);

		V3 delta = pointOnHandle - m_hitPointOnAxis;
		float magDelta = delta.magnitude;
		float signedMagDelta = magDelta;

		switch (m_hitAxis)
		{
			case GizmoBase::Axis::X:
			case GizmoBase::Axis::Y:
			case GizmoBase::Axis::Z:
			{
				MeshRenderer* axisRenderer = AxisRenderer(m_hitAxis);

				// 축의 방향과 최초 선택된 점으로부터 축의 평면에 투영된 점까지에 대한 방향벡터의 내적의 결과는
				// 최초 선택된 점으로부터 축의 평면에 투영된 점까지에 대한 방향벡터가 축의 방향으로부터 양의 방향인지 음의 방향인지에 대한 결과입니다.
				// 또한 transform->right, up, forward 등의 기저벡터는 스케일의 영향을 받으므로 스케일의 영향을 받지 않는 쿼터니언을 축의 방향으로 사용합니다.
				float sign = Sign(V3::Dot(axisRenderer->transform->rotation.MultiplyVector(V3::forward()), (pointOnHandle - m_hitPointOnAxis).normalized));
				signedMagDelta = magDelta * sign;
			}
			break;
			case GizmoBase::Axis::Pivot:
			{
				if (Camera* mainCamera = dynamic_cast<Camera*>(system->graphic->cameraManager->mainCamera))
				{
					float signedPercent = V3::Dot(mainCamera->transform->right, (pointOnHandle - m_pRenderer->transform->position).normalized);
					signedMagDelta = magDelta * signedPercent;
				}
			}
			break;
		}

		ApplyTransformationSelf(m_hitAxis, signedMagDelta);

		// Apply Scaling
		float adjustedSingedMagDelta = signedMagDelta * 0.5f;

		switch (m_hitAxis)
		{
			case GizmoBase::Axis::X:
				handlingTransform->transform->localScale = m_hitHandleTransformLocalScale + V3(adjustedSingedMagDelta, 0, 0);
				break;
			case GizmoBase::Axis::Y:
				handlingTransform->transform->localScale = m_hitHandleTransformLocalScale + V3(0, adjustedSingedMagDelta, 0);
				break;
			case GizmoBase::Axis::Z:
				handlingTransform->transform->localScale = m_hitHandleTransformLocalScale + V3(0, 0, adjustedSingedMagDelta);
				break;
			case GizmoBase::Axis::Pivot:
				handlingTransform->transform->localScale = m_hitHandleTransformLocalScale + V3(adjustedSingedMagDelta, adjustedSingedMagDelta, adjustedSingedMagDelta);
				break;
		}
	}

	// Select Axis
	if (!IsOnEnabled() && system->input->GetKeyDown(Key::LeftMouse))
	{
		m_hitAxis = PickTest();

		m_hitHandleTransformLocalScale = handlingTransform->localScale;

		MouseOnVirtualHandle(m_hitAxis, m_hitPointOnAxis);
	}

	// Unselect
	if (system->input->GetKeyUp(Key::LeftMouse))
	{
		m_hitAxis = GizmoBase::Axis::None;
		RollbackTransformationSelf();
	}
}

void GizmoScaling::OnChangedGizmoState()
{
}

void GizmoScaling::SetupResources()
{
	if (!m_axisMesh)
	{
		m_axisMesh = system->resource->builtIn->boxMesh;
	}
	
	ResourceRef<Shader> shader = system->resource->builtIn->colorShader;

	m_rMat = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_rMat->SetColor("_Color", Color::red());
	m_rMat->techniqueIndex = 1;

	m_gMat = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_gMat->SetColor("_Color", Color::green());
	m_gMat->techniqueIndex = 1;

	m_bMat = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_bMat->SetColor("_Color", Color::blue());
	m_bMat->techniqueIndex = 1;

	m_pMat = system->resource->factory->CreateMaterialByShaderUM(shader);
	m_pMat->SetColor("_Color", Color::white());
	m_pMat->techniqueIndex = 2;
}

void GizmoScaling::SetupObjects()
{
	GameObject* goX =  CreateGameObjectToChild(transform);
	GameObject* goY =  CreateGameObjectToChild(transform);
	GameObject* goZ =  CreateGameObjectToChild(transform);
	GameObject* goP =  CreateGameObjectToChild(transform);
	GameObject* goXP = CreateGameObjectToChild(transform);
	GameObject* goYP = CreateGameObjectToChild(transform);
	GameObject* goZP = CreateGameObjectToChild(transform);

	m_xRenderer = goX->AddComponent<MeshRenderer>();
	m_yRenderer = goY->AddComponent<MeshRenderer>();
	m_zRenderer = goZ->AddComponent<MeshRenderer>();
	m_pRenderer = goP->AddComponent<MeshRenderer>();
	m_xpRenderer = goXP->AddComponent<MeshRenderer>();
	m_ypRenderer = goYP->AddComponent<MeshRenderer>();
	m_zpRenderer = goZP->AddComponent<MeshRenderer>();

	RollbackTransformationSelf();

	m_xRenderer->mesh = m_axisMesh;
	m_yRenderer->mesh = m_axisMesh;
	m_zRenderer->mesh = m_axisMesh;
	m_pRenderer->mesh = m_axisMesh;
	m_xpRenderer->mesh = m_axisMesh;
	m_ypRenderer->mesh = m_axisMesh;
	m_zpRenderer->mesh = m_axisMesh;

	m_xRenderer->material = m_rMat;
	m_yRenderer->material = m_gMat;
	m_zRenderer->material = m_bMat;
	m_pRenderer->material = m_pMat;
	m_xpRenderer->material = m_rMat;
	m_ypRenderer->material = m_gMat;
	m_zpRenderer->material = m_bMat;
}
