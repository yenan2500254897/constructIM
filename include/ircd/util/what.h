#pragma once
#define HAVE_IRCD_UTIL_WHAT_H

namespace ircd { 
    inline namespace util
    {
        string_view what(const std::exception_ptr eptr = std::current_exception()) noexcept;
        string_view what(const std::exception &) noexcept;
    }
}

inline ircd::string_view
ircd::util::what(const std::exception &e)
noexcept
{
	return e.what();
}
