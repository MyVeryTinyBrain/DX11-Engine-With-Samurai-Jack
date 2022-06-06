#include "EnginePCH.h"
#include "GizmoRotation.h"
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
#include "VI.h"
#include "VIBuffer.h"
#include "Material.h"

GizmoBase::Axis GizmoRotation::PickTest() const
{
	Ray ray = system->input->GetRayInWorldSpace(system->graphic->cameraManager->mainCamera);
	V3 hitOnAxisBox;

	GizmoBase::Axis minDistAxis = GizmoBase::Axis::None;
	float minDist = FLT_MAX;

	if (m_xRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_xRenderer->transform->localToWorldMatrix, false))
	{
		float dist = V3::Distance(ray.origin, hitOnAxisBox);
		if (dist < minDist)
		{
			minDistAxis = GizmoBase::Axis::X;
			minDist = dist;
		}
	}
	if (m_yRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_yRenderer->transform->localToWorldMatrix, false))
	{
		float dist = V3::Distance(ray.origin, hitOnAxisBox);
		if (dist < minDist)
		{
			minDistAxis = GizmoBase::Axis::Y;
			minDist = dist;
		}
	}
	if (m_zRenderer->mesh->RaycastInWorldSpace(hitOnAxisBox, ray, m_zRenderer->transform->localToWorldMatrix, false))
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

MeshRenderer* GizmoRotation::AxisRenderer(GizmoBase::Axis axis) const
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

void GizmoRotation::VirtualPlane(const V3& normal, const V3& center, Plane& out_plane) const
{
	out_plane = Plane(normal, center);
}

bool GizmoRotation::MouseOnVirtualPlane(const V3& normal, const V3& center, V3& out_point) const
{
	Plane virtualPlane;
	VirtualPlane(normal, center, virtualPlane);

	Ray ray = system->input->GetRayInWorldSpace(system->graphic->cameraManager->mainCamera);
	float distance;

	if (virtualPlane.Raycast(ray, distance))
	{
		out_point = ray.GetPoint(distance);
		return true;
	}
	return false;
}

void GizmoRotation::UpdateGizmoColors(GizmoBase::Axis highlightedAxis)
{
	switch (highlightedAxis)
	{
		case GizmoBase::Axis::X:
			m_xRenderer->material = m_hrMat;
			m_yRenderer->material = m_gMat;
			m_zRenderer->material = m_bMat;
			break;
		case GizmoBase::Axis::Y:
			m_xRenderer->material = m_rMat;
			m_yRenderer->material = m_hgMat;
			m_zRenderer->material = m_bMat;
			break;
		case GizmoBase::Axis::Z:
			m_xRenderer->material = m_rMat;
			m_yRenderer->material = m_gMat;
			m_zRenderer->material = m_hbMat;
			break;
		default:
			m_xRenderer->material = m_rMat;
			m_yRenderer->material = m_gMat;
			m_zRenderer->material = m_bMat;
			break;
	}
}

void GizmoRotation::Awake()
{
	executionOrder = INT_MAX;

	SetupResources();
	SetupObjects();
}

void GizmoRotation::GizmoUpdate()
{
	// Sync to handling transform
	if (m_hitAxis == GizmoBase::Axis::None)
	{
		transform->position = handlingTransform->position;
		if (isLocalGizmo)
			transform->rotation = handlingTransform->rotation;
		else
			transform->rotation = Q::identity();
	}

	// Drag Axis
	if (m_hitAxis != GizmoBase::Axis::None)
	{
		V3 pointOnPlane;
		MouseOnVirtualPlane(m_hitNormal, m_hitCenter, pointOnPlane);

		V3 originDelta = m_hitPointOnAxis - m_hitCenter;
		V3 originDir = originDelta.normalized;

		V3 newDelta = pointOnPlane - m_hitCenter;
		V3 newDir = newDelta.normalized;

		Q delta = Q::FromToRotation(originDir, newDir);

		transform->rotation = m_hitGizmoRotation * delta;

		// Apply Rotation
		handlingTransform->rotation = m_hitHandleTransformRotation * delta;
	}

	// Find Hightlight Axis
	// Highlight Axis
	if (m_hitAxis == GizmoBase::Axis::None)
	{
		m_highlightedAxis = PickTest();
		UpdateGizmoColors(m_highlightedAxis);
	}

	// Select Axis
	if (system->input->GetKeyDown(Key::LeftMouse) && m_highlightedAxis != GizmoBase::Axis::None)
	{
		m_hitAxis = m_highlightedAxis;

		// ������ ����� ���� ������ �����մϴ�.
		// �� �������� ������ ���� ����� �����մϴ�.
		// �Ź� ���� ������ ����Ѵٸ� ������ �����˴ϴ�.
		// ���� ȸ���ϴ� ���� ������ �������δ� ȸ������ �ʱ� ������ �� ����� ����մϴ�.
		// MouseOnVirtualPlane �� �Լ��� ���ڷ� ������ �޴� ������ �̰� �����Դϴ�.
		m_hitNormal = AxisRenderer(m_hitAxis)->transform->forward;

		m_hitCenter = AxisRenderer(m_hitAxis)->transform->position;

		m_hitHandleTransformRotation = handlingTransform->rotation;

		MeshRenderer* axisRenderer = AxisRenderer(m_hitAxis);
		MouseOnVirtualPlane(m_hitNormal, axisRenderer->transform->position, m_hitPointOnAxis);

		m_hitGizmoRotation = transform->rotation;
	}

	// Unselect
	if (system->input->GetKeyUp(Key::LeftMouse))
	{
		m_hitAxis = GizmoBase::Axis::None;
	}
}

void GizmoRotation::OnChangedGizmoState()
{
}

void GizmoRotation::SetupAxisMesh(uint edge)
{
	edge = Clamp(edge, 3u, UINT_MAX);
	
	// Setup Vertices
	// Circle plane which is look at back
	uint numVertices = edge + 1;
	Vertex* vertices = new Vertex[numVertices]{};
	for (uint i = 0; i < edge; ++i)
	{
		float spinPercent = float(i) / float(edge - 1);
		float rad = spinPercent * TAU;
		vertices[i].position.x = Cos(rad);
		vertices[i].position.y = Sin(rad);
		vertices[i].position.z = 0.0f;
		vertices[i].uvw.x = vertices[i].position.x * 0.5f + 0.5f;
		vertices[i].uvw.y = vertices[i].position.y * 0.5f + 0.5f;
		vertices[i].normal = V3::back();
	}
	uint centerIndex = edge;
	vertices[centerIndex].position = V3::zero();
	vertices[centerIndex].uvw = V2::one() * 0.5f;
	vertices[centerIndex].normal = V3::back();

	// Setup Triangles
	uint numTriangles = edge;
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};
	for (uint i = 0; i < numTriangles; ++i)
	{
		triangles[i]._0 = i;
		triangles[i]._1 = centerIndex;
		triangles[i]._2 = Repeat(i + 1, centerIndex);
	}

	// Setup VI
	VI* vi = new VI;
	vi->SetVerticesNocopy(&vertices, numVertices);
	vi->SetIndicesAsTrianglesNocopy(&triangles, numTriangles);

	// Setup VIBuffer
	VIBuffer* viBuffer = nullptr;
	if (FAILED(VIBuffer::CreateVIBufferNocopy(
		system->graphic->device, system->graphic->deviceContext,
		&vi,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBuffer)))
		return;

	// Setup Mesh
	m_axisMesh = system->resource->factory->CreateMeshNocopyUM(&viBuffer);
}

void GizmoRotation::SetupAxisLineStripMesh(uint edge)
{
	edge = Clamp(edge, 3u, UINT_MAX);

	// Setup Vertices
	uint numVertices = edge ;
	Vertex* vertices = new Vertex[numVertices]{};
	for (uint i = 0; i < edge; ++i)
	{
		float spinPercent = float(i) / float(edge - 1);
		float rad = spinPercent * TAU;
		vertices[i].position.x = Cos(rad);
		vertices[i].position.y = Sin(rad);
		vertices[i].position.z = 0.0f;
		vertices[i].uvw.x = vertices[i].position.x * 0.5f + 0.5f;
		vertices[i].uvw.y = vertices[i].position.y * 0.5f + 0.5f;
		vertices[i].normal = V3::back();
	}

	// Setup Indices
	uint numIndices = edge * 2;
	Index* indices = new Index[numIndices]{};
	for (uint i = 0; i < edge; ++i)
	{
		indices[i * 2] = Repeat(i, edge);
		indices[i * 2 + 1] = Repeat(i + 1, edge);
	}

	// Setup VI
	VI* vi = new VI;
	vi->SetVerticesNocopy(&vertices, numVertices);
	vi->SetIndicesNocopy(&indices, numIndices);
	vi->SetPrimitiveTopology(PrimitiveTopology::Lines);

	// Setup VIBuffer
	VIBuffer* viBuffer = nullptr;
	if (FAILED(VIBuffer::CreateVIBufferNocopy(
		system->graphic->device, system->graphic->deviceContext,
		&vi,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBuffer)))
		return;

	// Setup Mesh
	m_axisLineMesh = system->resource->factory->CreateMeshNocopyUM(&viBuffer);
}

void GizmoRotation::SetupSliceLineMesh()
{
	// Setup Vertices
	uint numVertices = 2;
	Vertex* vertices = new Vertex[numVertices]{};

	vertices[0].position.x = +0.0f;
	vertices[0].position.y = +0.0f;
	vertices[0].position.z = -1.0f;
	vertices[0].uvw.y = 1.0f;

	vertices[1].position.x = +0.0f;
	vertices[1].position.y = +0.0f;
	vertices[1].position.z = +1.0f;
	vertices[1].uvw.y = 0.0f;

	// Setup Indices
	uint numIndices = 2;
	Index* indices = new Index[numIndices]{};
	indices[0] = 0;
	indices[1] = 1;

	// Setup VI
	VI* vi = new VI;
	vi->SetVerticesNocopy(&vertices, numVertices);
	vi->SetIndicesNocopy(&indices, numIndices);
	vi->SetPrimitiveTopology(PrimitiveTopology::Lines);

	// Setup VIBuffer
	VIBuffer* viBuffer = nullptr;
	if (FAILED(VIBuffer::CreateVIBufferNocopy(
		system->graphic->device, system->graphic->deviceContext,
		&vi,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBuffer)))
		return;

	// Setup Mesh
	m_sliceLineMesh = system->resource->factory->CreateMeshNocopyUM(&viBuffer);
}

void GizmoRotation::SetupResources()
{
	SetupAxisMesh(32);
	SetupAxisLineStripMesh(32);
	SetupSliceLineMesh();

	ResourceRef<Shader> colorShader = system->resource->builtIn->colorShader;
	ResourceRef<Shader> wireframeShader = system->resource->builtIn->wireframeShader;

	m_rMat = system->resource->factory->CreateMaterialByShaderUM(colorShader);
	m_rMat->SetColor("_Color", Color(1.0f, 0.0f, 0.0f, 0.25f));
	m_rMat->techniqueIndex = 1;

	m_gMat = system->resource->factory->CreateMaterialByShaderUM(colorShader);
	m_gMat->SetColor("_Color", Color(0.0f, 1.0f, 0.0f, 0.25f));
	m_gMat->techniqueIndex = 1;

	m_bMat = system->resource->factory->CreateMaterialByShaderUM(colorShader);
	m_bMat->SetColor("_Color", Color(0.0f, 0.0f, 1.0f, 0.25f));
	m_bMat->techniqueIndex = 1;

	m_hrMat = system->resource->factory->CreateMaterialByShaderUM(colorShader);
	m_hrMat->SetColor("_Color", Color(1.0f, 0.0f, 0.0f, 0.5f));
	m_hrMat->techniqueIndex = 2;

	m_hgMat = system->resource->factory->CreateMaterialByShaderUM(colorShader);
	m_hgMat->SetColor("_Color", Color(0.0f, 1.0f, 0.0f, 0.5f));
	m_hgMat->techniqueIndex = 2;

	m_hbMat = system->resource->factory->CreateMaterialByShaderUM(colorShader);
	m_hbMat->SetColor("_Color", Color(0.0f, 0.0f, 1.0f, 0.5f));
	m_hbMat->techniqueIndex = 2;

	m_lrMat = system->resource->factory->CreateMaterialByShaderUM(wireframeShader);
	m_lrMat->SetColor("_Color", Color(1.0f, 0.0f, 0.0f, 1.0f));
	m_lrMat->techniqueIndex = 1;

	m_lgMat = system->resource->factory->CreateMaterialByShaderUM(wireframeShader);
	m_lgMat->SetColor("_Color", Color(0.0f, 1.0f, 0.0f, 1.0f));
	m_lgMat->techniqueIndex = 1;

	m_lbMat = system->resource->factory->CreateMaterialByShaderUM(wireframeShader);
	m_lbMat->SetColor("_Color", Color(0.0f, 0.0f, 1.0f, 1.0f));
	m_lbMat->techniqueIndex = 1;

	m_slMat = system->resource->factory->CreateMaterialByShaderUM(wireframeShader);
	m_slMat->SetColor("_Color", Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_slMat->techniqueIndex = 1;
}

void GizmoRotation::SetupObjects()
{
	GameObject* goX = CreateGameObjectToChild(transform);
	GameObject* goY = CreateGameObjectToChild(transform);
	GameObject* goZ = CreateGameObjectToChild(transform);

	goX->transform->localEulerAngles = V3(+00.0f, +90.0f, +00.0f); // forward: right
	goY->transform->localEulerAngles = V3(-90.0f, +00.0f, +00.0f); // forward: up
	goZ->transform->localEulerAngles = V3(+00.0f, +00.0f, +00.0f); // forward: forward

	m_xRenderer = goX->AddComponent<MeshRenderer>();
	m_yRenderer = goY->AddComponent<MeshRenderer>();
	m_zRenderer = goZ->AddComponent<MeshRenderer>();

	m_lxRenderer = goX->AddComponent<MeshRenderer>();
	m_lyRenderer = goY->AddComponent<MeshRenderer>();
	m_lzRenderer = goZ->AddComponent<MeshRenderer>();

	m_slxRenderer = goX->AddComponent<MeshRenderer>();
	m_slyRenderer = goY->AddComponent<MeshRenderer>();
	m_slzRenderer = goZ->AddComponent<MeshRenderer>();

	m_xRenderer->mesh = m_axisMesh;
	m_yRenderer->mesh = m_axisMesh;
	m_zRenderer->mesh = m_axisMesh;

	m_lxRenderer->mesh = m_axisLineMesh;
	m_lyRenderer->mesh = m_axisLineMesh;
	m_lzRenderer->mesh = m_axisLineMesh;

	m_slxRenderer->mesh = m_sliceLineMesh;
	m_slyRenderer->mesh = m_sliceLineMesh;
	m_slzRenderer->mesh = m_sliceLineMesh;

	m_xRenderer->material = m_rMat;
	m_yRenderer->material = m_gMat;
	m_zRenderer->material = m_bMat;

	m_lxRenderer->material = m_lrMat;
	m_lyRenderer->material = m_lgMat;
	m_lzRenderer->material = m_lbMat;

	m_slxRenderer->material = m_slMat;
	m_slyRenderer->material = m_slMat;
	m_slzRenderer->material = m_slMat;
}
