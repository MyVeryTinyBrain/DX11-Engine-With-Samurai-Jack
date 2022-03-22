#pragma once

#include "Ref.h"
#include "Object.h"

ENGINE_BEGIN

template<class T>
inline Ref<T>::Ref()
{
	m_refData = nullptr;
}

template<class T>
inline Ref<T>::Ref(RefData* refData) : Ref()
{
	if (!refData) 
		return;

	m_refData = new RefData(*refData);
}

template<class T>
inline Ref<T>::Ref(T* pointer) : Ref()
{
	if (!pointer)
		return;

	Object* object = static_cast<Object*>(pointer);
	IObject* iObject = static_cast<IObject*>(object);

	const RefData* pRefData = iObject->GetRefData();

	if (!pRefData)
		return;

	m_refData = new RefData(*pRefData);
}

template<class T>
template<class U>
inline Ref<T>::Ref(U * pointer)
{
	static_cast<T*>((U*)0);

	if (!pointer)
		return;

	Object* object = static_cast<Object*>(pointer);

	if (!dynamic_cast<T*>(pointer))
		return;

	IObject* iObject = static_cast<IObject*>(object);
	const RefData* pRefData = iObject->GetRefData();

	if (!pRefData)
		return;

	m_refData = new RefData(*pRefData);
}

template<class T>
inline Ref<T>::Ref(const Ref& other) : Ref()
{
	if (!other.m_refData) 
		return;

	m_refData = new RefData(*other.m_refData);
}

template<class T>
template<class U>
inline Ref<T>::Ref(const Ref<U>& other) : Ref()
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
		m_refData = new RefData(*other.m_refData);
	}
}

template<class T>
inline Ref<T>::Ref(Ref&& other)
{
	m_refData = other.m_refData;

	other.m_refData = nullptr;
}

template<class T>
template<class U>
inline Ref<T>::Ref(Ref<U>&& other)
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
inline Ref<T>::~Ref()
{
	SafeDelete(m_refData);
}

template<class T>
inline Ref<T>& Ref<T>::operator=(const Ref& other)
{
	Reset();

	if (!other.m_refData) 
		return *this;

	m_refData = new RefData(*other.m_refData);

	return *this;
}

template<class T>
template<class U>
inline Ref<T>& Ref<T>::operator=(const Ref<U>& other)
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
		m_refData = new RefData(*other.m_refData);
	}

	return *this;
}

template <class T>
inline Ref<T>& Ref<T>::operator = (Ref&& other)
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
inline Ref<T>& Ref<T>::operator = (Ref<U>&& other)
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
inline bool Ref<T>::operator==(const Ref& other)
{
	return this->GetObject() == other.GetObject();
}

template<class T>
inline bool Ref<T>::operator!=(const Ref& other)
{
	return this->GetObject() != other.GetObject();
}

template<class T> 
template<class U>
inline bool Ref<T>::operator==(const Ref<U>& other)
{
	static_cast<T*>((U*)0);

	return this->GetObject() == other.GetObject();
}

template<class T>
template<class U>
inline bool Ref<T>::operator!=(const Ref<U>& other)
{
	static_cast<T*>((U*)0);

	return this->GetObject() != other.GetObject();
}

template<class T>
inline bool Ref<T>::operator!() const
{
	return IsNull();
}

template<class T>
inline Ref<T>::operator T* () const
{
	return GetPointer(); 
}

template<class T>
inline T* Ref<T>::operator->() const
{
	return GetPointer();
}

template<class T>
inline Object* Ref<T>::GetObject() const
{
	if (!m_refData)
		return nullptr;

	return m_refData->GetObject();
}

template<class T>
inline T* Ref<T>::GetPointer() const
{
	if (!m_refData)
		return nullptr;

	return static_cast<T*>(m_refData->GetObject());
}

template<class T>
inline uint Ref<T>::GetRefCount() const
{
	if (!m_refData)
		return 0;

	return m_refData->GetRefCount();
}

template<class T>
inline void Ref<T>::Reset()
{
	SafeDelete(m_refData);
}

template<class T>
inline bool Ref<T>::IsNull() const
{
	if (!m_refData)
		return true;

	return m_refData->IsNull();
}

ENGINE_END
