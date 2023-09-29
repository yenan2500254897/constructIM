#pragma once
#define HAVE_IRCD_UTIL_CLOSURE_H

namespace ircd {
inline namespace util
{
	template<template<class, class...>
	         class F,
	         class R,
	         class... A>
	struct closure;

	template<template<class, class...>
	         class F,
	         class... A>
	struct closure<F, bool, A...>;

	/// Template for creating callback-based iterations allowing closures of
	/// either `void (...)` or `bool (...)` to be passed to the same overload.
	///
	/// This reduces/deduplicates interfaces which offer an overload for each
	/// and thus two library symbols, usually just calling each other...
	///
	template<template<class, class...>
	         class F,
	         class... A>
	using closure_bool = closure<F, bool, A...>;

	template<class... A>
	using function_bool = closure_bool<std::function, A...>;
}}

template<template<class, class...>
         class F,
         class R,
         class... A>
struct ircd::util::closure
:F<R (A...)>
{
	using proto_type = R (A...);
	using func_type = F<proto_type>;

	using func_type::func_type;
	using func_type::operator=;
};

template<template<class, class...>
         class F,
         class... A>
struct ircd::util::closure<F, bool, A...>
:F<bool (A...)>
{
	using proto_bool_type = bool (A...);
	using proto_void_type = void (A...);

	using func_bool_type = F<proto_bool_type>;
	using func_void_type = F<proto_void_type>;

	template<class lambda>
	closure(lambda &&o,
	        typename std::enable_if<!std::is_constructible<func_bool_type, lambda>::value, int>::type = 0)
	noexcept(std::is_nothrow_invocable<lambda, void, A...>());

	template<class lambda>
	closure(lambda &&o,
	        typename std::enable_if<std::is_constructible<func_bool_type, lambda>::value, int>::type = 0)
	noexcept(std::is_nothrow_invocable<lambda, bool, A...>());

	closure() = default;
};

template<template<class, class...>
         class F,
         class... A>
template<class lambda>
[[gnu::always_inline]]
inline
ircd::util::closure<F, bool, A...>::closure(lambda &&o,
                                            typename std::enable_if<!std::is_constructible<func_bool_type, lambda>::value, int>::type)
noexcept(std::is_nothrow_invocable<lambda, void, A...>())
:F<bool (A...)>
{
	[o(std::move(o))](A&&... a)
	noexcept(std::is_nothrow_invocable<lambda, void, A...>())
	{
		static_assert
		(
			std::is_same<void, decltype(o(std::forward<A>(a)...))>()
		);

		o(std::forward<A>(a)...);
		return true;
	}
}
{}

template<template<class, class...>
         class F,
         class... A>
template<class lambda>
[[gnu::always_inline]]
inline
ircd::util::closure<F, bool, A...>::closure(lambda &&o,
                                            typename std::enable_if<std::is_constructible<func_bool_type, lambda>::value, int>::type)
noexcept(std::is_nothrow_invocable<lambda, bool, A...>())
:F<bool (A...)>
{
	std::forward<lambda>(o)
}
{}
