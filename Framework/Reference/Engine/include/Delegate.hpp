#pragma once

#include "Delegate.h"

ENGINE_BEGIN

template<class Ret, class ...Args>
inline delegate<Ret(Args...)>::delegate(delegate&& other) noexcept
{
	m_funcs = std::move(other.m_funcs);
}

template<class Ret, class ...Args>
inline delegate<Ret(Args...)>::~delegate()
{
	Clear();
}

template<class Ret, class ...Args>
inline delegate<Ret(Args...)>& delegate<Ret(Args...)>::operator=(const delegate& other)
{
	m_funcs = other.m_funcs;

	return *this;
}

template<class Ret, class ...Args>
inline delegate<Ret(Args...)>& delegate<Ret(Args...)>::operator=(delegate&& other)
{
	m_funcs = std::move(other.m_funcs);

	return *this;
}

template<class Ret, class ...Args>
inline delegate<Ret(Args...)>& delegate<Ret(Args...)>::operator+=(const func& function)
{
	Add(function);

	return *this;
}

template<class Ret, class ...Args>
inline delegate<Ret(Args...)>& delegate<Ret(Args...)>::operator-=(const func& function)
{
	Remove(function);

	return *this;
}

template<class Ret, class ...Args>
inline void delegate<Ret(Args...)>::operator()(Args ...args)
{
	Invoke(args...);
}

template<class Ret, class ...Args>
inline void delegate<Ret(Args...)>::Add(const func& function)
{
	auto find_it = std::find(m_funcs.begin(), m_funcs.end(), function);
	if (find_it == m_funcs.end())
		m_funcs.push_back(function);
}

template<class Ret, class ...Args>
inline void delegate<Ret(Args...)>::Remove(const func& function)
{
	auto find_it = std::find(m_funcs.begin(), m_funcs.end(), function);
	if (find_it != m_funcs.end()) 
		m_funcs.erase(find_it);
}

template<class Ret, class ...Args>
inline void delegate<Ret(Args...)>::Clear()
{
	m_funcs.clear();
}

template<class Ret, class ...Args>
inline void delegate<Ret(Args...)>::Invoke(Args ...args)
{
	for (auto& func : m_funcs)
	{
		func(args...);
	}
}

template<class Ret, class ...Args>
inline unsigned long delegate<Ret(Args...)>::GetFunctionCount() const
{
	return m_funcs.size();
}

template<class Ret, class ...Args>
inline const func<Ret(Args...)>& delegate<Ret(Args...)>::GetFunction(unsigned long index) const
{
	return m_funcs[index];
}

template<class Ret, class ...Args>
inline const func<Ret(Args...)>& delegate<Ret(Args...)>::GetFrontFunction() const
{
	return m_funcs.front();
}

template<class Ret, class ...Args>
inline const func<Ret(Args...)>& delegate<Ret(Args...)>::GetBackFunction() const
{
	return m_funcs.back();
}

template<class Ret, class ...Args>
inline bool delegate<Ret(Args...)>::Find(const func& function, unsigned long* pIndex) const
{
	for (unsigned long i = 0; i < m_funcs.size(); ++i)
	{
		if (m_funcs[i] == function)
		{
			*pIndex = i;
			return true;
		}
	}
	return false;
}

template<class Ref, class ...Args>
inline size_t delegate<Ref(Args...)>::GetNumFuncs() const
{
	return m_funcs.size();
}

template<class Ref, class ...Args>
inline bool delegate<Ref(Args...)>::Empty() const
{
	return m_funcs.empty();
}

ENGINE_END
