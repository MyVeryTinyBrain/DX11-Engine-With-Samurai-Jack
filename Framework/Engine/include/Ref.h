#pragma once

#include "EngineMacro.h"
#include "RefData.h"

ENGINE_BEGIN
class RefData;
class Object;
template <class T>
class Ref final
{
	template <class U> friend class Ref;

public:

	// Constructors ===========================================

	explicit Ref();

	explicit Ref(RefData* refData);

	Ref(T* pointer);

	template <class U>
	Ref(U* pointer);

	Ref(const Ref& other);

	template <class U>
	Ref(const Ref<U>& other);

	Ref(Ref&& other);

	template <class U>
	Ref(Ref<U>&& other);

	// Destructor =============================================

	~Ref();

	// Operators ==============================================

	Ref& operator = (const Ref& other);

	template <class U>
	Ref& operator = (const Ref<U>& other);

	Ref& operator = (Ref&& other);

	template <class U>
	Ref& operator = (Ref<U>&& other);

	bool operator == (const Ref& other);

	bool operator != (const Ref& other);

	bool operator == (const void* ptr);

	bool operator != (const void* ptr);

	template <class U>
	bool operator == (const Ref<U>& other);

	template <class U>
	bool operator != (const Ref<U>& other);

	bool operator ! () const;

	operator T*() const;

	T* operator -> () const;

	// Member Variables =======================================

	Object* GetObject() const;

	T* GetPointer() const;

	uint GetRefCount() const;

	// Member Functions =======================================

	void Reset();

	bool IsNull() const;

private:

	// Member Variables =======================================

	RefData* m_refData;
};
ENGINE_END

#include "Ref.hpp"
