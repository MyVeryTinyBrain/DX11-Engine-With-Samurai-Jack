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

	ResourceObject(ResourceManagement* management, bool managed, const tstring& path);

	virtual ~ResourceObject();

protected:

	bool ToManaged(const tstring& newPath);
	bool ToManaged();
	bool ToUnmanaged();

public:

	inline const tstring& GetPath() const { return m_path; }
	tstring GetDirectory() const;
	inline void SetGroupName(const tstring& groupName) { m_groupName = groupName; }
	inline const tstring& GetGroupName() const { return m_groupName; }

	inline bool IsUnmanaged() const { return m_unmanaged; }
	inline bool IsManaged() const { return !m_unmanaged; }
	inline ResourceManagement* GetManagement() const { return m_management; }
	const System* GetSystem() const;

	_declspec(property(get = GetPath)) const tstring& path;
	_declspec(property(get = GetDirectory)) const tstring& directory;
	_declspec(property(get = GetGroupName, put = SetGroupName)) const tstring& groupName;
	_declspec(property(get = IsUnmanaged)) bool isUnmanaged;
	_declspec(property(get = IsManaged)) bool isManaged;
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
