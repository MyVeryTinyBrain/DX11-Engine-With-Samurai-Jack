#pragma once

#include "ResourceRef.h"
#include "ResourceObject.h"

ENGINE_BEGIN

template<class T>
inline ResourceRef<T>::ResourceRef()
{
	m_refData = nullptr;
}

template<class T>
inline ResourceRef<T>::ResourceRef(ResourceRefData* refData) : ResourceRef()
{
	if (!refData)
		return;

	m_refData = new ResourceRefData(*refData);
}

template<class T>
inline ResourceRef<T>::ResourceRef(T* pointer) : ResourceRef()
{
	if (!pointer)
		return;

	ResourceObject* object = static_cast<ResourceObject*>(pointer);
	IResourceObject* iObject = static_cast<IResourceObject*>(object);
	
	const ResourceRefData* pRefData = iObject->GetRefData();

	if (!pRefData)
		return;

	m_refData = new ResourceRefData(*pRefData);
}

template<class T>
template<class U>
inline ResourceRef<T>::ResourceRef(U* pointer)
{
	static_cast<T*>((U*)0);

	if (!pointer)
		return;

	ResourceObject* object = static_cast<ResourceObject*>(pointer);

	if (!dynamic_cast<T*>(pointer))
		return;

	IResourceObject* iObject = static_cast<IResourceObject*>(object);
	
	const ResourceRefData* pRefData = iObject->GetRefData();

	if (!pRefData)
		return;

	m_refData = new ResourceRefData(*pRefData);
}

template<class T>
inline ResourceRef<T>::ResourceRef(const ResourceRef& other) : ResourceRef()
{
	if (!other.m_refData)
		return;

	m_refData = new ResourceRefData(*other.m_refData);
}

template<class T>
template<class U>
inline ResourceRef<T>::ResourceRef(const ResourceRef<U>& other) : ResourceRef()
{
	static_cast<T*>((U*)0);

	if (!other.m_refData)
		return;

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;
	}
	else
	{
		m_refData = new ResourceRefData(*other.m_refData);
	}
}

template<class T>
inline ResourceRef<T>::ResourceRef(ResourceRef&& other) noexcept
{
	m_refData = other.m_refData;

	other.m_refData = nullptr;
}

template<class T>
template<class U>
inline ResourceRef<T>::ResourceRef(ResourceRef<U>&& other) noexcept
{
	static_cast<T*>((U*)0);

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;

		return;
	}
	else
	{
		m_refData = other.m_refData;

		other.m_refData = nullptr;
	}
}

template<class T>
inline ResourceRef<T>::~ResourceRef()
{
	Reset();
}

template<class T>
inline ResourceRef<T>& ResourceRef<T>::operator=(const ResourceRef& other)
{
	Reset();

	if (!other.m_refData)
		return *this;

	m_refData = new ResourceRefData(*other.m_refData);

	return *this;
}

template<class T>
template<class U>
inline ResourceRef<T>& ResourceRef<T>::operator=(const ResourceRef<U>& other)
{
	static_cast<T*>((U*)0);

	Reset();

	if (!other.m_refData)
		return *this;

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;
	}
	else
	{
		m_refData = new ResourceRefData(*other.m_refData);
	}

	return *this;
}

template <class T>
inline ResourceRef<T>& ResourceRef<T>::operator = (ResourceRef&& other) noexcept
{
	if ((void*)this == (void*)&other)
		return *this;

	Reset();

	m_refData = other.m_refData;

	other.m_refData = nullptr;

	return *this;
}

template <class T>
template <class U>
inline ResourceRef<T>& ResourceRef<T>::operator = (ResourceRef<U>&& other) noexcept
{
	static_cast<T*>((U*)0);

	if ((void*)this == (void*)&other)
		return *this;

	Reset();

	if (!other.m_refData)
		return *this;

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;
	}
	else
	{
		m_refData = other.m_refData;
	}

	other.m_refData = nullptr;

	return *this;
}

template<class T>
inline bool ResourceRef<T>::operator==(const ResourceRef& other)
{
	return GetResourceObject() == other.GetResourceObject();
}

template<class T> 
inline bool ResourceRef<T>::operator!=(const ResourceRef& other)
{
	return GetResourceObject() != other.GetResourceObject();
}

template<class T>
inline bool ResourceRef<T>::operator==(const void* ptr)
{
	return GetResourceObject() == ptr;
}

template<class T>
inline bool ResourceRef<T>::operator!=(const void* ptr)
{
	return GetResourceObject() != ptr;
}

template<class T>
template<class U>
inline bool ResourceRef<T>::operator==(const ResourceRef<U>& other)
{
	static_cast<T*>((U*)0);

	return GetResourceObject() == other.GetResourceObject();
}

template<class T>
template<class U>
inline bool ResourceRef<T>::operator!=(const ResourceRef<U>& other)
{
	static_cast<T*>((U*)0);

	return GetResourceObject() != other.GetResourceObject();
}

template<class T>
inline bool ResourceRef<T>::operator!() const
{
	return IsNull();
}

template<class T>
inline ResourceRef<T>::operator T* () const
{
	return GetPointer();
}

template<class T>
inline T* ResourceRef<T>::operator->() const
{
	return GetPointer();
}

template<class T>
inline ResourceObject* ResourceRef<T>::GetResourceObject() const
{
	if (!m_refData)
		return nullptr;

	return m_refData->GetResourceObject();
}

template<class T>
inline T* ResourceRef<T>::GetPointer() const
{
	if (!m_refData)
		return nullptr;

	return static_cast<T*>(m_refData->GetResourceObject());
}

template<class T>
inline uint ResourceRef<T>::GetRefCount() const
{
	if (!m_refData)
		return 0;

	return m_refData->GetRefCount();
}

template<class T>
inline void ResourceRef<T>::Reset()
{
	// 삭제할 대상입니다. 초기에는 null 입니다.
	ResourceObject* destroyTargetResource = nullptr;

	// 이번에 참조 카운터가 0이 되는 경우 그리고 오브젝트가 파괴되지 않았을 경우,
	// 삭제할 오브젝트 선택합니다.
	if (m_refData && !m_refData->isNull && m_refData->referenceCount == 1)
	{
		destroyTargetResource = m_refData->GetResourceObject();
	}

	// 레퍼런스 데이터를 삭제합니다.
	SafeDelete(m_refData);
	// 오브젝트를 삭제합니다.
	SafeDelete(destroyTargetResource);
}

template<class T>
inline bool ResourceRef<T>::IsNull() const
{
	if (!m_refData)
		return true;

	return m_refData->IsNull();
}

ENGINE_END
