#pragma once

#include "EngineMacro.h"
#include "ResourceRefData.h"

ENGINE_BEGIN
class ResourceRefData;
class ResourceObject;
template <class T>
class ResourceRef final
{
	template <class U> friend class ResourceRef;

public:

	// Constructors ===========================================

	explicit ResourceRef();

	explicit ResourceRef(ResourceRefData* refData);

	ResourceRef(T* pointer);

	template <class U>
	ResourceRef(U* pointer);

	ResourceRef(const ResourceRef& other);

	template <class U>
	ResourceRef(const ResourceRef<U>& other);

	ResourceRef(ResourceRef&& other);

	template <class U>
	ResourceRef(ResourceRef<U>&& other);

	// Destructor =============================================

	~ResourceRef();

	// Operators ==============================================

	ResourceRef& operator = (const ResourceRef& other);

	template <class U>
	ResourceRef& operator = (const ResourceRef<U>& other);

	ResourceRef& operator = (ResourceRef&& other);

	template <class U>
	ResourceRef& operator = (ResourceRef<U>&& other);

	bool operator == (const ResourceRef& other);

	bool operator != (const ResourceRef& other);

	template <class U>
	bool operator == (const ResourceRef<U>& other);

	template <class U>
	bool operator != (const ResourceRef<U>& other);

	bool operator ! () const;

	operator T*() const;

	T* operator -> () const;

	// Member Variables =======================================

	ResourceObject* GetResourceObject() const;

	T* GetPointer() const;

	uint GetRefCount() const;

	// Member Functions =======================================

	void Reset();

	bool IsNull() const;

private:

	// Member Variables =======================================

	ResourceRefData* m_refData;
};
ENGINE_END

#include "ResourceRef.hpp"
