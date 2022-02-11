#include "EnginePCH.h"
#include "ResourceObject.h"
#include "ResourceRefData.h"
#include "ResourceManagement.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

ResourceObject::ResourceObject(ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName) :
	ObjectBase(fs::path(path).filename())
{
	tstring lowerPath = path;
	std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

	m_management = management;
	m_unmanaged = !managed;
	m_path = lowerPath;
	m_groupName = groupName;
	m_refData = new ResourceRefData(this);

	if (managed)
	{
		IResourceManagement* iManagement = management;
		bool ResourceNotExist = iManagement->AddManagedResource(this);

		// 이미 같은 경로를 가지는 리소스가 존재합니다.
		assert(ResourceNotExist);
	}
}

ResourceObject::~ResourceObject()
{
	SetToDestroyed();

	SafeDelete(m_refData);
}

ResourceObject::ResourceObject() :
	ObjectBase()
{
	m_unmanaged = true;
	m_refData = new ResourceRefData(this);
}

void ResourceObject::SetManagement(ResourceManagement* management)
{
	m_management = management;
}

void ResourceObject::SetToManaged(ResourceManagement* management, const tstring& path, const tstring& groupName)
{
	tstring lowerPath = path;
	std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

	m_management = management;
	m_unmanaged = false;
	m_path = lowerPath;
	m_groupName = groupName;

	name = fs::path(path).filename();

	IResourceManagement* iManagement = management;
	bool ResourceNotExist = iManagement->AddManagedResource(this);

	// 이미 같은 경로를 가지는 리소스가 존재합니다.
	assert(ResourceNotExist);
}

bool ResourceObject::ToUnmanaged()
{
	if (m_unmanaged)
		return false;

	IResourceManagement* iManagement = m_management;
	bool succeeded = iManagement->RemoveManagedResource(m_path);

	if (succeeded)
	{
		m_unmanaged = true;
		return true;
	}

	return false;
}

const tstring & ResourceObject::GetPath() const
{
	return m_path;
}

tstring ResourceObject::GetDirectory() const
{
	fs::path path = m_path;
	tstring tstrPath = path.parent_path();
	std::replace(tstrPath.begin(), tstrPath.end(), TEXT('\\'), TEXT('/'));
	return tstrPath;
}

const tstring & ResourceObject::GetGroupName() const
{
	return m_groupName;
}

bool ResourceObject::IsUnmanaged() const
{
	return m_unmanaged;
}

ResourceManagement* ResourceObject::GetManagement() const
{
	return m_management;
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
