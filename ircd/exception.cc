[[noreturn, gnu::cold]]
static void
ircd_terminate_handler()
noexcept
{
	std::abort();
}

[[gnu::cold]]
void
ircd::_aborting_()
noexcept
{
	std::set_terminate(&ircd_terminate_handler);
}

[[gnu::cold]]
void
ircd::panicking(const std::exception_ptr &eptr)
noexcept
{
	// log::critical
	// {
	// 	"IRCd panic :%s", what(eptr)
	// };
}

/// Called by the constructor of a panic exception when thown. We immediately
/// log a critical message, which is actually what triggers a termination when
/// assertions are enabled (!NDEBUG) otherwise a no-op.
[[gnu::cold]]
void
ircd::panicking(const std::exception &e)
noexcept
{
// 	log::critical
// 	{
// 		"IRCd panic :%s", e.what()
// 	};
}

std::string
ircd::string(const boost::system::system_error &e)
{
	return string(e.code());
}

std::string
ircd::string(const boost::system::error_code &ec)
{
	return ircd::string(128, [&ec]
	(const mutable_buffer &buf)
	{
		return string(buf, ec);
	});
}

std::string
ircd::string(const std::system_error &e)
{
	return string(e.code());
}

std::string
ircd::string(const std::error_code &ec)
{
	return ircd::string(128, [&ec]
	(const mutable_buffer &buf)
	{
		return string(buf, ec);
	});
}

ircd::string_view
ircd::string(const mutable_buffer &buf,
             const boost::system::system_error &e)
{
	return string(buf, e.code());
}

ircd::string_view
ircd::string(const mutable_buffer &buf,
             const boost::system::error_code &ec)
{
	return string(buf, make_system_error(ec));
}

ircd::string_view
ircd::string(const mutable_buffer &buf,
             const std::system_error &e)
{
	return string(buf, e.code());
}

ircd::string_view
ircd::string(const mutable_buffer &buf,
             const std::error_code &ec)
{
	return fmt::sprintf
	{
		buf, "%s :%s", ec.category().name(), ec.message()
	};
}

bool
ircd::is(const boost::system::error_code &ec,
         const std::errc &errc)
noexcept
{
	return is(make_error_code(ec), errc);
}

bool
ircd::is(const std::error_code &ec,
         const std::errc &errc)
noexcept
{
	return system_category(ec) && ec.value() == int(errc);
}

bool
ircd::system_category(const boost::system::error_code &ec)
noexcept
{
	return system_category(make_error_code(ec));
}

bool
ircd::system_category(const std::error_code &ec)
noexcept
{
	return system_category(ec.category());
}

bool
ircd::system_category(const std::error_category &ec)
noexcept
{
	return ec == std::system_category() ||
	       ec == std::generic_category() ||
	       ec == boost::system::system_category() ||
	       ec == boost::system::generic_category();
}

[[noreturn]]
void
ircd::throw_system_error()
{
    throw std::system_error
    {
		errno, std::system_category()
    };
}

std::system_error
ircd::make_system_error(const boost::system::system_error &e)
{
	return std::system_error
	{
		make_error_code(e.code()), e.what()
	};
}

std::system_error
ircd::make_system_error(const boost::system::error_code &ec)
{
	return std::system_error
	{
		make_error_code(ec)
	};
}

std::system_error
ircd::make_system_error(const std::error_code &ec)
{
	return std::system_error
	{
		make_error_code(ec)
	};
}

std::system_error
ircd::make_system_error(const std::errc &ec)
{
	return std::system_error
	{
		make_error_code(ec)
	};
}

std::system_error
ircd::make_system_error(const int &code)
{
	return std::system_error
	{
		make_error_code(code)
	};
}

std::error_code
ircd::make_error_code(const boost::system::system_error &e)
noexcept
{
	return std::error_code
	{
		e.code().value(), e.code().category()
	};
}

std::error_code
ircd::make_error_code(const boost::system::error_code &ec)
noexcept
{
	return std::error_code
	{
		ec.value(), ec.category()
	};
}

std::error_code
ircd::make_error_code(const std::system_error &e)
noexcept
{
	return e.code();
}

std::error_code
ircd::make_error_code(const int &code)
noexcept
{
	return std::error_code
	{
		code, std::generic_category()
	};
}

std::error_code
ircd::make_error_code(const std::error_code &ec)
noexcept
{
	return ec;
}

//
// exception
//

// Out-of-line placement
[[gnu::visibility("protected")]]
ircd::exception::~exception()
noexcept
{
}

[[gnu::visibility("protected")]]
ssize_t
ircd::exception::generate(const string_view &fmt,
                          const va_rtti &ap)
noexcept
{
	return fmt::vsnprintf(buf, sizeof(buf), fmt, ap);
}

[[gnu::visibility("protected")]]
ssize_t
ircd::exception::generate(const char *const &name,
                          const string_view &fmt,
                          const va_rtti &ap)
noexcept
{
	size_t size(0);
	const bool empty(!fmt || !fmt[0] || fmt[0] == ' ');
	size = strlcat(buf, name, sizeof(buf));
	size = strlcat(buf, empty? "." : " :", sizeof(buf));
	if(size < sizeof(buf))
		size += fmt::vsnprintf(buf + size, sizeof(buf) - size, fmt, ap);

	return size;
}

[[gnu::visibility("protected")]]
const char *
ircd::exception::what()
const noexcept
{
	return buf;
}

//
// terminate
//

[[noreturn, gnu::cold]]
ircd::terminate::terminate(std::exception_ptr eptr)
noexcept
{
	fprintf(stderr, "\nIRCd Terminated :%s\n", what(eptr).data());
	::fflush(stderr);
	std::terminate();
	__builtin_unreachable();
}

[[noreturn, gnu::cold]]
ircd::terminate::terminate(const std::exception &e)
noexcept
:terminate
{
	std::make_exception_ptr(e)
}
{
	__builtin_unreachable();
}

[[noreturn, gnu::cold]]
ircd::terminate::terminate()
noexcept
:terminate
{
	std::current_exception()
}
{
	__builtin_unreachable();
}

[[noreturn, gnu::cold]]
ircd::terminate::~terminate()
noexcept
{
	__builtin_unreachable();
}
