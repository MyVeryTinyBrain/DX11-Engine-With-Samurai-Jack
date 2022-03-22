#include "EnginePCH.h"
#include "CameraManager.h"
#include "ICamera.h"

void CameraManager::AddCamera(ICamera* camera)
{
	if (!camera)
		return;

	if (Contains(camera))
		return;

	m_cameras.push_back(camera);

	if (!m_mainCamera)
		m_mainCamera = camera;

	SortByOrder();
}

ICamera* CameraManager::GetCamera(uint index) const
{
	if (index >= uint(m_cameras.size()))
		return nullptr;

	return m_cameras[index];
}

uint CameraManager::GetCameraCount() const
{
	return uint(m_cameras.size());
}

void CameraManager::SetMainCamera(ICamera* camera)
{
	if (!camera)
		return;

	if (!Contains(camera))
		AddCamera(camera);

	m_mainCamera = camera;
}

ICamera* CameraManager::GetMainCamera() const
{
	return m_mainCamera;
}

void CameraManager::RemoveCamera(ICamera* camera)
{
	auto find_it = find(m_cameras.begin(), m_cameras.end(), camera);

	if (find_it != m_cameras.end())
	{
		m_cameras.erase(find_it);

		if (m_mainCamera == camera)
		{
			m_mainCamera = nullptr;

			for (auto& camera : m_cameras)
			{
				if (camera->IsWorking())
				{
					SetMainCamera(camera);
					break;
				}
			}
		}

		SortByOrder();
	}
}

void CameraManager::SortByOrder()
{
	sort(m_cameras.begin(), m_cameras.end(),
		[&](ICamera* lhs, ICamera* rhs) {
			return lhs->GetOrder() < rhs->GetOrder();
		});
}

void CameraManager::OnResolutionChanged(float width, float height, float aspect)
{
	for (auto& camera : m_cameras)
		camera->OnResolutionChanged(width, height, aspect);
}

bool CameraManager::Contains(ICamera* camera)
{
	auto find_it = find(m_cameras.begin(), m_cameras.end(), camera);
	if (find_it != m_cameras.end())
		return true;
	return false;
}
