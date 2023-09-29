#pragma once
#define HAVE_IRCD_PANIC_H

namespace ircd
{
	void panicking(const std::exception &) noexcept;
	void panicking(const std::exception_ptr &) noexcept;
	void _aborting_() noexcept;
}

/// Creates a panic-type exception.
///
/// Throwable which will terminate on construction in debug mode but throw
/// normally in release mode. Ideally this should never be thrown in release
/// mode because the termination in debug means a test can never pass and
/// the triggering callsite should be eliminated. Nevertheless it throws
/// normally in release mode for recovering at an exception handler.
#define IRCD_PANICKING(parent, name)                                          \
struct [[gnu::visibility("protected")]] name                                  \
:parent                                                                       \
{                                                                             \
    template<class... args>                                                   \
    [[gnu::noinline, gnu::cold]]                                              \
    name(const string_view &fmt, args&&... ap) noexcept                       \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(#name, fmt, ircd::va_rtti{std::forward<args>(ap)...});       \
        ircd::panicking(*this);                                               \
    }                                                                         \
                                                                              \
    template<class... args>                                                   \
    [[gnu::noinline, gnu::cold]]                                              \
    name(const string_view &fmt = " ") noexcept                               \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(#name, fmt, ircd::va_rtti{});                                \
        ircd::panicking(*this);                                               \
    }                                                                         \
                                                                              \
    [[gnu::always_inline]]                                                    \
    name(generate_skip_t)                                                     \
    :parent{generate_skip}                                                    \
    {                                                                         \
    }                                                                         \
};

namespace ircd
{
	// panic errors; see IRCD_PANICKING docs.
	IRCD_PANICKING(exception, panic)
	IRCD_PANICKING(panic, not_implemented)
}
