#pragma once

#include "EngineMacro.h"
#include "Function.h"

#include <vector>
#include <type_traits>

ENGINE_BEGIN

template <class T>
class Delegate;

template <class Ret, class... Args>
class Delegate<Ret(Args...)>
{
public:

	using Function = typename Function<Ret(Args...)>;

public:

	Delegate() = default;

	Delegate(const Delegate& other) = default;

	Delegate(Delegate&& other) noexcept;

	~Delegate();

	Delegate& operator = (const Delegate& other);

	Delegate& operator = (Delegate&& other);

	Delegate& operator += (const Function& function);

	Delegate& operator -= (const Function& function);

	void operator () (Args ...args);

	void Add(const Function& function);

	void Remove(const Function& function);

	void Clear();

	void Invoke(Args ...args);

	unsigned long GetFunctionCount() const;

	const Function& GetFunction(unsigned long index) const;

	const Function& GetFrontFunction() const;

	const Function& GetBackFunction() const;

	bool Find(const Function& function, unsigned long* pIndex) const;

private:

	std::vector<Function> m_funcs;
};

ENGINE_END

#include "Delegate.hpp"
