#include "EnginePCH.h"
#include "ResourceObject.h"
#include "ResourceRefData.h"
#include "ResourceManagement.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

ResourceObject::ResourceObject(ResourceManagement* management, bool managed, const tstring& path) :
	ObjectBase(fs::path(path).filename())
{
	tstring lowerPath = path;
	std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

	m_management = management;
	m_unmanaged = !managed;
	m_path = lowerPath;
	m_refData = new ResourceRefData(this);

	if (managed)
	{
		IResourceManagement* iManagement = management;
		bool ResourceNotExist = iManagement->AddManagedResource(this);

		assert(ResourceNotExist); // 이미 같은 경로를 가지는 리소스가 존재합니다.
	}
}

ResourceObject::~ResourceObject()
{
	SetToDestroyed();

	SafeDelete(m_refData);
}

bool ResourceObject::ToManaged(const tstring& newPath)
{
	if (m_management)
		return false;

	tstring lowerPath = newPath;
	std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);
	m_path = lowerPath;

	IResourceManagement* iManagement = management;
	if (iManagement->AddManagedResource(this))
	{
		m_unmanaged = false;
		return true;
	}
	return false;
}

bool ResourceObject::ToManaged()
{
	if (m_management)
		return false;

	IResourceManagement* iManagement = management;
	if (iManagement->AddManagedResource(this))
	{
		m_unmanaged = false;
		return true;
	}

	return false;
}

bool ResourceObject::ToUnmanaged()
{
	if (m_unmanaged)
		return false;

	IResourceManagement* iManagement = m_management;
	if (iManagement->RemoveManagedResource(m_path))
	{
		m_unmanaged = true;
		return true;
	}

	return false;
}

tstring ResourceObject::GetDirectory() const
{
	fs::path path = m_path;
	tstring tstrPath = path.parent_path();
	std::replace(tstrPath.begin(), tstrPath.end(), TEXT('\\'), TEXT('/'));
	return tstrPath;
}

const System* ResourceObject::GetSystem() const
{
	return m_management->GetSystem();
}

void ResourceObject::SetToDestroyed()
{
	m_refData->SetToDestroyed();
}

const ResourceRefData* ResourceObject::GetRefData() const
{
	return m_refData;
}
