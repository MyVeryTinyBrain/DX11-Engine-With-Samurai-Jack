#pragma once

#include "SubResourceRef.h"
#include "SubResourceObject.h"
#include "ResourceObject.h"

ENGINE_BEGIN

template<class T>
inline SubResourceRef<T>::SubResourceRef()
{
	m_refData = nullptr;
	m_basedResourceRefData = nullptr;
}

template<class T>
inline SubResourceRef<T>::SubResourceRef(T* pointer)
{
	if (!pointer)
		return;

	SubResourceObject* subResourceObject = static_cast<SubResourceObject*>(pointer);
	IObject* iObject = static_cast<IObject*>(subResourceObject);

	const RefData* pRefData = iObject->GetRefData();

	if (!pRefData)
		return;

	ISubResourceObject* iSubResourceObject = static_cast<ISubResourceObject*>(subResourceObject);
	ResourceObject* resourceObject = iSubResourceObject->GetBasedResourceObjectPointer();
	IResourceObject* iResourceObject = static_cast<IResourceObject*>(resourceObject);

	if (!iResourceObject)
		return;

	const ResourceRefData* pResourceRefData = iResourceObject->GetRefData();

	if (!pResourceRefData)
		return;

	m_refData = new RefData(*pRefData);

	m_basedResourceRefData = new ResourceRefData(*pResourceRefData);
}

template<class T>
template<class U>
inline SubResourceRef<T>::SubResourceRef(U* pointer)
{
	static_cast<T*>((U*)0);

	if (!pointer)
		return;

	if (!dynamic_cast<T*>(pointer))
		return;

	SubResourceObject* subResourceObject = static_cast<SubResourceObject*>(pointer);
	IObject* iObject = static_cast<IObject*>(subResourceObject);

	const RefData* pRefData = iObject->GetRefData();

	if (!pRefData)
		return;

	ISubResourceObject* iSubResourceObject = static_cast<ISubResourceObject*>(subResourceObject);
	ResourceObject* resourceObject = iSubResourceObject->GetBasedResourceObjectPointer();
	IResourceObject* iResourceObject = static_cast<IResourceObject*>(resourceObject);

	if (!iResourceObject)
		return;

	const ResourceRefData* pResourceRefData = iResourceObject->GetRefData();

	if (!pResourceRefData)
		return;

	m_refData = new RefData(*pRefData);

	m_basedResourceRefData = new ResourceRefData(*pResourceRefData);
}

template<class T>
inline SubResourceRef<T>::SubResourceRef(const SubResourceRef& other)
{
	if (!other.m_refData || !other.m_basedResourceRefData)
		return;

	m_refData = new RefData(*other.m_refData);
	m_basedResourceRefData = new ResourceRefData(*other.m_basedResourceRefData);
}

template<class T>
template<class U>
inline SubResourceRef<T>::SubResourceRef(const SubResourceRef<U>& other)
{
	static_cast<T*>((U*)0);

	if (!other.m_refData || !other.m_basedResourceRefData)
		return;

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;
		m_basedResourceRefData = nullptr;
	}
	else
	{
		m_refData = new RefData(*other.m_refData);
		m_basedResourceRefData = new ResourceRefData(*other.m_basedResourceRefData);
	}
}

template<class T>
inline SubResourceRef<T>::SubResourceRef(SubResourceRef&& other)
{
	m_refData = other.m_refData;
	m_basedResourceRefData = other.m_basedResourceRefData;

	other.m_refData = nullptr;
	m_basedResourceRefData = nullptr;
}

template<class T>
template<class U>
inline SubResourceRef<T>::SubResourceRef(SubResourceRef<U>&& other)
{
	static_cast<T*>((U*)0);

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;
		m_basedResourceRefData = nullptr;

		return;
	}
	else
	{
		m_refData = other.m_refData;
		m_basedResourceRefData = other.m_basedResourceRefData;

		other.m_refData = nullptr;
		m_basedResourceRefData = nullptr;
	}
}


template<class T>
inline SubResourceRef<T>::~SubResourceRef()
{
	Reset();
}

template<class T>
inline SubResourceRef<T>& SubResourceRef<T>::operator=(const SubResourceRef& other)
{
	Reset();

	if (!other.m_refData || !other.m_basedResourceRefData)
		return *this;

	m_refData = new RefData(*other.m_refData);
	m_basedResourceRefData = new ResourceRefData(*other.m_basedResourceRefData);

	return *this;
}

template<class T>
template<class U>
inline SubResourceRef<T>& SubResourceRef<T>::operator=(const SubResourceRef<U>& other)
{
	static_cast<T*>((U*)0);

	Reset();

	if (!other.m_refData || !other.m_basedResourceRefData)
		return *this;

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;
	}
	else
	{
		m_refData = new RefData(*other.m_refData);
		m_basedResourceRefData = new ResourceRefData(*other.m_basedResourceRefData);
	}

	return *this;
}

template<class T>
inline SubResourceRef<T>& SubResourceRef<T>::operator=(SubResourceRef&& other)
{
	if ((void*)this == (void*)&other)
		return *this;

	Reset();

	m_refData = other.m_refData;
	m_basedResourceRefData = other.m_basedResourceRefData;

	other.m_refData = nullptr;
	other.m_basedResourceRefData = nullptr;

	return *this;
}

template<class T>
template<class U>
inline SubResourceRef<T>& SubResourceRef<T>::operator=(SubResourceRef<U>&& other)
{
	static_cast<T*>((U*)0);

	if ((void*)this == (void*)&other)
		return *this;

	Reset();

	U* otherPointer = other.GetPointer();
	T* cast = dynamic_cast<T*>(otherPointer);

	if (!cast)
	{
		m_refData = nullptr;
		m_basedResourceRefData = nullptr;
	}
	else
	{
		m_refData = other.m_refData;
		m_basedResourceRefData = other.m_basedResourceRefData;
	}

	other.m_refData = nullptr;
	other.m_basedResourceRefData = nullptr;

	return *this;
}

template<class T>
inline bool SubResourceRef<T>::operator==(const SubResourceRef& other)
{
	return GetSubResourceObject() == other.GetSubResourceObject();
}

template<class T>
inline bool SubResourceRef<T>::operator!=(const SubResourceRef& other)
{
	return GetSubResourceObject() != other.GetSubResourceObject();
}

template<class T>
template<class U>
inline bool SubResourceRef<T>::operator==(const SubResourceRef<U>& other)
{
	static_cast<T*>((U*)0);

	return GetSubResourceObject() == other.GetSubResourceObject();
}

template<class T>
template<class U>
inline bool SubResourceRef<T>::operator!=(const SubResourceRef<U>& other)
{
	static_cast<T*>((U*)0);

	return GetSubResourceObject() != other.GetSubResourceObject();
}

template<class T>
inline bool SubResourceRef<T>::operator!() const
{
	return IsNull();
}

template<class T>
inline SubResourceRef<T>::operator T* () const
{
	return GetPointer();
}

template<class T>
inline T* SubResourceRef<T>::operator->() const
{
	return GetPointer();
}

template<class T>
inline SubResourceObject* SubResourceRef<T>::GetSubResourceObject() const
{
	if (!m_refData || !m_basedResourceRefData)
		return nullptr;

	return m_refData->GetObject();
}

template<class T>
inline T* SubResourceRef<T>::GetPointer() const
{
	if (!m_refData || !m_basedResourceRefData)
		return nullptr;

	return static_cast<T*>(m_refData->GetObject());
}

template<class T>
inline uint SubResourceRef<T>::GetRefCount() const
{
	if (!m_refData || !m_basedResourceRefData)
		return nullptr;

	return m_basedResourceRefData->GetRefCount();
}

template<class T>
inline void SubResourceRef<T>::Reset()
{
	// =========================================================================================================

	// 이 객체의 참조데이터를 삭제합니다.
	SafeDelete(m_refData);

	// =========================================================================================================

	// 삭제할 리소스오브젝트입니다.
	ResourceObject* destroyTargetResource = nullptr;

	// 마지막 레퍼런스인 경우
	// 리소스오브젝트가 파괴되지 않았을 경우
	if (m_basedResourceRefData && !m_basedResourceRefData->isNull && m_basedResourceRefData->referenceCount == 1)
	{
		destroyTargetResource = m_basedResourceRefData->GetResourceObject();
	}

	// 레퍼런스 데이터를 삭제합니다.
	SafeDelete(m_basedResourceRefData);

	// 관리되지 않는 리소스오브젝트가 마지막 참조를 잃은 경우 nullptr이 아니게 됩니다.
	// destroyTargetResource이 nullptr이 아니라면 삭제합니다.
	// nullptr이 아니라면 위의 삭제에서 레퍼런스 카운트가 0이 되었으므로 
	// 레퍼런스 데이터의 모든 동적할당된 변수들 또한 삭제됩니다.
	SafeDelete(destroyTargetResource);

	// =========================================================================================================
}

template<class T>
inline bool SubResourceRef<T>::IsNull() const
{
	if (!m_refData)
		return true;

	return m_refData->IsNull();
}

ENGINE_END