#pragma once

#include "Function.h"

ENGINE_BEGIN

template<class Ret, class ...Args>
template<class Class>
inline func<Ret(Args...)>::func(void* obj, In_MemFunc<Class, Ret, Args...> mf) :
	m_caller(reinterpret_cast<Caller*>(obj)),
	m_func(reinterpret_cast<MemFunc>(mf))
{
}

template<class Ret, class ...Args>
inline func<Ret(Args...)>::func(In_GloFunc gf) :
	m_caller(nullptr),
	m_func(FunctionUtility::ConvertFunction<MemFunc>(gf))
{
}

template<class Ret, class ...Args>
inline func<Ret(Args...)>::func(const func& other) :
	m_caller(other.m_caller),
	m_func(other.m_func)
{
}

template<class Ret, class ...Args>
inline func<Ret(Args...)>::func(func&& other) noexcept :
	m_caller(other.m_caller),
	m_func(other.m_func)
{
	other.m_caller = nullptr;
	other.m_func = nullptr;
}

template<class Ret, class ...Args>
inline func<Ret(Args...)>& func<Ret(Args...)>::operator=(const func& other)
{
	m_caller = other.m_caller;
	m_func = other.m_func;

	return *this;
}

template<class Ret, class ...Args>
inline func<Ret(Args...)>& func<Ret(Args...)>::operator=(In_GloFunc gf)
{
	m_caller = nullptr;
	m_func = FunctionUtility::ConvertFunction<MemFunc>(gf);

	return *this;
}

template<class Ret, class ...Args>
inline func<Ret(Args...)>& func<Ret(Args...)>::operator=(func&& other) noexcept
{
	m_caller = other.m_caller;
	m_func = other.m_func;

	other.m_caller = nullptr;
	other.m_func = nullptr;

	return *this;
}

template<class Ret, class ...Args>
inline bool func<Ret(Args...)>::operator==(const func& other) const
{
	return m_caller == other.m_caller && m_func == other.m_func;
}

template<class Ret, class ...Args>
inline bool func<Ret(Args...)>::operator!=(const func& other) const
{
	return !func::operator=(other);
}

template<class Ret, class ...Args>
inline Ret func<Ret(Args...)>::operator()(Args ...args)
{
	return Invoke(args...);
}

template<class Ret, class ...Args>
inline Ret func<Ret(Args...)>::Invoke(Args ...args)
{
	// Caller 클래스는 아무런 함수도 가지고 있지 않지만, 
	// 마치 본인의 함수인 것 처럼 다른 클래스의 멤버 함수를 호출합니다.
	if (m_caller) return (m_caller->*m_func)(args...);
	else return FunctionUtility::ConvertFunction<GloFunc>(m_func)(args...);
}

template<class Ret, class ...Args>
inline void* func<Ret(Args...)>::GetObject() const
{
	return m_caller;
}

template<class Ret, class ...Args>
inline void* func<Ret(Args...)>::GetFunctionAddress() const
{
	return m_func;
}

template<class Ret, class ...Args>
inline bool func<Ret(Args...)>::IsMemberFunction() const
{
	return m_caller != nullptr;
}

ENGINE_END
