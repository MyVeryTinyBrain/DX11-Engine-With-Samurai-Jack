#pragma once

#pragma pointers_to_members(full_generality, virtual_inheritance)

#include "EngineMacro.h"
#include "FunctionConvert.h"

ENGINE_BEGIN

class Caller {};

template <class T>
class func;

template <class Ret, class... Args>
class func<Ret(Args...)>
{
public:

	using MemFunc = Ret(Caller::*)(Args...);

	using GloFunc = Ret(*)(Args...);

	template <class Class, class Ret, class... Args>
	using In_MemFunc = Ret(Class::*)(Args...);

	using In_GloFunc = GloFunc;

public:

	template <class Class>
	func(void* obj, In_MemFunc<Class, Ret, Args...> mf);

	func(In_GloFunc gf);

	func(const func& other);

	func(func&& other) noexcept;

	func& operator = (const func& other);

	func& operator = (In_GloFunc gf);

	func& operator = (func&& other) noexcept;

	bool operator == (const func& other) const;

	bool operator != (const func& other) const;

	Ret operator () (Args ...args);

	Ret Invoke(Args ...args);

	void* GetObject() const;

	void* GetFunctionAddress() const;

	bool IsMemberFunction() const;

private:

	Caller* m_caller;

	MemFunc m_func;
};

ENGINE_END

#include "Function.hpp"
