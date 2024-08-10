#pragma once

#pragma pointers_to_members(full_generality, virtual_inheritance)

#include "EngineMacro.h"
#include "FunctionConvert.h"

ENGINE_BEGIN

class Caller {};

template <class T>
class func;

// Ret: �Լ��� ��ȯ Ÿ��
// Args: �Լ� �Ķ���� Ÿ�Ե�
template <class Ret, class... Args>
class func<Ret(Args...)>
{
public:
	// ��� �Լ� �����͸� �����ϴ� Ÿ��
	using MemFunc = Ret(Caller::*)(Args...);
	// ���� �Լ� �����͸� �����ϴ� Ÿ��
	using GloFunc = Ret(*)(Args...);
	// �����ڷ� ���޵Ǵ� ��� �Լ� ������ Ÿ��
	template <class Class, class Ret, class... Args>
	using In_MemFunc = Ret(Class::*)(Args...);
	// �����ڷ� ���޵Ǵ� ���� �Լ� ������ Ÿ��
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
	// �Լ��� ������ �ν��Ͻ�
	Caller* m_caller;
	// �Լ� ������
	MemFunc m_func;
};

ENGINE_END

#include "Function.hpp"
