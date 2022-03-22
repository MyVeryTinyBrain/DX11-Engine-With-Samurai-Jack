#pragma once

#include "EngineMacro.h"
#include "Function.h"

#include <vector>

ENGINE_BEGIN

template <class T>
class delegate;

template <class Ret, class... Args>
class delegate<Ret(Args...)>
{
public:

	using func = typename func<Ret(Args...)>;

public:

	delegate() = default;

	delegate(const delegate& other) = default;

	delegate(delegate&& other) noexcept;

	~delegate();

	delegate& operator = (const delegate& other);

	delegate& operator = (delegate&& other);

	delegate& operator += (const func& function);

	delegate& operator -= (const func& function);

	void operator () (Args ...args);

	void Add(const func& function);

	void Remove(const func& function);

	void Clear();

	void Invoke(Args ...args);

	unsigned long GetFunctionCount() const;

	const func& GetFunction(unsigned long index) const;

	const func& GetFrontFunction() const;

	const func& GetBackFunction() const;

	bool Find(const func& function, unsigned long* pIndex) const;

private:

	std::vector<func> m_funcs;
};

ENGINE_END

#include "Delegate.hpp"
