#pragma once
#ifndef __RESOURCE_OBJECT_H__
#define __RESOURCE_OBJECT_H__

#include "ObjectBase.h"
#include "IResourceObject.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class System;
class ResourceManagement;
class ResourceRefData;
class ENGINE_API ResourceObject abstract : public ObjectBase, public IResourceObject
{
public:

	ResourceObject(ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName);

	virtual ~ResourceObject();

protected:

	// Create to unmanaged resource
	// And can change to managed resource

	ResourceObject();

	void SetManagement(ResourceManagement* management);

	void SetToManaged(ResourceManagement* management, const tstring& path, const tstring& groupName);

public:

	bool ToUnmanaged();

	const tstring& GetPath() const;

	tstring GetDirectory() const;

	const tstring& GetGroupName() const;

	bool IsUnmanaged() const;

	ResourceManagement* GetManagement() const;

	const System* GetSystem() const;

	_declspec(property(get = GetPath)) const tstring& path;
	_declspec(property(get = GetDirectory)) const tstring& directory;
	_declspec(property(get = GetGroupName)) const tstring& groupName;
	_declspec(property(get = IsUnmanaged)) bool isUnmanaged;
	_declspec(property(get = GetManagement)) ResourceManagement* management;
	_declspec(property(get = GetSystem)) System* system;

private:

	virtual void SetToDestroyed() final override;

	virtual const ResourceRefData* GetRefData() const final override;

private:

	ResourceManagement* m_management = nullptr;

	tstring m_path = TEXT("");

	tstring m_groupName = TEXT("");

	bool m_unmanaged = false;

	ResourceRefData* m_refData = nullptr;
};

ENGINE_END

#endif
