#include "EnginePCH.h"
#include "ResourceManagement.h"
#include "ResourceObject.h"

ResourceManagement::ResourceManagement()
{
}

ResourceManagement::~ResourceManagement()
{
	SafeDelete(m_builtInResource);
	SafeDelete(m_resourceFactory);
	RemoveAllManagedResource();
}
bool ResourceManagement::Initialize(System* system)
{
	if (!system)
		return false;

	m_system = system;

	m_resourceFactory = new ResourceFactory();
	IResourceFactory* iResourceFactory = m_resourceFactory;
	if (FAILED(iResourceFactory->Initialize(this)))
		return false;

	m_builtInResource = new BuiltInResources();
	IBuiltInResources* iBuiltInResources = m_builtInResource;
	if (FAILED(iBuiltInResources->Initialize(m_resourceFactory)))
		return false;

	return true;
}

bool ResourceManagement::AddManagedResource(ResourceRef<ResourceObject> resource)
{
	if (!resource)
		return false;

	auto resultPair = m_managedResources.emplace(resource->path, resource);
	if (!resultPair.second)
		return false;

	return true;
}

bool ResourceManagement::RemoveManagedResource(const tstring& path)
{
	tstring lowerPath = path;
	std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), std::tolower);

	auto it = m_managedResources.find(lowerPath);

	if (it == m_managedResources.end())
		return false;

	m_managedResources.erase(it);

	return true;
}

ResourceRef<ResourceObject> ResourceManagement::FindManagedResource(const tstring& path) const
{
	tstring lowerPath = path;
	std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), std::tolower);

	auto it = m_managedResources.find(lowerPath);

	if (it == m_managedResources.end())
		return nullptr;

	return it->second;
}

void ResourceManagement::RemoveAllManagedResource()
{
	m_managedResources.clear();
}

unsigned int ResourceManagement::RemoveGroupedManagedResource(const tstring& groupName)
{
	unsigned int deleteCount = 0;

	for (auto it = m_managedResources.begin(); it != m_managedResources.end();)
	{
		ResourceRef<ResourceObject> resource = it->second;
		if (resource->groupName == groupName)
		{
			it = m_managedResources.erase(it);
			++deleteCount;
		}
		else
		{
			++it;
		}
	}

	return deleteCount;
}


bool ResourceManagement::Exist(const tstring& path) const
{
	ResourceObject* resource = FindManagedResource(path);

	return resource != nullptr;
}

ResourceRef<ResourceObject> ResourceManagement::Find(const tstring& path) const
{
	ResourceObject* resource = FindManagedResource(path);

	return ResourceRef<ResourceObject>(resource);
}

System* ResourceManagement::GetSystem() const
{
	return m_system;
}

ResourceFactory* ResourceManagement::GetFactory() const
{
	return m_resourceFactory;
}

BuiltInResources* ResourceManagement::GetBuiltInResource() const
{
	return m_builtInResource;
}
