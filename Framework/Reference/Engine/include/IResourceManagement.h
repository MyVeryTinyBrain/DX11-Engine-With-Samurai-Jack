#pragma once

#include "ResourceRef.h"

ENGINE_BEGIN
class ResourceObject;
class System;
class ENGINE_API IResourceManagement abstract
{
public:

	virtual ~IResourceManagement() = default;

	virtual bool Initialize(System* system) = 0;

	virtual bool AddManagedResource(ResourceRef<ResourceObject> resource) = 0;

	virtual bool RemoveManagedResource(const tstring& path) = 0;

	virtual ResourceRef<ResourceObject> FindManagedResource(const tstring& path) const = 0;

	virtual void RemoveAllManagedResource() = 0;

	virtual unsigned int RemoveGroupedManagedResource(const tstring& groupName) = 0;

};

ENGINE_END
