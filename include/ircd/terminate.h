#pragma once
#define HAVE_IRCD_TERMINATE_H

namespace ircd
{
	struct terminate;
}

/// Always prefer ircd::terminate() to std::terminate() for all project code.
struct ircd::terminate
{
	template<class E = panic,
	         class... A>
	[[noreturn]] terminate(const string_view &, A&&...) noexcept;

	[[noreturn]] terminate(const std::exception &) noexcept;

	[[noreturn]] terminate(std::exception_ptr) noexcept;

	[[noreturn]] terminate() noexcept;

	[[noreturn]] ~terminate() noexcept;
};

template<class E,
         class... A>
[[noreturn]]
inline
ircd::terminate::terminate(const string_view &fmt,
                           A&&... ap)
noexcept
:terminate
{
	E{fmt, std::forward<A>(ap)...}
}
{
	__builtin_unreachable();
}
