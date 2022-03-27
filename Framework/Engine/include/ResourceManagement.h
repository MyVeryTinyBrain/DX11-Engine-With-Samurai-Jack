#pragma once

#include "IResourceManagement.h"
#include "ResourceRef.h"
#include "ResourceFactory.h"
#include "BuiltInResources.h"

ENGINE_BEGIN

class ENGINE_API ResourceManagement : public IResourceManagement
{
public:

	ResourceManagement();

	virtual ~ResourceManagement();

public:

	bool Exist(const tstring& path) const;

	ResourceRef<ResourceObject> Find(const tstring& path) const;

public:

	System* GetSystem() const;

	ResourceFactory* GetFactory() const;

	BuiltInResources* GetBuiltInResource() const;

	_declspec(property(get = GetSystem)) System* system;
	_declspec(property(get = GetFactory)) ResourceFactory* factory;
	_declspec(property(get = GetBuiltInResource)) BuiltInResources* builtInResources;

private:

	virtual bool Initialize(System* system) final override;

	virtual bool AddManagedResource(ResourceRef<ResourceObject> resource) final override;

	virtual bool RemoveManagedResource(const tstring& path) final override;

	virtual ResourceRef<ResourceObject> FindManagedResource(const tstring& path) const final override;

	virtual void RemoveAllManagedResource() final override;

	virtual unsigned int RemoveGroupedManagedResource(const tstring& groupName) final override;

private:

	using ManagedResources = unordered_map<tstring, ResourceRef<ResourceObject>>;

	ManagedResources			m_managedResources;

	System*						m_system = nullptr;

	ResourceFactory*			m_resourceFactory = nullptr;

	BuiltInResources*			m_builtInResource = nullptr;
};

ENGINE_END
