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

	// �� ��ü�� ���������͸� �����մϴ�.
	SafeDelete(m_refData);

	// =========================================================================================================

	// ������ ���ҽ�������Ʈ�Դϴ�.
	ResourceObject* destroyTargetResource = nullptr;

	// ������ ���۷����� ���
	// ���ҽ�������Ʈ�� �ı����� �ʾ��� ���
	if (m_basedResourceRefData && !m_basedResourceRefData->isNull && m_basedResourceRefData->referenceCount == 1)
	{
		destroyTargetResource = m_basedResourceRefData->GetResourceObject();
	}

	// ���۷��� �����͸� �����մϴ�.
	SafeDelete(m_basedResourceRefData);

	// �������� �ʴ� ���ҽ�������Ʈ�� ������ ������ ���� ��� nullptr�� �ƴϰ� �˴ϴ�.
	// destroyTargetResource�� nullptr�� �ƴ϶�� �����մϴ�.
	// nullptr�� �ƴ϶�� ���� �������� ���۷��� ī��Ʈ�� 0�� �Ǿ����Ƿ� 
	// ���۷��� �������� ��� �����Ҵ�� ������ ���� �����˴ϴ�.
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