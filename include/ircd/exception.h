#pragma once
#define HAVE_IRCD_EXCEPTION_H

/// Forward declarations for boost::system because it is not included here.
namespace boost::system
{
	struct error_code;
	struct error_category;
	struct system_error;
	namespace errc {}
}

namespace ircd
{
	struct exception; // Root exception

	// util
	bool system_category(const std::error_category &) noexcept;
	bool system_category(const std::error_code &) noexcept;
	bool system_category(const boost::system::error_code &) noexcept;
	bool is(const std::error_code &, const std::errc &) noexcept;
	bool is(const boost::system::error_code &, const std::errc &) noexcept;
	std::error_code make_error_code(const int &code = errno) noexcept;
	std::error_code make_error_code(const std::error_code &) noexcept;
	std::error_code make_error_code(const std::system_error &) noexcept;
	std::error_code make_error_code(const boost::system::error_code &) noexcept;
	std::error_code make_error_code(const boost::system::system_error &) noexcept;
	std::system_error make_system_error(const int &code = errno);
	std::system_error make_system_error(const std::errc &);
	std::system_error make_system_error(const std::error_code &);
	std::system_error make_system_error(const boost::system::error_code &);
	std::system_error make_system_error(const boost::system::system_error &);
	[[noreturn]] void throw_system_error();
	template<class... args> std::exception_ptr make_system_eptr(args&&...);
	template<class... args> [[noreturn]] void throw_system_error(args&&...);
	template<class E, class... args> std::exception_ptr make_exception_ptr(args&&...);

	string_view string(const mutable_buffer &, const std::error_code &);
	string_view string(const mutable_buffer &, const std::system_error &);
	string_view string(const mutable_buffer &, const boost::system::error_code &);
	string_view string(const mutable_buffer &, const boost::system::system_error &);
	std::string string(const std::error_code &);
	std::string string(const std::system_error &);
	std::string string(const boost::system::error_code &);
	std::string string(const boost::system::system_error &);
}

/// The root exception type.
///
/// All exceptions in the project inherit from this type. We generally don't
/// have catch blocks on this type. Instead we use `ircd::error` to catch
/// project-specific exceptions. This gives us just a little more indirection
/// to play with before inheriting from std::exception.
///
/// Not all exceptions are from project developer's code, such as
/// `std::out_of_range`, etc. It's not necessarily bad to just catch
/// `std::exception` and we do it often enough, but be considerate.
///
/// Remember: Not all exceptions have to inherit from `std::exception` either.
/// We have only one example of this: ctx::terminated. To be sure nothing can
/// possibly get through you can `catch(...)` but with extreme care that you
/// are not discarding a termination which will hang the `ircd::ctx` you're on.
///
/// !!!
/// NOTE: CONTEXT SWITCHES CANNOT OCCUR INSIDE CATCH BLOCKS UNLESS YOU USE THE
/// MITIGATION TOOLS PROVIDED IN `ircd::ctx` WHICH RESULT IN THE LOSS OF THE
/// ABILITY TO RETHROW THE EXCEPTION (i.e `throw` statement). BEST PRACTICE
/// IS TO RETURN CONTROL FROM THE CATCH BLOCK BEFORE CONTEXT SWITCHING.
/// !!!
struct ircd::exception
:virtual std::exception
{
	static constexpr const size_t BUFSIZE
	{
		512UL
	};

	IRCD_OVERLOAD(generate_skip)
	IRCD_OVERLOAD(hide_name)

  protected:
	char buf[BUFSIZE];

	ssize_t generate(const char *const &name, const string_view &fmt, const va_rtti &ap) noexcept;
	ssize_t generate(const string_view &fmt, const va_rtti &ap) noexcept;

  public:
	const char *what() const noexcept override;

	exception(generate_skip_t = {}) noexcept;
	exception(exception &&) = delete;
	exception(const exception &) = delete;
	exception &operator=(exception &&) = delete;
	exception &operator=(const exception &) = delete;
	~exception() noexcept override;
};

/// Exception generator convenience macro
///
/// If you want to create your own exception type, you have found the right
/// place! This macro allows creating an exception in the the hierarchy.
///
/// To create an exception, invoke this macro in your header. Examples:
///
///    IRCD_EXCEPTION(ircd::exception, my_exception)
///    IRCD_EXCEPTION(my_exception, my_specific_exception)
///
/// Then your catch sequence can look like the following:
///
///    catch(const my_specific_exception &e)
///    {
///        log("something specifically bad happened: %s", e.what());
///    }
///    catch(const my_exception &e)
///    {
///        log("something generically bad happened: %s", e.what());
///    }
///    catch(const ircd::exception &e)
///    {
///        log("unrelated bad happened: %s", e.what());
///    }
///    catch(const std::exception &e)
///    {
///        log("unhandled bad happened: %s", e.what());
///    }
///
/// Remember: the order of the catch blocks is important.
///
#define IRCD_EXCEPTION(parent, name)                                          \
struct [[gnu::visibility("protected")]] name                                  \
:parent                                                                       \
{                                                                             \
    template<class... args>                                                   \
    [[gnu::noinline]]                                                         \
    name(hide_name_t, const string_view &fmt, args&&... ap) noexcept          \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(fmt, ircd::va_rtti{std::forward<args>(ap)...});              \
    }                                                                         \
                                                                              \
    template<class... args>                                                   \
    [[gnu::noinline]]                                                         \
    name(hide_name_t, const string_view &fmt = " ") noexcept                  \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(fmt, ircd::va_rtti{});                                       \
    }                                                                         \
                                                                              \
    template<class... args>                                                   \
    [[gnu::noinline]]                                                         \
    name(const string_view &fmt, args&&... ap) noexcept                       \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(#name, fmt, ircd::va_rtti{std::forward<args>(ap)...});       \
    }                                                                         \
                                                                              \
    template<class... args>                                                   \
    [[gnu::noinline]]                                                         \
    name(const string_view &fmt = " ") noexcept                               \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(#name, fmt, ircd::va_rtti{});                                \
    }                                                                         \
                                                                              \
    [[using gnu: flatten, always_inline]]                                     \
    name(generate_skip_t) noexcept                                            \
    :parent{generate_skip}                                                    \
    {                                                                         \
    }                                                                         \
};

/// Hides the name of the exception when generating a string
#define IRCD_EXCEPTION_HIDENAME(parent, name)                                 \
struct [[gnu::visibility("protected")]] name                                  \
:parent                                                                       \
{                                                                             \
    template<class... args>                                                   \
    [[gnu::noinline]]                                                         \
    name(const string_view &fmt, args&&... ap) noexcept                       \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(fmt, ircd::va_rtti{std::forward<args>(ap)...});              \
    }                                                                         \
                                                                              \
    template<class... args>                                                   \
    [[gnu::noinline]]                                                         \
    name(const string_view &fmt = " ") noexcept                               \
    :parent{generate_skip}                                                    \
    {                                                                         \
        generate(fmt, ircd::va_rtti{});                                       \
    }                                                                         \
                                                                              \
    [[using gnu: flatten, always_inline]]                                     \
    name(generate_skip_t = {}) noexcept                                       \
    :parent{generate_skip}                                                    \
    {                                                                         \
    }                                                                         \
};

namespace ircd
{
	/// Root error exception type. Inherit from this.
	/// List your own exception somewhere else (unless you're overhauling libircd).
	/// example, in your namespace:
	///
	/// IRCD_EXCEPTION(ircd::error, error)
	///
	IRCD_EXCEPTION(exception, error)             // throw ircd::error("something bad")
	IRCD_EXCEPTION(error, user_error)            // throw ircd::user_error("something silly")
}

template<class E,
         class... args>
inline std::exception_ptr
ircd::make_exception_ptr(args&&... a)
try
{
	throw E{std::forward<args>(a)...};
}
catch(const E &)
{
	return std::current_exception();
};

template<class... args>
inline void
ircd::throw_system_error(args&&... a)
{
	throw make_system_error(std::forward<args>(a)...);
}

template<class... args>
inline std::exception_ptr
ircd::make_system_eptr(args&&... a)
{
	return std::make_exception_ptr(make_system_error(std::forward<args>(a)...));
}

[[gnu::always_inline]]
inline
ircd::exception::exception(generate_skip_t)
noexcept
{
	buf[0] = '\0';
}
