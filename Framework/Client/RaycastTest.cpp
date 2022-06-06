#include "stdafx.h"
#include "RaycastTest.h"

void RaycastTest::Awake()
{
	m_meshRenderer = gameObject->GetComponent<MeshRenderer>();
}

void RaycastTest::Update()
{
	if (!m_meshRenderer)
		return;

	M4 localToWorldMatrix = m_meshRenderer->transform->localToWorldMatrix;
	V3 hit;
	Ray ray = system->input->GetRayInWorldSpace(system->graphic->cameraManager->mainCamera);
	if (system->input->GetKeyDown(Key::LeftMouse))
	{
		if (m_meshRenderer->mesh->RaycastInWorldSpace(hit, ray, localToWorldMatrix, true))
		{
			cout << "hit" << endl;
		}
		else
		{
			cout << "no hit" << endl;
		}
	}
}
