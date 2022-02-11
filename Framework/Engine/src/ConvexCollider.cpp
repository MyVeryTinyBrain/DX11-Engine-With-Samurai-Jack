#include "EnginePCH.h"
#include "ConvexCollider.h"

PxGeometryHolder ConvexCollider::CreatePxGeometry(bool& out_invalid) const
{
	if (!m_mesh)
	{
		out_invalid = true;
		return PxGeometryHolder();
	}
	

	return PxGeometryHolder();
}

void ConvexCollider::Awake()
{
	Collider::Awake();

	if (CheckMeshRenderer())
		UpdatePxGeometry();
}

void ConvexCollider::Start()
{
	Collider::Start();

	if (CheckMeshRenderer())
		UpdatePxGeometry();
}

void ConvexCollider::Update()
{
	Collider::Update();

	if (CheckMeshRenderer())
		UpdatePxGeometry();
}

bool ConvexCollider::CheckMeshRenderer()
{
	return false;
}

void ConvexCollider::UpdatePxGeometry()
{
}
