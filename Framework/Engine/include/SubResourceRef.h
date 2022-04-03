#pragma once

#include "EngineMacro.h"
#include "RefData.h"
#include "ResourceRefData.h"

ENGINE_BEGIN
class RefData;
class ResourceRefData;
class SubResourceObject;
template <class T>
class SubResourceRef final
{
	template <class U> friend class SubResourceRef;

public:

	// Constructors ===========================================

	explicit SubResourceRef();

	SubResourceRef(T* pointer);

	template <class U>
	SubResourceRef(U* pointer);

	SubResourceRef(const SubResourceRef& other);

	template <class U>
	SubResourceRef(const SubResourceRef<U>& other);

	SubResourceRef(SubResourceRef&& other);

	template <class U>
	SubResourceRef(SubResourceRef<U>&& other);

	// Destructor =============================================

	~SubResourceRef();

	// Operators ==============================================

	SubResourceRef& operator = (const SubResourceRef& other);

	template <class U>
	SubResourceRef& operator = (const SubResourceRef<U>& other);

	SubResourceRef& operator = (SubResourceRef&& other);

	template <class U>
	SubResourceRef& operator = (SubResourceRef<U>&& other);

	bool operator == (const SubResourceRef& other);

	bool operator != (const SubResourceRef& other);

	bool operator == (const void* ptr);

	bool operator != (const void* ptr);

	template <class U>
	bool operator == (const SubResourceRef<U>& other);

	template <class U>
	bool operator != (const SubResourceRef<U>& other);

	bool operator ! () const;

	operator T* () const;

	T* operator -> () const;

	// Member Variables =======================================

	SubResourceObject* GetSubResourceObject() const;

	T* GetPointer() const;

	uint GetRefCount() const;

	// Member Functions =======================================

	void Reset();

	bool IsNull() const;

private:

	// Member Variables =======================================

	RefData* m_refData;

	ResourceRefData* m_basedResourceRefData;
};
ENGINE_END

#include "SubResourceRef.hpp"
