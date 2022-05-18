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
#include "Texture2D.h"

GizmoBase::Axis GizmoRotation::PickTest() const
{
	Ray ray = system->input->GetRayInWorldSpace();
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

	Ray ray = system->input->GetRayInWorldSpace();
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

		// 선택한 당시의 원의 법선을 저장합니다.
		// 이 법선으로 투영을 위한 평면을 생성합니다.
		// 매번 원의 법선을 사용한다면 오차가 누적됩니다.
		// 또한 회전하는 원의 법선을 기준으로는 회전되지 않기 때문에 이 방법을 사용합니다.
		// MouseOnVirtualPlane 의 함수의 인자로 법선을 받는 이유가 이것 때문입니다.
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
	vertices[0].position.z = -0.5f;
	vertices[0].uvw.y = 1.0f;

	vertices[1].position.x = +0.0f;
	vertices[1].position.y = +0.0f;
	vertices[1].position.z = +0.5f;
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
	//if (!m_rTexture)
	//{
	//	m_rTexture = system->resource->factory->CreateUnmanagedTexture2D(Color(1, 0, 0, 0.5f), 16, 16);
	//}
	//if (!m_gTexture)
	//{
	//	m_gTexture = system->resource->factory->CreateUnmanagedTexture2D(Color(0, 1, 0, 0.5f), 16, 16);
	//}
	//if (!m_bTexture)
	//{
	//	m_bTexture = system->resource->factory->CreateUnmanagedTexture2D(Color(0, 0, 1, 0.5f), 16, 16);
	//}
	//if (!m_lrTexture)
	//{
	//	m_lrTexture = system->resource->factory->CreateUnmanagedTexture2D(Color::red(), 16, 16);
	//}
	//if (!m_lgTexture)
	//{
	//	m_lgTexture = system->resource->factory->CreateUnmanagedTexture2D(Color::green(), 16, 16);
	//}
	//if (!m_lbTexture)
	//{
	//	m_lbTexture = system->resource->factory->CreateUnmanagedTexture2D(Color::blue(), 16, 16);
	//}
	//if (!m_rMat)
	//{
	//	m_rMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationNoHighlight>();
	//	m_rMat->diffuseTexture = system->resourceManagement->factory->CreateUnmanagedTexture2D(Color(1, 1, 1, 0.25f), 16, 16);
	//}
	//if (!m_gMat)
	//{
	//	m_gMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationNoHighlight>();
	//	m_gMat->diffuseTexture = system->resourceManagement->factory->CreateUnmanagedTexture2D(Color(1, 1, 1, 0.25f), 16, 16);
	//}
	//if (!m_bMat)
	//{
	//	m_bMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationNoHighlight>();
	//	m_bMat->diffuseTexture = system->resourceManagement->factory->CreateUnmanagedTexture2D(Color(1, 1, 1, 0.25f), 16, 16);
	//}
	//if (!m_hrMat)
	//{
	//	m_hrMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationHighlight>();
	//	m_hrMat->diffuseTexture = m_rTexture;
	//}
	//if (!m_hgMat)
	//{
	//	m_hgMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationHighlight>();
	//	m_hgMat->diffuseTexture = m_gTexture;
	//}
	//if (!m_hbMat)
	//{
	//	m_hbMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationHighlight>();
	//	m_hbMat->diffuseTexture = m_bTexture;
	//}
	//if (!m_lrMat)
	//{
	//	m_lrMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationLine>();
	//	m_lrMat->diffuseTexture = m_lrTexture;
	//}
	//if (!m_lgMat)
	//{
	//	m_lgMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationLine>();
	//	m_lgMat->diffuseTexture = m_lgTexture;
	//}
	//if (!m_lbMat)
	//{
	//	m_lbMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationLine>();
	//	m_lbMat->diffuseTexture = m_lbTexture;
	//}
	//if (!m_slMat)
	//{
	//	m_slMat = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialGizmoRotationLine>();
	//	m_slMat->diffuseTexture = system->resourceManagement->builtIn->whiteTexture;
	//}
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
