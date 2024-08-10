#pragma once

#pragma pointers_to_members(full_generality, virtual_inheritance)

#include "EngineMacro.h"
#include "FunctionConvert.h"

ENGINE_BEGIN

class Caller {};

template <class T>
class func;

// Ret: 함수의 반환 타입
// Args: 함수 파라미터 타입들
template <class Ret, class... Args>
class func<Ret(Args...)>
{
public:
	// 멤버 함수 포인터를 저장하는 타입
	using MemFunc = Ret(Caller::*)(Args...);
	// 정적 함수 포인터를 저장하는 타입
	using GloFunc = Ret(*)(Args...);
	// 생성자로 전달되는 멤버 함수 포인터 타입
	template <class Class, class Ret, class... Args>
	using In_MemFunc = Ret(Class::*)(Args...);
	// 생성자로 전달되는 정적 함수 포인터 타입
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
	// 함수를 소유한 인스턴스
	Caller* m_caller;
	// 함수 포인터
	MemFunc m_func;
};

ENGINE_END

#include "Function.hpp"
